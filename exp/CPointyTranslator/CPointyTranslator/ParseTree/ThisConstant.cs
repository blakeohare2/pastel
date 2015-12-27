using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ThisConstant : Node
	{
		public StructDefinition StructDef { get; set; }

		public ThisConstant(Token token, StructDefinition structDef)
			: base(NodeType.THIS, token)
		{
			this.StructDef = structDef;
		}

		public override IList<Node> Resolve(Context context)
		{
			throw new Exception(); // This is generated as a result of the resolver so should not be double-called.
		}

		public override void ResolveType(Context context)
		{
			throw new Exception(); // this is generated in the resolve pass, so this should never be called. 
		}
	}
}
