using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ArrayAllocation : Node
	{
		public Token BracketToken { get; set; }
		public PointyType ItemType { get; set; }
		public Node Size { get; set; }

		public ArrayAllocation(Token newToken, Token bracketToken, PointyType type, Node size)
			: base(NodeType.ARRAY_ALLOCATION, newToken)
		{
			this.BracketToken = bracketToken;
			this.ItemType = type;
			this.Size = size;
		}
	}
}
