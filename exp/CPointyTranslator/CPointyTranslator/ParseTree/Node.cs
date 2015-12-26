using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public class Node
	{
		public Token Token { get; private set; }
		public Node(Token token)
		{
			this.Token = token;
		}
	}
}
