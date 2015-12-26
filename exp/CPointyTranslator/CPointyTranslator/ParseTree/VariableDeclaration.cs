using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class VariableDeclaration : Node
	{
		public PointyType Type { get; set; }
		public Token NameToken { get; set; }

		public VariableDeclaration(Token firstToken, PointyType type, Token name)
			: base(firstToken)
		{
			this.Type = type;
			this.NameToken = name;
		}
	}
}
