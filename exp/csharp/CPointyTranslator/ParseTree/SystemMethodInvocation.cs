using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class SystemMethodInvocation : Node
	{
		public string MethodId { get; set; }
		public Node[] Args { get; set; }

		public SystemMethodInvocation(Token token, string name, IList<Node> args)
			: base(NodeType.SYSTEM_METHOD_INVOCATION, token)
		{
			this.MethodId = name;
			this.Args = args.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			return Listify(this); // This is the result of a resolution pass, so don't do any double-resolving.
		}

		public override void ResolveType(Context context)
		{
			throw new Exception(); // This is the result of the .Resolve() pass and so .ResolveType() should never be called. Must manually set the return type.
		}
	}
}
