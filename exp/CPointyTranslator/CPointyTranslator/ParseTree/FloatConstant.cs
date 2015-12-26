﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class FloatConstant : Node
	{
		public double Value { get; set; }

		public FloatConstant(Token token, double value)
			: base(token)
		{
			this.Value = value;
		}
	}
}