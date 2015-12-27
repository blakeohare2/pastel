using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class AssignmentStatement : Node
	{
		public PointyType TypeDeclaration { get; set; }
		public Node TargetExpression { get; set; }
		public Token OpToken { get; set; }
		public Node ValueExpression { get; set; }

		public AssignmentStatement(Token firstToken, Node targetExpression, Node valueExpression, Token opToken, PointyType nullableTypeDeclaration)
			: base(NodeType.ASSIGNMENT, firstToken)
		{
			this.TypeDeclaration = nullableTypeDeclaration;
			this.TargetExpression = targetExpression;
			this.OpToken = opToken;
			this.ValueExpression = valueExpression;
		}

		public override IList<Node> Resolve(Context context)
		{
			this.ValueExpression = this.ValueExpression.ResolveExpression(context);
			this.TargetExpression = this.TargetExpression.ResolveExpression(context);
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.ValueExpression.ResolveType(context);
			if (this.TypeDeclaration != null)
			{
				if (!this.TypeDeclaration.Equals(this.ValueExpression.ReturnType))
				{
					throw new ParserException(this.ValueExpression.Token, "Type mismatch. Expected " + this.TypeDeclaration.ToQuickString() + ", found " + this.ValueExpression.ReturnType.ToQuickString());
				}

				if (this.TargetExpression is Variable)
				{
					context.DeclareVariableInCurrentScope(this.TargetExpression.Token, this.TypeDeclaration);
				}
			}
			else
			{
				this.TargetExpression.ResolveType(context);
			}
		}
	}
}
