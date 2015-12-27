using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class VariableDeclaration : Node
	{
		public PointyType VariableType { get; set; }
		public Token NameToken { get; set; }

		public VariableDeclaration(Token firstToken, PointyType type, Token name)
			: base(NodeType.VARIABLE_DECLARATION, firstToken)
		{
			this.VariableType = type;
			this.NameToken = name;
		}

		public override IList<Node> Resolve(Context context)
		{
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			context.DeclareVariableInCurrentScope(this.NameToken, this.VariableType);
		}
	}
}
