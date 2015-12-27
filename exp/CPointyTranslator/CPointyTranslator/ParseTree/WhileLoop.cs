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
			: base(NodeType.WHILE_LOOP, whileToken)
		{
			this.Condition = condition;
			this.Code = code.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			this.Condition = this.Condition.ResolveExpression(context);
			this.Code = Node.ResolveCodeChunk(context, this.Code);

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Condition.ResolveType(context);
			if (this.Condition.ReturnType.Name != "bool") throw new ParserException(this.Condition.Token, "While loop condition must be a boolean.");
			foreach (Node node in this.Code)
			{
				node.ResolveType(context);
			}
		}
	}
}
