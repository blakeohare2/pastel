using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class SwitchStatement : Node
	{
		public Node Expression { get; set; }
		public Node[][] Cases { get; set; }
		public int[] ResolvedCases { get; set; }
		public Node[][] Code { get; set; }

		public SwitchStatement(Token switchToken, Node expression, List<Node[]> rawCases, List<Node[]> codePerCase)
			: base(NodeType.SWITCH_STATEMENT, switchToken)
		{
			this.Expression = expression;
			this.Cases = rawCases.ToArray();
			this.Code = codePerCase.ToArray();
		}
	}
}
