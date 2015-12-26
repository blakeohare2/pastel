using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class StringConstant : Node
	{
		public string Value { get; set; }
		public bool IsChar { get; set; }

		public StringConstant(Token token, bool isChar)
			: base(token)
		{
			this.IsChar = isChar;
			this.Value = Util.GetStringValueFromStringToken(token);
		}
	}
}
