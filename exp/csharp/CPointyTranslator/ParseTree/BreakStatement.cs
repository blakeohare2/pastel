using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class BreakStatement : Node
	{

		public BreakStatement(Token breakToken)
			: base(NodeType.BREAK, breakToken)
		{ }

		public override IList<Node> Resolve(Context context)
		{
			return Listify(this);
		}

		public override void ResolveType(Context context) { }
	}
}
