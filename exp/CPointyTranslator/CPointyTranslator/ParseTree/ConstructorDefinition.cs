using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public class ConstructorDefinition : Node
	{
		public PointyType[] ArgTypes { get; set; }
		public Token[] ArgNames { get; set; }
		public Node[] Code { get; set; }
		public StructDefinition Parent { get; set; }
		public string ArgFingerprint { get; set; }

		public int UniqueId { get; set; }

		private static int uniqueIdAlloc = 0;

		public ConstructorDefinition(Token constructorToken, IList<PointyType> argTypes, IList<Token> argNames, IList<Node> code)
			: base(NodeType.CONSTRUCTOR_DECLARATION, constructorToken)
		{
			this.UniqueId = ++uniqueIdAlloc;
			this.ArgTypes = argTypes.ToArray();
			this.ArgNames = argNames.ToArray();
			this.Code = code.ToArray();
			this.ArgFingerprint = string.Join("+", this.ArgTypes.Select<PointyType, string>(t => t.ToQuickString()));
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
			context.ActiveContext = this;
			context.PushVariableScope();

			for (int i = 0; i < this.ArgNames.Length; ++i)
			{
				context.DeclareVariableInCurrentScope(this.ArgNames[i], this.ArgTypes[i]);
			}

			foreach (Node node in this.Code)
			{
				node.ResolveType(context);
			}

			context.PopVariableScope();
			context.ActiveContext = null;
		}
	}
}
