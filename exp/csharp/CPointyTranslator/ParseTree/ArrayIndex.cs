using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ArrayIndex : Node
	{
		public Node Root { get; set; }
		public Token BracketToken { get; set; }
		public Node Index { get; set; }

		public ArrayIndex(Node root, Token bracketToken, Node index)
			: base(NodeType.ARRAY_INDEX, root.Token)
		{
			this.Root = root;
			this.BracketToken = bracketToken;
			this.Index = index;
		}

		public override IList<Node> Resolve(Context context)
		{
			this.Root = this.Root.ResolveExpression(context);
			this.Index = this.Index.ResolveExpression(context);
			if (this.Root.ReturnType.Name == "List")
			{
				return Listify(new SystemMethodInvocation(this.Token, "List.get", new Node[] { this.Root, this.Index }) { ReturnType = this.ReturnType });
			}
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Root.ResolveType(context);
			this.Index.ResolveType(context);
			if (this.Index.ReturnType.Name != "int") throw new ParserException(this.Index.Token, "Array index must be integer. Found " + this.Index.ReturnType.ToQuickString());
			if (this.Root.ReturnType.IsArray)
			{
				this.ReturnType = this.Root.ReturnType.Generics[0];
			}
			else if (this.Root.ReturnType.Name == "UniString")
			{
				this.ReturnType = PointyType.UNI_CHAR;
			}
			else if (this.Root.ReturnType.Name == "List")
			{
				this.ReturnType = this.Root.ReturnType.Generics[0];
			}
			else
			{
				throw new ParserException(this.BracketToken, "Attempted to index into a non-array type.");
			}
		}
	}
}
