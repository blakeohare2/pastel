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
		public string SystemMethodPointerHint { get; set; }

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
			else if (this.SystemMethodPointerHint != null)
			{
				return Listify(new SystemMethodPointer(this.Token, this.SystemMethodPointerHint, this.ReturnType));
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
			else if (this.Name[0] == '$')
			{
				this.SystemMethodPointerHint = this.Name.Substring(1);
				switch (this.SystemMethodPointerHint)
				{
					case "print":
					case "println":
						this.ReturnType = PointyType.VOID;
						return;
				}
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
