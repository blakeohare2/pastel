using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public class FunctionDefinition : Node
	{
		public Token NameToken { get; set; }
		public PointyType FunctionReturnType { get; set; }
		public PointyType[] ArgTypes { get; set; }
		public Token[] ArgNames { get; set; }
		public Node[] Code { get; set; }
		public StructDefinition Parent { get; set; }
		public string ArgTypeFingerprint { get; set; }
		public int UniqueId { get; set; }

		private static int uniqueIdAlloc = 0;

		public FunctionDefinition(Token functionToken, PointyType type, Token nameToken, IList<PointyType> argTypes, IList<Token> argNames, IList<Node> code)
			: base(NodeType.FUNCTION_DECLARATION, functionToken)
		{
			this.UniqueId = ++uniqueIdAlloc;
			this.NameToken = nameToken;
			this.ArgNames = argNames.ToArray();
			this.ArgTypes = argTypes.ToArray();
			this.Code = code.ToArray();
			this.FunctionReturnType = type;
			this.ArgTypeFingerprint = string.Join("+", this.ArgTypes.Select<PointyType, string>(t => t.ToQuickString()));
		}

		public override IList<Node> Resolve(Context context)
		{
			context.ActiveContext = this;
			this.Code = Node.ResolveCodeChunk(context, this.Code);
			context.ActiveContext = null;
			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			context.PushVariableScope();
			int argLength = this.ArgNames.Length;
			for (int i = 0; i < argLength; ++i)
			{
				Token name = this.ArgNames[i];
				PointyType type = this.ArgTypes[i];
				context.DeclareVariableInCurrentScope(name, type);
			}

			foreach (Node node in this.Code)
			{
				node.ResolveType(context);
			}
			context.PopVariableScope();
		}
	}
}
