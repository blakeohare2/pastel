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
			string directory = @"C:\Things\Pastel\exp\cpointy";

			string[] files = System.IO.Directory.GetFiles(directory).Where<string>(f => f.ToLowerInvariant().EndsWith(".cp")).ToArray();

			List<TokenStream> tokenStreams = new List<TokenStream>();
			foreach (string file in files)
			{
				tokenStreams.Add(Tokenizer.Tokenize(file, System.IO.File.ReadAllText(file)));
			}

			List<Node> nodes = ParseTree.Parser.Parse(tokenStreams);
		}
	}
}
