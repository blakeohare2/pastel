using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class FunctionInvocation : Node
	{
		public Node Root { get; set; }
		public Token ParenToken { get; set; }
		public Node[] Args { get; set; }

		public FunctionInvocation(Node root, Token parenToken, IList<Node> args)
			: base(root.Token)
		{
			this.Root = root;
			this.ParenToken = parenToken;
			this.Args = args.ToArray();
		}
	}
}
