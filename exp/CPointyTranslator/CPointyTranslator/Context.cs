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
		private Dictionary<string, List<FunctionDefinition>> functionDefinitionsByName;
		private Dictionary<string, List<ConstructorDefinition>> constructorDefinitionsByStruct;
		private Dictionary<string, StructDefinition> structsByName;

		// key 1: struct name.
		// key 2: method name.
		// list is all method definitions matching that name. Final matching is done based on parameter matching.
		private Dictionary<string, Dictionary<string, List<FunctionDefinition>>> methodDefinitionsByStructAndName;

		private List<Dictionary<string, PointyType>> variableDeclarations = new List<Dictionary<string, PointyType>>();

		public int Tab { get; set; }
		public bool InForLoop { get; set; }
		public bool NeedsExceptionCheck { get; set; }

		public Node ActiveContext { get; set; }
		public StructDefinition ActiveContextStruct
		{
			get
			{
				if (this.ActiveContext != null)
				{
					if (this.ActiveContext is FunctionDefinition) return ((FunctionDefinition)this.ActiveContext).Parent;
					if (this.ActiveContext is ConstructorDefinition) return ((ConstructorDefinition)this.ActiveContext).Parent;
				}
				return null;
			}
		}

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
			this.NeedsExceptionCheck = false;
			this.InForLoop = false;
			this.functionDefinitionsByName = new Dictionary<string, List<FunctionDefinition>>();
			this.constructorDefinitionsByStruct = new Dictionary<string, List<ConstructorDefinition>>();
			this.methodDefinitionsByStructAndName = new Dictionary<string, Dictionary<string, List<FunctionDefinition>>>();
			this.structsByName = new Dictionary<string, StructDefinition>();
			this.cpToCType = new Dictionary<string, string>()
			{
				{ "void", "void" },
				{ "UniString", "int*" },
				{ "UniChar", "int" },
				{ "object", "void*" },
				{ "bool", "int" },
				{ "int", "int" },
				{ "List", "List*" },
			};
			this.enumValues = new Dictionary<string, Dictionary<string, int>>();
		}

		public bool IsEnumName(string name)
		{
			return this.enumValues.ContainsKey(name);
		}

		public bool IsEnumMember(string enumName, string itemName)
		{
			return this.enumValues[enumName].ContainsKey(itemName);
		}

		public int GetEnumValue(string enumName, string itemName)
		{
			return this.enumValues[enumName][itemName];
		}

		public void PushVariableScope()
		{
			this.variableDeclarations.Add(new Dictionary<string, PointyType>());
		}

		public void PopVariableScope()
		{
			if (this.variableDeclarations.Count == 0) throw new Exception(); // this should not happen.
			this.variableDeclarations.RemoveAt(this.variableDeclarations.Count - 1);
		}

		public void DeclareVariableInCurrentScope(Token variableToken, PointyType type)
		{
			string name = variableToken.Value;
			if (this.variableDeclarations.Count == 0) throw new Exception(); // this should not happen.
			foreach (Dictionary<string, PointyType> lookups in this.variableDeclarations)
			{
				if (lookups.ContainsKey(name))
				{
					throw new ParserException(variableToken, "The variable '" + name + "' was already declared in this scope or above it.");
				}
			}
			this.variableDeclarations[this.variableDeclarations.Count - 1][name] = type;
		}

		public PointyType GetVariableType(string name)
		{
			PointyType output;
			for (int i = this.variableDeclarations.Count - 1; i >= 0; --i)
			{
				if (this.variableDeclarations[i].TryGetValue(name, out output))
				{
					return output;
				}
			}
			return null;
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
			if (this.structsByName.ContainsKey(structDef.Name))
			{
				throw new ParserException(structDef.Token, "Struct by the same name defined twice.");
			}
			this.structsByName[structDef.Name] = structDef;
		}

		public bool IsStructName(string name)
		{
			return this.structsByName.ContainsKey(name);
		}

		public StructDefinition GetStruct(string name)
		{
			return this.structsByName[name];
		}

		public void RegisterFunction(FunctionDefinition funcDef)
		{
			string name = funcDef.NameToken.Value;
			if (!this.functionDefinitionsByName.ContainsKey(name))
			{
				this.functionDefinitionsByName.Add(name, new List<FunctionDefinition>());
			}

			this.functionDefinitionsByName[name].Add(funcDef);
		}

		public void RegisterConstructor(ConstructorDefinition constructorDef)
		{
			string structName = constructorDef.Parent.Name;
			if (!this.constructorDefinitionsByStruct.ContainsKey(structName))
			{
				this.constructorDefinitionsByStruct[structName] = new List<ConstructorDefinition>() { constructorDef };
			}
			else
			{
				this.constructorDefinitionsByStruct[structName].Add(constructorDef);
			}
		}

		public void RegisterMethod(FunctionDefinition methodDef)
		{
			StructDefinition sd = methodDef.Parent;
			if (sd == null) throw new Exception();

			if (!this.methodDefinitionsByStructAndName.ContainsKey(sd.Name))
			{
				this.methodDefinitionsByStructAndName[sd.Name] = new Dictionary<string, List<FunctionDefinition>>();
			}

			Dictionary<string, List<FunctionDefinition>> lookup = this.methodDefinitionsByStructAndName[sd.Name];
			string methodName = methodDef.NameToken.Value;
			if (!lookup.ContainsKey(methodName))
			{
				lookup[methodName] = new List<FunctionDefinition>() { methodDef };
				return;
			}

			// Is this needed anymore? Isn't this checked during parsing?
			foreach (FunctionDefinition prevDefs in lookup[methodName])
			{
				if (prevDefs.ArgTypeFingerprint == methodDef.ArgTypeFingerprint)
				{
					throw new ParserException(methodDef.Token, "Multiple methods with same name and argument types.");
				}
			}

			lookup[methodName].Add(methodDef);
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
