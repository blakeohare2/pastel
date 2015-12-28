using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator
{
	class ParserException : Exception
	{
		public ParserException(Token token, string message)
			: base(GetLocation(token) + message)
		{
		}

		private static string GetLocation(Token token)
		{
			if (token == null) return "";

			return token.FileName + " Line " + (token.Line + 1) + " Column: " + (token.Column + 1) + ": ";
		}
	}
}
