using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class BinaryOpChain : Node
	{
		public Token[] OpTokens { get; set; }
		public Node[] Expressions { get; set; }

		public BinaryOpChain(List<Node> expressions, List<Token> ops)
			: base(NodeType.BINARY_OP_CHAIN, expressions[0].Token)
		{
			this.Expressions = expressions.ToArray();
			this.OpTokens = ops.ToArray();
		}
	}
}
