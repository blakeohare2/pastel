using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class Cast : Node
	{
		public PointyType CastTo { get; set; }
		public Node Expression { get; set; }

		public Cast(Token token, PointyType castTo, Node expression)
			: base(NodeType.CAST, token)
		{
			this.CastTo = castTo;
			this.Expression = expression;
			this.ReturnType = castTo;
		}

		public override IList<Node> Resolve(Context context)
		{
			throw new Exception();
		}

		public override void ResolveType(Context context)
		{
			throw new Exception();
		}
	}
}
