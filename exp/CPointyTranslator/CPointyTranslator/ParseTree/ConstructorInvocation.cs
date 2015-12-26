using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ConstructorInvocation : Node
	{
		public PointyType Type { get; set; }
		public Node[] Args { get; set; }
		public ConstructorInvocation(Token newToken, PointyType type, List<Node> args)
			: base(newToken)
		{
			this.Type = type;
			this.Args = args.ToArray();
		}
	}
}
