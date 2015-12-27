using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public abstract class Node
	{
		public NodeType Type { get; set; }

		public PointyType ReturnType { get; set; }

		public Token Token { get; private set; }
		public Node(NodeType nodeType, Token token)
		{
			this.ReturnType = null;
			this.Token = token;
			this.Type = nodeType;
		}

		public abstract IList<Node> Resolve(Context context);

		public Node ResolveExpression(Context context)
		{
			return this.Resolve(context)[0];
		}

		protected IList<Node> Listify(Node node)
		{
			return new List<Node>() { node };
		}

		protected static Node[] ResolveCodeChunk(Context context, Node[] code)
		{
			List<Node> output = new List<Node>();
			foreach (Node line in code)
			{
				output.AddRange(line.Resolve(context));
				if (context.NeedsExceptionCheck)
				{
					context.NeedsExceptionCheck = false;
					// TODO: add exception check
				}
			}
			return output.ToArray();
		}

		public abstract void ResolveType(Context context);
	}

	public enum NodeType
	{
		ARRAY_ALLOCATION,
		ARRAY_INDEX,
		ASSIGNMENT,
		BINARY_OP_CHAIN,
		BOOLEAN_CONSTANT,
		BREAK,
		CONSTRUCTOR_DECLARATION,
		CONSTRUCTOR_INVOCATION,
		DOT_FIELD,
		ENUM_DEFINITION,
		FLOAT_CONSTANT,
		FOR_LOOP,
		FUNCTION_DECLARATION,
		FUNCTION_INVOCATION,
		IF_STATEMENT,
		INCREMENT,
		INTEGER_CONSTANT,
		NEGATE,
		NULL_CONSTANT,
		RETURN,
		STRING_CONSTANT,
		STRUCT_DECLARATION,
		SWITCH_STATEMENT,
		SYSTEM_METHOD_INVOCATION,
		TERNARY,
		THIS,
		VARIABLE,
		VARIABLE_DECLARATION,
		WHILE_LOOP,
	}
}
