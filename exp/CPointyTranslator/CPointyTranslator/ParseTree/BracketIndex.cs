using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class BracketIndex : Node
	{
		public Node Root { get; set; }
		public Token BracketToken { get; set; }
		public Node Index { get; set; }

		public BracketIndex(Node root, Token bracketToken, Node index)
			: base(NodeType.ARRAY_INDEX, root.Token)
		{
			this.Root = root;
			this.BracketToken = bracketToken;
			this.Index = index;
		}
	}
}
