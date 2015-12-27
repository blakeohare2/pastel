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

		public override IList<Node> Resolve(Context context)
		{
			// TODO: lots

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			PointyType t = context.GetVariableType(this.Name);
			if (t == null)
			{
				throw new ParserException(this.Token, "Variable used before it was declared.");
			}
			this.ReturnType = t;
		}
	}
}
