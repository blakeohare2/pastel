using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class NullConstant : Node
	{
		public NullConstant(Token token) : base(NodeType.NULL_CONSTANT, token) { }

		public override IList<Node> Resolve(Context context)
		{
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.ReturnType = PointyType.NULL_POINTER;
		}
	}
}
