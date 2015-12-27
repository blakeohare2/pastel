using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class IncrementNode : Node
	{
		public Token IncrementToken { get; set; }
		public Node Expression { get; set; }
		public bool IsPrefix { get; set; }

		public IncrementNode(Token token, Node expression, Token incrementToken, bool isPrefix)
			: base(NodeType.INCREMENT, token)
		{
			this.IncrementToken = incrementToken;
			this.Expression = expression;
			this.IsPrefix = isPrefix;
		}
	}
}
