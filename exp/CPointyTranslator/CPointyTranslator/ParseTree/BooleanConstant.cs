using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class BooleanConstant : Node
	{
		public bool Value { get; set; }

		public BooleanConstant(Token token, bool value)
			: base(NodeType.BOOLEAN_CONSTANT, token)
		{
			this.Value = value;
		}
	}
}
