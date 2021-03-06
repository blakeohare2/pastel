﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class FunctionInvocation : Node
	{
		public Node Root { get; set; }
		public Token ParenToken { get; set; }
		public Node[] Args { get; set; }
		public FunctionDefinition StructMethodResolutionHint { get; set; }
		public string SystemMethodHint { get; set; }
		public string PrimitiveMethodResolutionHint { get; set; }
		public Node ContextArg { get; set; } // target of method invocation

		public FunctionInvocation(Node root, Token parenToken, IList<Node> args)
			: base(NodeType.FUNCTION_INVOCATION, root.Token)
		{
			this.Root = root;
			this.ParenToken = parenToken;
			this.Args = args.ToArray();
		}

		public override IList<Node> Resolve(Context context)
		{
			// TODO: check for throws declaration of invokee and then mark context as needing exception check.
			for (int i = 0; i < this.Args.Length; ++i)
			{
				this.Args[i] = this.Args[i].ResolveExpression(context);
			}

			DotField df = this.Root as DotField;

			if (this.StructMethodResolutionHint != null)
			{
				this.ContextArg = df.Root.ResolveExpression(context);
				this.Root = null;
			}
			else if (this.PrimitiveMethodResolutionHint != null)
			{
				df.Root = df.Root.ResolveExpression(context);
				Node target = df.Root;
				switch (this.PrimitiveMethodResolutionHint)
				{
					case "List.add(object)":
						return Listify(new SystemMethodInvocation(
							this.Token,
							"List.add",
							new Node[] { 
								target, 
								new Cast(this.Args[0].Token, PointyType.INTEGER, this.Args[0])
							}) { ReturnType = PointyType.VOID });
				}
				throw new ParserException(this.Token, "Unable to resolve primitive method."); // I don't think this should happen, otherwise the primitive method hint shouldn't have been set.
			}
			else if (this.SystemMethodHint != null)
			{
				string realName;
				switch (this.SystemMethodHint)
				{
					case "io_is_dir(UniString)": realName = "IO.is_directory"; break;
					case "io_list_dir(UniString)": realName = "IO.list_dir"; break;
					case "io_read_all_text(UniString)": realName = "IO.read_all_text"; break;
					case "print(UniString)": realName = "System.print"; break;
					case "println(UniString)": realName = "System.println"; break;
					default: throw new Exception(); // forgot to add system method here.
				}
				return Listify(new SystemMethodInvocation(this.Token, realName, this.Args) { ReturnType = this.ReturnType });
			}
			else
			{
				this.Root = this.Root.ResolveExpression(context);
			}

			return Listify(this);
		}

		public override void ResolveType(Context context)
		{
			this.Root.ResolveType(context);
			List<string> fingerprintBuilder = new List<string>();
			foreach (Node arg in this.Args)
			{
				arg.ResolveType(context);
				fingerprintBuilder.Add(arg.ReturnType.ToQuickString());
			}
			PointyType type = this.Root.ReturnType;

			DotField df = this.Root as DotField;
			if (df != null && df.ResolutionHint != ResolutionHintType.NONE)
			{
				switch (df.ResolutionHint)
				{
					case ResolutionHintType.STRUCT_METHOD:
						StructDefinition sd = df.ResolutionHintStruct;
						string fingerprint = string.Join("+", fingerprintBuilder);
						string methodName = df.FieldToken.Value;
						FunctionDefinition fd = null;
						foreach (FunctionDefinition md in sd.Methods)
						{
							if (md.NameToken.Value == methodName && fingerprint == md.ArgTypeFingerprint)
							{
								fd = md;
							}
						}
						if (fd == null)
						{
							throw new ParserException(this.ParenToken, sd.Name + " does not have a method called " + methodName + " with those args.");
						}
						this.ReturnType = fd.FunctionReturnType;
						this.StructMethodResolutionHint = fd;
						return;

					case ResolutionHintType.PRIMITIVE_METHOD:
						switch (df.ResolutionHintArg.ToString())
						{
							case "List.add":
								if (fingerprintBuilder.Count == 1)
								{
									this.PrimitiveMethodResolutionHint = "List.add(object)";
									this.ReturnType = PointyType.VOID;
									return;
								}
								break;
						}
						throw new ParserException(this.Token, "Method not defined.");
				}
			}

			if (this.Root is Variable && ((Variable) this.Root).SystemMethodPointerHint != null)
			{
				string sysMethodName = ((Variable)this.Root).SystemMethodPointerHint;
				string fullSignature = sysMethodName + "(" + string.Join(",", fingerprintBuilder) + ")";
				switch (fullSignature)
				{
					case "io_is_dir(UniString)":
					case "io_list_dir(UniString)":
					case "io_read_all_text(UniString)":
					case "print(UniString)":
					case "println(UniString)":
						this.ReturnType = this.Root.ReturnType;
						this.SystemMethodHint = fullSignature;
						return;
						
					default:
						throw new ParserException(this.Token, "$" + sysMethodName + " does not have any signature that matched these args.");
				}
			}

			throw new NotImplementedException(); // TODO: regular functions
		}
	}
}
