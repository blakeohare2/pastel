using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class TernaryNode : Node
	{
		public Node Condition { get; set; }
		public Node TrueCode { get; set; }
		public Node FalseCode { get; set; }

		public TernaryNode(Node condition, Node trueCode, Node falseCode)
			: base(NodeType.TERNARY, condition.Token)
		{
			this.Condition = condition;
			this.TrueCode = trueCode;
			this.FalseCode = falseCode;
		}
	}
}
