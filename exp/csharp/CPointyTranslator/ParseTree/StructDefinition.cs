﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public class StructDefinition : Node
	{
		public string Name { get; private set; }
		public Dictionary<string, PointyType> TypesByName { get; private set; }
		public Dictionary<string, Token> FieldTokens { get; private set; }
		public string[] FieldOrder { get; private set; }
		public ConstructorDefinition[] Constructors { get; private set; }
		public FunctionDefinition[] Methods { get; private set; }

		public StructDefinition(Token structToken, string name, List<Token> fieldTokens, List<PointyType> types, IList<ConstructorDefinition> constructors, IList<FunctionDefinition> methods)
			: base(NodeType.STRUCT_DECLARATION, structToken)
		{
			if (constructors.Count == 0)
			{
				constructors = new ConstructorDefinition[] { 
					new ConstructorDefinition(structToken, new PointyType[0], new Token[0], new Node[0]) { Parent = this } 
				};
			}
			Dictionary<string, Token> tokensByName = new Dictionary<string, Token>();
			Dictionary<string, PointyType> typesByName = new Dictionary<string, PointyType>();
			List<string> fieldOrder = new List<string>();
			for (int i = 0; i < fieldTokens.Count; ++i)
			{
				Token token = fieldTokens[i];
				string fieldName = token.Value;
				PointyType type = types[i];
				tokensByName[fieldName] = token;
				typesByName[fieldName] = type;
				fieldOrder.Add(fieldName);
			}
			this.TypesByName = typesByName;
			this.FieldTokens = tokensByName;
			this.FieldOrder = fieldOrder.ToArray();
			this.Name = name;
			this.Constructors = constructors.ToArray();
			this.Methods = methods.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			// Resolve is called directly on members from the exporter

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			// ResolveType is called directly on members from the exporter
		}
	}
}
