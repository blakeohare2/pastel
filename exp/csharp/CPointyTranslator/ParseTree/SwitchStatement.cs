using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class SwitchStatement : Node
	{
		public Node Expression { get; set; }
		public Node[][] Cases { get; set; }
		public int[] ResolvedCases { get; set; }
		public Node[][] Code { get; set; }

		public SwitchStatement(Token switchToken, Node expression, List<Node[]> rawCases, List<Node[]> codePerCase)
			: base(NodeType.SWITCH_STATEMENT, switchToken)
		{
			this.Expression = expression;
			this.Cases = rawCases.ToArray();
			this.Code = codePerCase.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			for (int i = 0; i < this.Cases.Length; ++i)
			{
				for (int j = 0; j < this.Cases[i].Length; ++j)
				{
					if (this.Cases[i][j] != null)
					{
						this.Cases[i][j] = this.Cases[i][j].ResolveExpression(context);
					}
				}
			}

			for (int i = 0; i < this.Code.Length; ++i)
			{
				this.Code[i] = Node.ResolveCodeChunk(context, this.Code[i]);

				bool bad = false;
				if (this.Code[i].Length == 0) bad = true;
				else
				{
					Node last = this.Code[i][this.Code[i].Length - 1];
					bad = !(last is ReturnStatement) && !(last is BreakStatement);
				}

				if (bad)
				{
					throw new ParserException(this.Token, "Fallthrough switch code.");
				}
			}

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Expression.ResolveType(context);
			if (!this.Expression.ReturnType.IsIntLike) throw new ParserException(this.Expression.Token, "Switch statement expressions must be an integer.");
			foreach (Node[] caseClusters in this.Cases)
			{
				foreach (Node caseItem in caseClusters)
				{
					if (caseItem != null)
					{
						caseItem.ResolveType(context);
						if (!caseItem.ReturnType.IsIntLike)
						{
							throw new ParserException(caseItem.Token, "Switch case value must be an integer.");
						}
					}
				}
			}

			foreach (Node[] caseCodes in this.Code)
			{
				foreach (Node caseCode in caseCodes)
				{
					caseCode.ResolveType(context);
				}
			}
		}
	}
}
