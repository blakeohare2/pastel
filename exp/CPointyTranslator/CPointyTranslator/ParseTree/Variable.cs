using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class Variable : Node
	{
		public string Name { get; set; }

		public Variable(Token token, string name)
			: base(NodeType.VARIABLE, token)
		{
			this.Name = name;
		}
	}
}
