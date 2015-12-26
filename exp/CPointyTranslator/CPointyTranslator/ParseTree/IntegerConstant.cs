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
			: base(token)
		{
			this.Value = value;
		}
	}
}
