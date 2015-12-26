using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class IfStatement : Node
	{
		public Node Condition { get; set; }
		public Node[] TrueCode { get; set; }
		public Node[] FalseCode { get; set; }

		public IfStatement(Token ifToken, Node condition, List<Node> trueCode, List<Node> falseCode)
			: base(ifToken)
		{
			this.Condition = condition;
			this.TrueCode = trueCode.ToArray();
			this.FalseCode = falseCode.ToArray();
		}
	}
}
