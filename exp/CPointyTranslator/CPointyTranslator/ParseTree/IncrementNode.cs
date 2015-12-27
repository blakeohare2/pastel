using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class IncrementNode : Node
	{
		public Token IncrementToken { get; set; }
		public Node Expression { get; set; }
		public bool IsPrefix { get; set; }

		public IncrementNode(Token token, Node expression, Token incrementToken, bool isPrefix)
			: base(NodeType.INCREMENT, token)
		{
			this.IncrementToken = incrementToken;
			this.Expression = expression;
			this.IsPrefix = isPrefix;
		}

		public override IList<Node> Resolve(Context context)
		{
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Expression.ResolveType(context);
			if (this.Expression.ReturnType.Name != "int") throw new ParserException(this.IncrementToken, this.IncrementToken.Value + " can only be applied to integer types.");
			if (this.Expression is Variable || this.Expression is ArrayIndex || this.Expression is DotField)
			{
				// this is fine.
			}
			else
			{
				throw new ParserException(this.IncrementToken, this.IncrementToken.Value + " can only be applied to variables or array/struct dereferences.");
			}
		}
	}
}
