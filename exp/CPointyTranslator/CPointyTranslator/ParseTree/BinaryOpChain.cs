using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class BinaryOpChain : Node
	{
		public Token[] OpTokens { get; set; }
		public Node[] Expressions { get; set; }

		public BinaryOpChain(List<Node> expressions, List<Token> ops)
			: base(NodeType.BINARY_OP_CHAIN, expressions[0].Token)
		{
			this.Expressions = expressions.ToArray();
			this.OpTokens = ops.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			// TODO: resolve constant expressions
			for (int i = 0; i < this.Expressions.Length; ++i)
			{
				this.Expressions[i] = this.Expressions[i].ResolveExpression(context);
			}

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			for (int i = 0; i < this.Expressions.Length; ++i)
			{
				this.Expressions[i].ResolveType(context);
			}

			PointyType t = this.Expressions[0].ReturnType;
			for (int i = 1; i < this.Expressions.Length; ++i)
			{
				t = this.EnsureCorrectType(t, this.OpTokens[i - 1], this.Expressions[i].ReturnType);
			}

			this.ReturnType = t;
		}

		private PointyType EnsureCorrectType(PointyType leftType, Token opToken, PointyType rightType)
		{
			switch (opToken.Value)
			{
				case "+":
				case "-":
				case "*":
				case "/":
				case "%":
				case "<":
				case ">":
				case "<=":
				case ">=":
					bool isCompare = opToken.Value.Contains("<") || opToken.Value.Contains(">");
					if (leftType.Name == "int" && rightType.Name == "int") return isCompare ? PointyType.BOOLEAN : PointyType.INTEGER;
					if (leftType.Name == "float" && rightType.Name == "float") return isCompare ? PointyType.BOOLEAN : PointyType.FLOAT;
					if (leftType.Name == "float" && rightType.Name == "int") return isCompare ? PointyType.BOOLEAN : PointyType.FLOAT;
					if (leftType.Name == "int" && rightType.Name == "float") return isCompare ? PointyType.BOOLEAN : PointyType.FLOAT;
					if (leftType.Name == "UniChar" && rightType.Name == "UniChar") return isCompare ? PointyType.BOOLEAN : PointyType.UNI_CHAR;
					break;
				case "|":
				case "&":
				case "^":
				case "<<":
				case ">>":
					if (leftType.Name == "int" && rightType.Name == "int") return PointyType.INTEGER;
					break;
				case "&&":
				case "||":
					if (leftType.Name == "bool" && rightType.Name == "bool") return PointyType.BOOLEAN;
					break;
				case "==":
				case "!=":
					if (leftType.Name == "UniChar" && rightType.Name == "UniChar")
					{
						return PointyType.BOOLEAN;
					}

					if ((leftType.Name == "int" || leftType.Name == "double" || leftType.Name == "bool") &&
						(rightType.Name == "int" || rightType.Name == "double" || rightType.Name == "bool"))
						return PointyType.BOOLEAN;

					break;
			}
			throw new ParserException(opToken, "Invalid values for op: '" + opToken.Value + "'");
		}
	}
}
