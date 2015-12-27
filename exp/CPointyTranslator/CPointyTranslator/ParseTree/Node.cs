using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public class Node
	{
		public NodeType Type { get; set; }

		public Token Token { get; private set; }
		public Node(NodeType nodeType, Token token)
		{
			this.Token = token;
			this.Type = nodeType;
		}
	}

	public enum NodeType
	{
		ARRAY_ALLOCATION,
		ARRAY_INDEX,
		ASSIGNMENT,
		BINARY_OP_CHAIN,
		BOOLEAN_CONSTANT,
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
		TERNARY,
		VARIABLE,
		VARIABLE_DECLARATION,
		WHILE_LOOP,
	}
}
