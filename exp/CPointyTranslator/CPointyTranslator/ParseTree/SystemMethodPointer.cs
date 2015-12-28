using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class SystemMethodPointer : Node
	{
		public string Name { get; set; }

		public SystemMethodPointer(Token token, string methodName, PointyType type)
			: base(NodeType.SYSTEM_METHOD_POINTER, token)
		{
			this.ReturnType = type;
			this.Name = methodName;
		}
		public override IList<Node> Resolve(Context context)
		{
			throw new Exception(); // created during resolution. should not be called.
		}

		public override void ResolveType(Context context)
		{
			throw new Exception(); // created during resolution. should not be called.
		}
	}
}
