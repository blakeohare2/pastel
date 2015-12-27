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

		public override IList<Node> Resolve(Context context)
		{
			if (this.Expression != null) this.Expression = this.Expression.ResolveExpression(context);
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			if (this.Expression != null)
			{
				this.Expression.ResolveType(context);
			}
		}
	}
}
