using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator
{
	public class Token
	{
		public string FileName { get; set; }
		public int Line { get; set; }
		public int Column { get; set; }
		public string Value { get; set; }
	}
}
