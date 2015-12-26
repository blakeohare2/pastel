using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class WhileLoop : Node
	{
		public Node Condition { get; set; }
		public Node[] Code { get; set; }

		public WhileLoop(Token whileToken, Node condition, List<Node> code)
			: base(whileToken)
		{
			this.Condition = condition;
			this.Code = code.ToArray();
		}
	}
}
