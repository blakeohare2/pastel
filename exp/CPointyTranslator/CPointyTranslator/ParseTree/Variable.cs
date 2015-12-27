using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class Variable : Node
	{
		public string Name { get; set; }

		private StructDefinition ValueOfThis { get; set; }

		public Variable(Token token, string name)
			: base(NodeType.VARIABLE, token)
		{
			this.Name = name;
		}

		public override IList<Node> Resolve(Context context)
		{
			if (this.ValueOfThis != null)
			{
				return Listify(new ThisConstant(this.Token, this.ValueOfThis));
			}

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			if (this.Name == "this")
			{
				StructDefinition sd = context.ActiveContextStruct;
				if (sd == null)
				{
					throw new ParserException(this.Token, "Cannot use 'this' outside of a struct method.");
				}
				this.ValueOfThis = sd;
				this.ReturnType = new PointyType() { Name = sd.Name };
				return;
			}

			PointyType t = context.GetVariableType(this.Name);
			if (t == null)
			{
				throw new ParserException(this.Token, "Variable used before it was declared.");
			}
			this.ReturnType = t;
		}
	}
}
