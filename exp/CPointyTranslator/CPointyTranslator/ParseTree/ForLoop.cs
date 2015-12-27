using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ForLoop : Node
	{
		public Node[] Init { get; set; }
		public Node Condition { get; set; }
		public Node[] Step { get; set; }
		public Node[] Code { get; set; }

		public ForLoop(Token forToken, List<Node> init, Node condition, List<Node> step, List<Node> code)
			: base(NodeType.FOR_LOOP, forToken)
		{
			this.Init = init.ToArray();
			this.Condition = condition;
			this.Step = step.ToArray();
			this.Code = code.ToArray();
		}
	}
}
