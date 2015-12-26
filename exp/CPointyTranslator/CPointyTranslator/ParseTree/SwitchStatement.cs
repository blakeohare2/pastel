using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class SwitchStatement : Node
	{
		public Node Expression { get; set; }
		public List<Node>[] Cases { get; set; }
		public int[] ResolvedCases { get; set; }
		public bool HasDefault { get; set; }
		public List<Node>[] Code { get; set; }
		public Node[] DefaultCode { get; set; }

		public SwitchStatement(Token switchToken, Node expression, List<List<Node>> rawCases, List<List<Node>> codePerCase, bool hasDefault, List<Node> defaultCode)
			: base(switchToken)
		{
			this.Expression = expression;
			this.Cases = rawCases.ToArray();
			this.Code = codePerCase.ToArray();
			this.HasDefault = hasDefault;
			this.DefaultCode = defaultCode.ToArray();
		}
	}
}
