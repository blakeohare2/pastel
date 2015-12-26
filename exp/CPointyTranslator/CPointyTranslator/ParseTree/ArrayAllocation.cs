using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ArrayAllocation : Node
	{
		public Token BracketToken { get; set; }
		public PointyType Type { get; set; }
		public Node Size { get; set; }

		public ArrayAllocation(Token newToken, Token bracketToken, PointyType type, Node size)
			: base(newToken)
		{
			this.BracketToken = bracketToken;
			this.Type = type;
			this.Size = size;
		}
	}
}
