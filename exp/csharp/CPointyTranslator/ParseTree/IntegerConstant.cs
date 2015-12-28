using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class IntegerConstant : Node
	{
		public int Value { get; set; }

		public IntegerConstant(Token token, int value)
			: base(NodeType.INTEGER_CONSTANT, token)
		{
			this.Value = value;
		}

		public override IList<Node> Resolve(Context context)
		{
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.ReturnType = PointyType.INTEGER;
		}
	}
}
