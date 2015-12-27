using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class NullConstant : Node
	{
		public NullConstant(Token token) : base(NodeType.NULL_CONSTANT, token) { }
	}
}
