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
			: base(NodeType.NEGATE, opToken)
		{
			this.Expression = expression;
		}

		public override IList<Node> Resolve(Context context)
		{
			// TODO: more

			this.Expression = this.Expression.ResolveExpression(context);
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Expression.ResolveType(context);
			PointyType t = this.Expression.ReturnType;
			if (this.Token.Value == "-")
			{
				if (t.Name == "int" || t.Name == "double")
				{
					this.ReturnType = t;
					return;
				}
			}
			else if (this.Token.Value == "!")
			{
				if (t.Name == "bool")
				{
					this.ReturnType = t;
					return;
				}
			}

			throw new ParserException(this.Token, "The '" + this.Token.Value + "' operator cannot be applied to this type.");
		}
	}
}
