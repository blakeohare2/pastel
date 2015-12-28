using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class IfStatement : Node
	{
		public Node Condition { get; set; }
		public Node[] TrueCode { get; set; }
		public Node[] FalseCode { get; set; }

		public IfStatement(Token ifToken, Node condition, List<Node> trueCode, List<Node> falseCode)
			: base(NodeType.IF_STATEMENT, ifToken)
		{
			this.Condition = condition;
			this.TrueCode = trueCode.ToArray();
			this.FalseCode = falseCode.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			this.Condition = this.Condition.ResolveExpression(context);
			this.TrueCode = Node.ResolveCodeChunk(context, this.TrueCode);
			this.FalseCode = Node.ResolveCodeChunk(context, this.FalseCode);

			if (this.Condition is BooleanConstant)
			{
				return ((BooleanConstant)this.Condition).Value ? this.TrueCode : this.FalseCode;
			}

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Condition.ResolveType(context);
			if (this.Condition.ReturnType.Name != "bool") throw new ParserException(this.Condition.Token, "If statement conditions must be booleans.");

			context.PushVariableScope();
			foreach (Node node in this.TrueCode) node.ResolveType(context);
			context.PopVariableScope();

			context.PushVariableScope();
			foreach (Node node in this.FalseCode) node.ResolveType(context);
			context.PopVariableScope();
		}
	}
}
