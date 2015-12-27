using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CPointyTranslator.ParseTree;

namespace CPointyTranslator
{
	public class Context
	{
		private Dictionary<string, string> cpToCType;
		private Dictionary<string, Dictionary<string, int>> enumValues;

		public int Tab { get; set; }
		public bool InForLoop { get; set; }

		private static Dictionary<int, string> tabs = new Dictionary<int, string>();
		public string Tabs
		{
			get
			{
				string output;
				if (!tabs.TryGetValue(this.Tab, out output))
				{
					output = "";
					for (int i = 0; i < this.Tab; ++i)
					{
						output += "\t";
					}
					tabs[this.Tab] = output;
				}
				return output;
			}
		}

		public Context()
		{
			this.InForLoop = false;
			this.cpToCType = new Dictionary<string, string>()
			{
				{ "UniString", "int*" },
				{ "object", "void*" },
				{ "bool", "int" },
				{ "int", "int" },
			};
			this.enumValues = new Dictionary<string, Dictionary<string, int>>();
		}

		public string CifyType(PointyType type)
		{
			int starCount = 0;
			while (type.Name == "Array")
			{
				type = type.Generics[0];
				starCount++;
			}

			string output;
			if (!this.cpToCType.TryGetValue(type.Name, out output))
			{
				throw new ParserException(type.NameToken, "This type isn't decared anywhere.");
			}
			while (starCount-- > 0)
			{
				output += "*";
			}
			return output;
		}

		public void RegisterStruct(StructDefinition structDef)
		{
			this.cpToCType.Add(structDef.Name, structDef.Name + "*");
		}

		public void RegsterEnum(EnumDeclaration enumDef)
		{
			HashSet<int> assignedValues = new HashSet<int>();
			Dictionary<string, int> values = new Dictionary<string, int>();
			int hasExplicitValues = 0; // 0 - I don't know, 1 - yes, 2 - no.
			int i = 0;
			foreach (string item in enumDef.Values.Keys.OrderBy<string, string>(k => k.ToLower()))
			{
				Node value = enumDef.Values[item];
				if (value == null)
				{
					if (hasExplicitValues == 0)
					{
						hasExplicitValues = 2;
					}
					if (hasExplicitValues == 1)
					{
						throw new ParserException(enumDef.Token, "All values must be declared or none of them.");
					}
					values[item] = ++i;
				}
				else
				{
					if (hasExplicitValues == 0)
					{
						hasExplicitValues = 1;
					}
					if (hasExplicitValues == 2)
					{
						throw new ParserException(enumDef.Token, "All values must be declared or none of them.");
					}
					if (value is IntegerConstant)
					{
						int intValue = ((IntegerConstant) value).Value;
						if (assignedValues.Contains(intValue))
						{
							throw new ParserException(value.Token, "Integer value used twice for same enum.");
						}

						assignedValues.Add(intValue);
						values[item] = intValue;
					}
					else
					{
						throw new ParserException(value.Token, "Only integer constants or expressions that resolve to constants may be used as enum values.");
					}
				}
			}
			string name = enumDef.NameToken.Value;
			this.enumValues[name] = values;
		}
	}
}
