using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public class EnumDeclaration : Node
	{
		public Dictionary<string, Node> Values { get; set; }
		public Token NameToken { get; set; }

		public EnumDeclaration(Token enumToken, Token nameToken, IList<Token> names, IList<Node> values)
			: base(NodeType.ENUM_DEFINITION, enumToken)
		{
			this.NameToken = nameToken;
			this.Values = new Dictionary<string, Node>();
			for (int i = 0; i < names.Count; ++i)
			{
				Token itemNameToken = names[i];
				Node itemValue = values[i];
				string itemName = itemNameToken.Value;
				if (this.Values.ContainsKey(itemName))
				{
					throw new ParserException(itemNameToken, "Enum value declared multiple times.");
				}
				this.Values[itemName] = itemValue;
			}
		}
	}
}
