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

		public ConstructorDefinition ConstructorDefinition { get; set; }
		public string SystemConstructorHint { get; set; }

		public ConstructorInvocation(Token newToken, PointyType type, List<Node> args)
			: base(NodeType.CONSTRUCTOR_INVOCATION, newToken)
		{
			this.StructType = type;
			this.Args = args.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			for (int i = 0; i < this.Args.Length; ++i)
			{
				this.Args[i] = this.Args[i].ResolveExpression(context);
			}

			if (this.SystemConstructorHint != null)
			{
				switch (this.SystemConstructorHint)
				{
					case "List()":
						return Listify(new SystemMethodInvocation(this.Token, "List.new", this.Args));
					case "List(int)":
						return Listify(new SystemMethodInvocation(this.Token, "List.new_with_capacity", this.Args));
					case "UniString(UniChar)":
						return Listify(new SystemMethodInvocation(this.Token, "UniString.new_from_single_char", this.Args));
					default:
						throw new Exception(); // forgot to add system constructor to this switch.
				}
			}
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			List<PointyType> argTypes = new List<PointyType>();
			for (int i = 0; i < this.Args.Length; ++i)
			{
				this.Args[i].ResolveType(context);
				argTypes.Add(this.Args[i].ReturnType);
			}

			string sysMethodLookup = this.StructType.Name + "(" + string.Join("+", argTypes.Select<PointyType, string>(t => t.ToQuickString())) + ")";
			switch (sysMethodLookup)
			{
				case "List()":
				case "List(int)":
				case "UniString(UniChar)":
					this.SystemConstructorHint = sysMethodLookup;
					this.ReturnType = this.StructType;
					return;
				default:
					switch (this.StructType.Name)
					{
						case "List":
						case "UniString":
						case "UniChar":
							throw new ParserException(this.Token, this.StructType.Name + " does not have a constructor matching those args.");
						default:
							break;
					}
					break;
			}

			StructDefinition sd = context.GetStruct(this.StructType.Name);
			if (sd == null)
			{
				throw new ParserException(this.Token, "Unknown type.");
			}

			foreach (ConstructorDefinition cd in sd.Constructors)
			{
				if (cd.ArgTypes.Length == argTypes.Count)
				{
					bool match = true;
					for (int i = 0; i < cd.ArgTypes.Length; ++i)
					{
						if (!cd.ArgTypes.Equals(argTypes[i]))
						{
							match = false;
							break;
						}
					}

					if (match)
					{
						this.ConstructorDefinition = cd;
						break;
					}
				}
			}

			if (this.ConstructorDefinition == null)
			{
				throw new ParserException(this.Token, "Could not find constructor for '" + this.StructType.Name + "' that matched those arg types.");
			}

			this.ReturnType = this.StructType;
		}
	}
}
