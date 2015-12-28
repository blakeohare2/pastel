using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator
{
	public class CompilerException : Exception
	{
		public CompilerException(string message) : base(message) { }
	}
}
