using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ConstructorInvocation : Node
	{
		public PointyType StructType { get; set; }
		public Node[] Args { get; set; }
		
		public ConstructorInvocation(Token newToken, PointyType type, List<Node> args)
			: base(NodeType.CONSTRUCTOR_INVOCATION, newToken)
		{
			this.StructType = type;
			this.Args = args.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			// TODO: verify constructor args are correct

			for (int i = 0; i < this.Args.Length; ++i)
			{
				this.Args[i] = this.Args[i].ResolveExpression(context);
			}
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			for (int i = 0; i < this.Args.Length; ++i)
			{
				this.Args[i].ResolveType(context);
			}
			// TODO: verify arg types match.

			this.ReturnType = this.StructType;
		}
	}
}
