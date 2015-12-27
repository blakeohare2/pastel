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

		public override IList<Node> Resolve(Context context)
		{
			for (int i = 0; i < this.Init.Length; ++i)
			{
				this.Init[i] = this.Init[i].ResolveExpression(context);
				if (context.NeedsExceptionCheck) throw new ParserException(this.Init[i].Token, "Cannot have expression that throws here.");
			}

			for (int i = 0; i < this.Step.Length; ++i)
			{
				this.Step[i] = this.Step[i].ResolveExpression(context);
				if (context.NeedsExceptionCheck) throw new ParserException(this.Step[i].Token, "Cannot have expression that throws here.");
			}

			this.Condition = this.Condition.ResolveExpression(context);

			this.Code = Node.ResolveCodeChunk(context, this.Code);

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			context.PushVariableScope();
			foreach (Node node in this.Init)
			{
				node.ResolveType(context);
			}

			this.Condition.ResolveType(context);
			if (this.Condition.ReturnType.Name != "bool")
			{
				throw new ParserException(this.Condition.Token, "For loop condition must be a boolean.");
			}
			foreach (Node node in this.Step)
			{
				node.ResolveType(context);
			}

			foreach (Node node in this.Code)
			{
				node.ResolveType(context);
			}

			context.PopVariableScope();
		}
	}
}
