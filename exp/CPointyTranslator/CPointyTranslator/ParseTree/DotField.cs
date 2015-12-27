using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public enum ResolutionHintType
	{
		NONE,
		ENUM_ITEM,
		STRUCT_FIELD,
		STRUCT_METHOD,
		PRIMITIVE_METHOD,
		PRIMITIVE_FIELD,
	}
	
	public class DotField : Node
	{
		public Node Root { get; set; }
		public Token DotToken { get; set; }
		public Token FieldToken { get; set; }

		public ResolutionHintType ResolutionHint = ResolutionHintType.NONE;
		public object ResolutionHintArg = null;
		public StructDefinition ResolutionHintStruct = null;

		public DotField(Node root, Token dotToken, Token fieldToken)
			: base(NodeType.DOT_FIELD, root.Token)
		{
			this.Root = root;
			this.DotToken = dotToken;
			this.FieldToken = fieldToken;
		}

		public override IList<Node> Resolve(Context context)
		{
			switch (this.ResolutionHint)
			{
				case ResolutionHintType.ENUM_ITEM:
					IntegerConstant ic = new IntegerConstant(this.Token, (int)this.ResolutionHintArg);
					return Listify(ic);

				case ResolutionHintType.PRIMITIVE_FIELD:
					switch (this.ResolutionHintArg.ToString())
					{
						case "UniString.length":
							return Listify(new SystemMethodInvocation(this.Token, "UniString.length", new Node[] { this.Root }) { ReturnType = PointyType.INTEGER });
					}
					break;

				case ResolutionHintType.PRIMITIVE_METHOD:
					switch (this.ResolutionHintArg.ToString())
					{
						case "List.add":
							throw new ParserException(this.Token, "Method pointers cannot be passed.");
					}
					break;

				case ResolutionHintType.STRUCT_METHOD:
					// This is resolved away in FunctionInvocation instead of getting resolved. If it's still here, then it wasn't invoked.
					throw new ParserException(this.Token, "Struct method pointers cannot be passed.");

				case ResolutionHintType.STRUCT_FIELD:
					this.Root = this.Root.ResolveExpression(context);
					return Listify(this);

				default:
					throw new Exception(); // Resolution hint not implemented or set.
			}
			throw new Exception();
		}

		public override void ResolveType(Context context)
		{
			if (this.Root is Variable)
			{
				string name = this.Root.Token.Value;
				// is it actually an enum?
				if (context.IsEnumName(name))
				{
					if (context.IsEnumMember(name, this.FieldToken.Value))
					{
						this.ReturnType = PointyType.INTEGER;
						this.ResolutionHint = ResolutionHintType.ENUM_ITEM;
						this.ResolutionHintArg = context.GetEnumValue(name, this.FieldToken.Value);
						return;
					}
					throw new ParserException(this.DotToken, "The enum '" + name + "' does not have a value called '" + this.FieldToken.Value);
				}

				this.Root.ResolveType(context);
			}

			this.Root.ResolveType(context);
			PointyType rootType = this.Root.ReturnType;
			if (rootType.Name == "UniString")
			{
				switch (this.FieldToken.Value)
				{
					case "length":
						this.ReturnType = PointyType.INTEGER;
						this.ResolutionHint = ResolutionHintType.PRIMITIVE_FIELD;
						this.ResolutionHintArg = "UniString.length";
						return;
				}
			}

			if (rootType.Name == "List")
			{
				switch (this.FieldToken.Value)
				{
					case "length":
						this.ReturnType = PointyType.INTEGER;
						this.ResolutionHint = ResolutionHintType.PRIMITIVE_FIELD;
						this.ResolutionHintArg = "List.length";
						return;
					case "add":
						this.ReturnType = null;
						this.ResolutionHint = ResolutionHintType.PRIMITIVE_METHOD;
						this.ResolutionHintArg = "List.add";
						return;
					default: break;
				}
			}

			if (context.IsStructName(rootType.Name))
			{
				StructDefinition sd = context.GetStruct(rootType.Name);
				this.ResolutionHintStruct = sd;
				PointyType fieldType;
				if (sd.TypesByName.TryGetValue(this.FieldToken.Value, out fieldType))
				{
					this.ReturnType = fieldType;
					this.ResolutionHint = ResolutionHintType.STRUCT_FIELD;
					this.ResolutionHintArg = this.FieldToken.Value;
				}
				else
				{
					this.ReturnType = new PointyType() { Name = "STRUCT-FP", StructHint = sd };
					this.ResolutionHint = ResolutionHintType.STRUCT_METHOD;
					this.ResolutionHintArg = this.FieldToken.Value;
				}
				return;
			}

			throw new ParserException(this.DotToken, "Unable to resolve this field.");
		}
	}
}
