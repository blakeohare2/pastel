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
			: base(firstToken)
		{
			this.TypeDeclaration = nullableTypeDeclaration;
			this.TargetExpression = targetExpression;
			this.OpToken = opToken;
			this.ValueExpression = valueExpression;
		}
	}
}
