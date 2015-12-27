using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CPointyTranslator.ParseTree;

namespace CPointyTranslator
{
	// Sort nodes by type dependencies
	class NodeSorter
	{

		public static List<StructDefinition> SortByTypeDependency(List<StructDefinition> structs, ICollection<EnumDeclaration> enums)
		{
			List<StructDefinition> output = new List<StructDefinition>();
			HashSet<string> resolvedTypes = new HashSet<string>("int object UniString UniChar char bool".Split(' '));
			Dictionary<string, List<string>> dependencies = new Dictionary<string, List<string>>();
			foreach (StructDefinition sd in structs)
			{
				dependencies.Add(sd.Name, GenerateDependencies(sd));
			}

			foreach (EnumDeclaration e in enums)
			{
				resolvedTypes.Add(e.NameToken.Value);
			}

			while (structs.Count > 0)
			{
				bool anyRemoved = false;
				for (int i = structs.Count - 1; i >= 0; --i)
				{
					StructDefinition  sd = structs[i];
					bool allDependenciesCovered = true;
					foreach (string dependency in dependencies[sd.Name])
					{
						if (!resolvedTypes.Contains(dependency))
						{
							allDependenciesCovered = false;
							break;
						}
					}

					if (allDependenciesCovered)
					{
						structs.RemoveAt(i);
						anyRemoved = true;
						output.Add(sd);
						resolvedTypes.Add(sd.Name);
					}
				}

				if (!anyRemoved)
				{
					throw new ParserException(structs[0].Token, "Circular dependency detected.");
				}
			}

			return output;
		}

		private static List<string> GenerateDependencies(StructDefinition sd)
		{
			List<string> output = new List<string>();
			foreach (PointyType pt in sd.TypesByName.Values)
			{
				PointyType effectiveType = pt;
				while (effectiveType.Name == "Array") 
				{
					effectiveType = effectiveType.Generics[0];
				}
				output.Add(effectiveType.Name);
			}

			return output;
		}
	}

}
