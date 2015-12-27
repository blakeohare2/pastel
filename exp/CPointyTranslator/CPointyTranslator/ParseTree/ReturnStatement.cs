using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ReturnStatement : Node
	{
		public Node Expression { get; set; }
		public ReturnStatement(Token returnToken, Node expression)
			: base(NodeType.RETURN, returnToken)
		{
			this.Expression = expression;
		}
	}
}
