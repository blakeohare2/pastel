using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ArrayAllocation : Node
	{
		public Token BracketToken { get; set; }
		public PointyType ItemType { get; set; }
		public Node Size { get; set; }

		public ArrayAllocation(Token newToken, Token bracketToken, PointyType type, Node size)
			: base(NodeType.ARRAY_ALLOCATION, newToken)
		{
			this.BracketToken = bracketToken;
			this.ItemType = type;
			this.Size = size;
		}

		public override IList<Node> Resolve(Context context)
		{
			this.Size = this.Size.Resolve(context)[0];
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Size.ResolveType(context);
			if (this.Size.ReturnType.Name != "int")
			{
				throw new ParserException(this.Size.Token, "Expected integer. Found '" + this.Size.ReturnType.ToQuickString() + "'");
			}
			this.ReturnType = PointyType.NewArrayType(this.ItemType);
		}
	}
}
