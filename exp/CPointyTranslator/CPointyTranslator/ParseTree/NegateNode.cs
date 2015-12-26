using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class NegateNode : Node
	{
		public Node Expression { get; set; }

		public NegateNode(Token opToken, Node expression)
			: base(opToken)
		{
			this.Expression = expression;
		}
	}
}
