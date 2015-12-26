using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class DotField : Node
	{
		public Node Root { get; set; }
		public Token DotToken { get; set; }
		public Token FieldToken { get; set; }

		public DotField(Node root, Token dotToken, Token fieldToken)
			: base(root.Token)
		{
			this.Root = root;
			this.DotToken = dotToken;
			this.FieldToken = fieldToken;
		}
	}
}
