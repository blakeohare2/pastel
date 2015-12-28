using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CPointyTranslator.ParseTree;

namespace CPointyTranslator
{
	class Program
	{
		static void Main(string[] args)
		{
#if DEBUG
			string buildFile = @"C:\Things\Pastel\exp\cpointy\Crayon.cpbuild";
			Run(buildFile);
#else
			// not tested yet
			string buildFile = args[0];
			try
			{
				Run(buildFile);
			}
			catch (Exception e)
			{
				System.Console.WriteLine(e.Message);
			}

#endif
			
		}

		private static void Run(string buildFile)
		{
			if (!System.IO.File.Exists(buildFile)) throw new CompilerException(buildFile + " could not be found.");
			
			string directory = System.IO.Path.GetDirectoryName(buildFile);

			List<string> sourceCodeFiles = new List<string>();
			Dictionary<string, string> dirsToCopy = new Dictionary<string, string>();
			string outputDirectory = null;
			string[] buildFileContents = System.IO.File.ReadAllText(buildFile).Split('\n');
			
			for (int i = 0; i < buildFileContents.Length; ++i) 
			{
				string line = buildFileContents[i].Trim();
				if (line.Length > 0)
				{
					string[] parts = line.Split(' ');
					switch (parts[0].ToLowerInvariant())
					{
						// defines a folder with source code in it.
						case "source":
							if (parts.Length != 2) throw new CompilerException("Build file, Line " + (i + 1) + ": malformed source declaration.");
							string fullpath = System.IO.Path.Combine(directory, parts[1]);
							if (!System.IO.Directory.Exists(fullpath)) throw new CompilerException("Build file, Line " + (i + 1) + ": source directory not found.");
							sourceCodeFiles.AddRange(System.IO.Directory.GetFiles(fullpath).Where<string>(f => f.ToLowerInvariant().EndsWith(".cp")));
							break;

						case "output-to":
							if (parts.Length != 2) throw new CompilerException("Build file, Line " + (i + 1) + ": malformed output-to declaration.");
							outputDirectory = parts[1];
							break;

						case "copydir":
							if (parts.Length != 3) throw new CompilerException("Build file, Line " + (i + 1) + ": malformed copydir declration.");
							string copySource = System.IO.Path.Combine(directory, parts[1]).Replace('/', '\\');
							if (!System.IO.Directory.Exists(copySource))
							{
								throw new CompilerException("Build file, Line " + (i + 1) + ": directory to copy does not exist.");
							}
							dirsToCopy[copySource] = parts[2];
							break;

						default:
							throw new CompilerException("Build file, Line " + (i + 1) + ": unknown command.");
					}
				}
			}

			if (outputDirectory == null) throw new CompilerException("Build file does not declare an output directory.");
			if (sourceCodeFiles.Count == 0) throw new CompilerException("Build file does not declare any source files.");

			foreach (string key in dirsToCopy.Keys.ToArray())
			{
				dirsToCopy[key] = Util.PathCombineAndCollapse(directory, outputDirectory, dirsToCopy[key]);
			}

			List<TokenStream> tokenStreams = new List<TokenStream>();
			foreach (string file in sourceCodeFiles)
			{
				tokenStreams.Add(Tokenizer.Tokenize(file, System.IO.File.ReadAllText(file)));
			}

			List<Node> nodes = ParseTree.Parser.Parse(tokenStreams);

			Dictionary<string, string> output = Exporter.Export(nodes);
			foreach (string file in output.Keys)
			{
				string fullPath = Util.PathCombineAndCollapse(directory, outputDirectory, file);
				Util.WriteAllTextAndEnsureDirectoryExists(fullPath, output[file]);
			}

			foreach (string copyDir in dirsToCopy.Keys)
			{
				string[] files = Util.GetAllFilesRecursively(copyDir);
				string targetDir = dirsToCopy[copyDir];
				foreach (string file in files)
				{
					byte[] contents = System.IO.File.ReadAllBytes(System.IO.Path.Combine(copyDir, file));
					string finalPath = Util.PathCombineAndCollapse(targetDir, file);
					Util.WriteAllBytesAndEnsureDirectoryExists(finalPath, contents);
				}
			}
		}
	}
}
