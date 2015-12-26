using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class FunctionDefinition : Node
	{
		public Token NameToken { get; set; }
		public PointyType ReturnType { get; set; }
		public PointyType[] ArgTypes { get; set; }
		public Token[] ArgNames { get; set; }
		public Node[] Code { get; set; }

		public FunctionDefinition(Token functionToken, PointyType type, Token nameToken, IList<PointyType> argTypes, IList<Token> argNames, IList<Node> code)
			: base(functionToken)
		{
			this.NameToken = nameToken;
			this.ArgNames = argNames.ToArray();
			this.ArgTypes = argTypes.ToArray();
			this.Code = code.ToArray();
		}
	}
}
