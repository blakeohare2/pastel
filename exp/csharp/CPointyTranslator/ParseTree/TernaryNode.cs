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

		public override IList<Node> Resolve(Context context)
		{
			this.Condition = this.Condition.ResolveExpression(context);
			this.TrueCode = this.TrueCode.ResolveExpression(context);
			this.FalseCode = this.FalseCode.ResolveExpression(context);

			if (this.Condition is BooleanConstant)
			{
				return Listify(((BooleanConstant)this.Condition).Value ? this.TrueCode : this.FalseCode);
			}

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Condition.ResolveType(context);
			if (this.Condition.ReturnType.Name != "bool") throw new ParserException(this.Condition.Token, "Ternary conditions must be booleans.");
			this.TrueCode.ResolveType(context);
			this.FalseCode.ResolveType(context);
			PointyType left = this.TrueCode.ReturnType;
			PointyType right = this.FalseCode.ReturnType;

			if (!left.Equals(right)) {
				if ((left.Name == "int" || left.Name == "double") &&
					(right.Name == "int" || right.Name == "double"))
				{
					this.ReturnType = new PointyType() { Name = "double" };
					return;
				}

				throw new ParserException(this.TrueCode.Token, "Outcomes of ternary expression are not the same type.");
			}
		}
	}
}
