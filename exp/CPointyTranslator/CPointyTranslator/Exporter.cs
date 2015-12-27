using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CPointyTranslator.ParseTree;

namespace CPointyTranslator
{
	static class Exporter
	{
		public static Dictionary<string, string> Export(List<Node> code)
		{
			Context context = new Context();

			Dictionary<string, string> output = new Dictionary<string, string>();

			List<EnumDeclaration> enums = new List<EnumDeclaration>();
			List<FunctionDefinition> functions = new List<FunctionDefinition>();
			List<StructDefinition> structs = new List<StructDefinition>();
			List<ConstructorDefinition> constructors = new List<ConstructorDefinition>();
			List<FunctionDefinition> methods = new List<FunctionDefinition>();

			foreach (Node node in code)
			{
				if (node is FunctionDefinition) functions.Add((FunctionDefinition)node);
				else if (node is StructDefinition) structs.Add((StructDefinition)node);
				else if (node is EnumDeclaration) enums.Add((EnumDeclaration)node);
				else throw new ParserException(node.Token, "This expression must appear within a function.");
			}

			foreach (EnumDeclaration ed in enums)
			{
				context.RegsterEnum(ed);
			}

			foreach (StructDefinition sd in structs)
			{
				context.RegisterStruct(sd);
				foreach (ConstructorDefinition cd in sd.Constructors)
				{
					context.RegisterConstructor(cd);
					constructors.Add(cd);
				}

				foreach (FunctionDefinition fd in sd.Methods)
				{
					context.RegisterMethod(fd);
					methods.Add(fd);
				}
			}

			foreach (FunctionDefinition fd in functions)
			{
				context.RegisterFunction(fd);
			}

			structs = NodeSorter.SortByTypeDependency(structs, enums);

			List<Node> all = new List<Node>();
			all.AddRange(functions.Cast<Node>());
			all.AddRange(constructors.Cast<Node>());
			all.AddRange(methods.Cast<Node>());

			foreach (Node node in all)
			{
				node.ResolveType(context);
			}

			List<Node> resolvedNodes = new List<Node>();
			foreach (Node node in all)
			{
				resolvedNodes.AddRange(node.Resolve(context));
			}
			all = resolvedNodes;

			List<string> mainFileBuffer = new List<string>();

			mainFileBuffer.Add(Util.LoadTextResource("Templates/headers.c.txt"));
			mainFileBuffer.Add(Util.LoadTextResource("Templates/mem.c.txt"));
			mainFileBuffer.Add(Util.LoadTextResource("Templates/list.c.txt"));
			mainFileBuffer.Add(Util.LoadTextResource("Templates/unistring.c.txt"));

			ExportNodes(context, structs.Cast<Node>(), mainFileBuffer);

			ExportPrototypes(context, resolvedNodes, mainFileBuffer);

			ExportNodes(context, resolvedNodes, mainFileBuffer);

			mainFileBuffer.Add(Util.LoadTextResource("Templates/main.c.txt"));


			output["main.c"] = string.Join("", mainFileBuffer);

			return output;
		}

		public static void ExportPrototypes(Context context, List<Node> nodes, List<string> buffer)
		{
			StructDefinition sd;
			foreach (Node node in nodes)
			{
				switch (node.Type)
				{
					case NodeType.CONSTRUCTOR_DECLARATION:
						ConstructorDefinition cd = (ConstructorDefinition)node;
						sd = cd.Parent;
						buffer.Add(sd.Name);
						buffer.Add("* CONS_OUTER_");
						buffer.Add(cd.UniqueId + "");
						buffer.Add("_");
						buffer.Add(sd.Name);
						buffer.Add("(");
						for (int i = 0; i < cd.ArgNames.Length; ++i)
						{
							if (i > 0) buffer.Add(", ");
							buffer.Add(context.CifyType(cd.ArgTypes[i]));
							buffer.Add(" v_");
							buffer.Add(cd.ArgNames[i].Value);
						}
						buffer.Add(");\n");

						buffer.Add("void CONS_INNER_");
						buffer.Add(cd.UniqueId + "");
						buffer.Add("_");
						buffer.Add(sd.Name);
						buffer.Add("(");
						buffer.Add(sd.Name);
						buffer.Add("* _this");
						for (int i = 0; i < cd.ArgNames.Length; ++i)
						{
							buffer.Add(", ");
							buffer.Add(context.CifyType(cd.ArgTypes[i]));
							buffer.Add(" v_");
							buffer.Add(cd.ArgNames[i].Value);
						}
						buffer.Add(");\n");
						break;

					case NodeType.FUNCTION_DECLARATION:
						FunctionDefinition fd = (FunctionDefinition)node;
						sd = fd.Parent;
						buffer.Add(context.CifyType(fd.FunctionReturnType));
						buffer.Add(" ");
						if (fd.Parent != null)
						{
							buffer.Add("METHOD_");
						}
						else
						{
							buffer.Add("FUN_");
						}
						buffer.Add(fd.UniqueId + "");
						buffer.Add("_");
						buffer.Add(fd.NameToken.Value);
						buffer.Add("(");
						if (fd.Parent != null)
						{
							buffer.Add(sd.Name);
							buffer.Add("* _this");
							if (fd.ArgNames.Length > 0) buffer.Add(", ");
						}

						for (int i = 0; i < fd.ArgNames.Length; ++i)
						{
							if (i > 0) buffer.Add(", ");
							buffer.Add(context.CifyType(fd.ArgTypes[i]));
							buffer.Add(" v_");
							buffer.Add(fd.ArgNames[i].Value);
						}
						buffer.Add(");\n");

						break;

					case NodeType.STRUCT_DECLARATION:
						break;

					default:
						throw new Exception(); // WAT?
				}
			}
		}

		public static void ExportNodes(Context context, IEnumerable<Node> nodes, List<string> buffer)
		{
			buffer.Add("\n");
			foreach (Node node in nodes)
			{
				ExportNode(context, node, buffer);
			}
			buffer.Add("\n");
		}

		public static void ExportNode(Context context, Node node, List<string> buffer)
		{
			if (node is FunctionDefinition) ExportFunctionDefinition(context, (FunctionDefinition)node, buffer);
			else if (node is StructDefinition) ExportStructDefinition(context, (StructDefinition)node, buffer);
			else if (node is ConstructorDefinition) ExportConstructorDefinition(context, (ConstructorDefinition)node, buffer);
			else throw new ParserException(node.Token, "I don't have an exporter defined for this syntax yet.");
		}

		public static void ExportConstructorDefinition(Context context, ConstructorDefinition cd, List<string> buffer)
		{
			StructDefinition sd = cd.Parent;
			buffer.Add(sd.Name);
			buffer.Add("* CONS_OUTER_");
			buffer.Add(cd.UniqueId + "");
			buffer.Add("_");
			buffer.Add(sd.Name);
			buffer.Add("(");
			for (int i = 0; i < cd.ArgNames.Length; ++i)
			{
				if (i > 0) buffer.Add(", ");
				buffer.Add(context.CifyType(cd.ArgTypes[i]));
				buffer.Add(" ");
				buffer.Add(cd.ArgNames[i].Value);
			}
			buffer.Add(")\n{\t");
			buffer.Add(sd.Name);
			buffer.Add("* _this = (");
			buffer.Add(sd.Name);
			buffer.Add("*) malloc(sizeof(");
			buffer.Add(sd.Name);
			buffer.Add("));\n\tCONS_INNER_");
			buffer.Add(sd.Name);
			buffer.Add("(_this");
			for (int i = 0; i < cd.ArgNames.Length; ++i)
			{
				buffer.Add(", ");
				buffer.Add(cd.ArgNames[i].Value);
			}
			buffer.Add(");\n\treturn _this;\n}\n\n");

			buffer.Add("void CONS_INNER_");
			buffer.Add(sd.Name);
			buffer.Add("(");
			buffer.Add(sd.Name);
			buffer.Add("* _this");
			for (int i = 0; i < cd.ArgNames.Length; ++i)
			{
				buffer.Add(", ");
				buffer.Add(context.CifyType(cd.ArgTypes[i]));
				buffer.Add(" ");
				buffer.Add(cd.ArgNames[i].Value);
			}
			buffer.Add(")\n");
			context.Tab++;
			ExportExecutables(context, cd.Code, buffer);
			context.Tab--;
			buffer.Add("}\n\n");
		}

		public static void ExportFunctionDefinition(Context context, FunctionDefinition func, List<string> buffer)
		{
			StructDefinition methodParent = func.Parent;
			bool isMethod = methodParent != null;
			buffer.Add("\n");
			buffer.Add(context.CifyType(func.FunctionReturnType));
			buffer.Add(" ");
			if (isMethod)
			{
				buffer.Add("METHOD_" + func.UniqueId + "_");
			}
			else
			{
				buffer.Add("FUN_");
			}
			buffer.Add(func.NameToken.Value);
			buffer.Add("(");
			if (isMethod)
			{
				buffer.Add(methodParent.Name);
				buffer.Add("* _this, ");
			}

			for (int i = 0; i < func.ArgNames.Length; ++i)
			{
				if (i > 0) buffer.Add(", ");
				buffer.Add(context.CifyType(func.ArgTypes[i]));
				buffer.Add(" ");
				buffer.Add(func.ArgNames[i].Value);
			}
			buffer.Add(")\n{\n");
			context.Tab = 1;

			ExportExecutables(context, func.Code, buffer);

			buffer.Add("}\n");
			context.Tab = 0;
		}

		public static void ExportStructDefinition(Context context, StructDefinition sd, List<string> buffer)
		{
			buffer.Add("\ntypedef struct ");
			buffer.Add(sd.Name);
			buffer.Add(" {\n");
			foreach (string field in sd.FieldOrder)
			{
				buffer.Add("\t");
				buffer.Add(context.CifyType(sd.TypesByName[field]));
				buffer.Add(" ");
				buffer.Add(field);
				buffer.Add(";\n");
			}
			buffer.Add("} ");
			buffer.Add(sd.Name);
			buffer.Add(";\n");
		}

		public static void ExportExecutables(Context context, IList<Node> nodes, List<string> buffer)
		{
			foreach (Node node in nodes)
			{
				ExportExecutable(context, node, buffer);
			}
		}

		public static void ExportExpression(Context context, Node node, List<string> buffer)
		{
			switch (node.Type)
			{
				case NodeType.ARRAY_ALLOCATION: ExportArrayAllocation(context, (ArrayAllocation)node, buffer); return;
				case NodeType.ARRAY_INDEX: ExportArrayIndex(context, (ArrayIndex)node, buffer); return;
				case NodeType.BOOLEAN_CONSTANT: ExportBooleanConstant(context, (BooleanConstant)node, buffer); return;
				case NodeType.BINARY_OP_CHAIN: ExportBinaryOpChain(context, (BinaryOpChain)node, buffer); return;
				case NodeType.CONSTRUCTOR_INVOCATION: ExportConstructorInvocation(context, (ConstructorInvocation)node, buffer); return;
				case NodeType.DOT_FIELD: ExportDotField(context, (DotField)node, buffer); return;
				case NodeType.INCREMENT: ExportIncrement(context, (IncrementNode)node, buffer); return;
				case NodeType.INTEGER_CONSTANT: ExportIntegerConstant(context, (IntegerConstant)node, buffer); return;
				case NodeType.NEGATE: ExportNegate(context, (NegateNode)node, buffer); return;
				case NodeType.STRING_CONSTANT: ExportStringConstant(context, (StringConstant)node, buffer); return;
				case NodeType.SYSTEM_METHOD_INVOCATION: ExportSystemMethodInvocation(context, (SystemMethodInvocation)node, buffer); return;
				case NodeType.TERNARY: ExportTernary(context, (TernaryNode)node, buffer); break;
				case NodeType.THIS: ExportThis(context, (ThisConstant)node, buffer); break;
				case NodeType.VARIABLE: ExportVariable(context, (Variable)node, buffer); return;
				default: throw new ParserException(node.Token, "I don't know what this is.");
			}
		}

		public static void ExportThis(Context context, ThisConstant tc, List<string> buffer)
		{
			buffer.Add("_this");
		}

		public static void ExportDotField(Context context, DotField df, List<string> buffer)
		{
			if (df.ResolutionHint == ResolutionHintType.STRUCT_FIELD)
			{
				ExportExpression(context, df.Root, buffer);
				buffer.Add("->");
				buffer.Add(df.FieldToken.Value);
			}
			else
			{
				throw new Exception(); // Everything ought to have been resolved into something else by now.
			}
		}

		public static void ExportNegate(Context context, NegateNode ng, List<string> buffer)
		{
			buffer.Add(ng.Token.Value);
			buffer.Add("(");
			ExportExpression(context, ng.Expression, buffer);
			buffer.Add(")");
		}

		public static void ExportBooleanConstant(Context context, BooleanConstant bc, List<string> buffer)
		{
			buffer.Add(bc.Value ? "1" : "0");
		}

		public static void ExportTernary(Context context, TernaryNode tn, List<string> buffer)
		{
			ExportExpression(context, tn.Condition, buffer);
			buffer.Add(" ? ");
			ExportExpression(context, tn.TrueCode, buffer);
			buffer.Add(" : ");
			ExportExpression(context, tn.FalseCode, buffer);
		}

		public static void ExportStringConstant(Context context, StringConstant sc, List<string> buffer)
		{
			if (sc.IsChar)
			{
				buffer.Add("'");
				switch (sc.Value[0])
				{
					case '\n': buffer.Add("\\n"); break;
					case '\r': buffer.Add("\\r"); break;
					case '\t': buffer.Add("\\t"); break;
					case '\0': buffer.Add("\\0"); break;
					case '\'': buffer.Add("\\'"); break;
					case '\\': buffer.Add("\\\\"); break;
					default: buffer.Add(sc.Value); break;
				}
				buffer.Add("'");
			}
			else
			{
				throw new Exception();
			}
		}

		public static void ExportConstructorInvocation(Context context, ConstructorInvocation ci, List<string> buffer)
		{
			buffer.Add("CONS_OUTER_");
			buffer.Add(ci.ConstructorDefinition.UniqueId + "");
			buffer.Add("_");
			buffer.Add(ci.StructType.Name);
			buffer.Add("(");
			for (int i = 0; i < ci.Args.Length; ++i)
			{
				if (i > 0) buffer.Add(", ");
				ExportExpression(context, ci.Args[i], buffer);
			}
			buffer.Add(")");
		}

		public static void ExportIntegerConstant(Context context, IntegerConstant ic, List<string> buffer)
		{
			buffer.Add(ic.Value + "");
		}

		public static void ExportBinaryOpChain(Context context, BinaryOpChain binaryOpChain, List<string> buffer)
		{
			buffer.Add("(");
			ExportExpression(context, binaryOpChain.Expressions[0], buffer);
			for (int i = 0; i < binaryOpChain.OpTokens.Length; ++i)
			{
				buffer.Add(" ");
				buffer.Add(binaryOpChain.OpTokens[i].Value);
				buffer.Add(" ");
				ExportExpression(context, binaryOpChain.Expressions[i + 1], buffer);
			}
			buffer.Add(")");
		}

		public static void ExportArrayAllocation(Context context, ArrayAllocation arrayAlloc, List<string> buffer)
		{
			string ptr = context.CifyType(arrayAlloc.ItemType);
			buffer.Add("((");
			buffer.Add(ptr);
			buffer.Add("*) allocate_array(");
			buffer.Add("sizeof(");
			buffer.Add(ptr);
			buffer.Add("), ");
			ExportExpression(context, arrayAlloc.Size, buffer);
			buffer.Add("))");
		}

		public static void ExportVariable(Context context, Variable variable, List<string> buffer)
		{
			buffer.Add("v_");
			buffer.Add(variable.Name);
		}

		public static void ExportSystemMethodInvocation(Context context, SystemMethodInvocation sysMethod, List<string> buffer)
		{
			buffer.Add("SM_");
			buffer.Add(sysMethod.MethodId.Replace('.', '_'));
			buffer.Add("(");
			for (int i = 0; i < sysMethod.Args.Length; ++i)
			{
				if (i > 0) buffer.Add(", ");
				ExportExpression(context, sysMethod.Args[i], buffer);
			}
			buffer.Add(")");
		}

		public static void ExportExecutable(Context context, Node node, List<string> buffer)
		{
			if (!context.InForLoop) buffer.Add(context.Tabs);
			switch (node.Type)
			{
				// These end in semicolons
				case NodeType.ASSIGNMENT: ExportAssignment(context, (AssignmentStatement)node, buffer); break;
				case NodeType.BREAK: ExportBreakStatement(context, (BreakStatement)node, buffer); break;
				case NodeType.FUNCTION_INVOCATION: ExportFunctionInvocation(context, (FunctionInvocation)node, buffer); break;
				case NodeType.VARIABLE_DECLARATION: ExportVariableDeclaration(context, (VariableDeclaration)node, buffer); break;
				case NodeType.INCREMENT: ExportIncrement(context, (IncrementNode)node, buffer); break;

				// These don't.
				case NodeType.FOR_LOOP: ExportForLoop(context, (ForLoop)node, buffer); return;
				case NodeType.IF_STATEMENT: ExportIfStatement(context, (IfStatement)node, buffer); return;
				case NodeType.SWITCH_STATEMENT: ExportSwitchStatement(context, (SwitchStatement)node, buffer); return;
				case NodeType.WHILE_LOOP: ExportWhileLoop(context, (WhileLoop)node, buffer); return;

				// These are acceptable as standalone expressions and end in semicolons
				case NodeType.SYSTEM_METHOD_INVOCATION:
					ExportExpression(context, node, buffer);
					break;

				case NodeType.ARRAY_INDEX:
				case NodeType.VARIABLE:
					//ExportExpression(context, node, buffer);
					throw new ParserException(node.Token, "This expression does nothing.");
				
				default:
					throw new ParserException(node.Token, "I don't know what this is.");
			}
			if (!context.InForLoop)
			{
				buffer.Add(";\n");
			}
		}

		public static void ExportWhileLoop(Context context, WhileLoop wl, List<string> buffer)
		{
			buffer.Add("while (");
			ExportExpression(context, wl.Condition, buffer);
			buffer.Add(")\n");
			buffer.Add(context.Tabs);
			buffer.Add("{\n");
			context.Tab++;
			ExportExecutables(context, wl.Code, buffer);
			context.Tab--;
			buffer.Add(context.Tabs);
			buffer.Add("}\n");
		}

		public static void ExportFunctionInvocation(Context context, FunctionInvocation fi, List<string> buffer)
		{
			if (fi.ContextArg != null)
			{
				buffer.Add("method_");
				buffer.Add(fi.StructMethodResolutionHint.Parent.Name);
				buffer.Add("_");
				buffer.Add(fi.StructMethodResolutionHint.NameToken.Value);
				buffer.Add("_");
				buffer.Add(fi.StructMethodResolutionHint.UniqueId.ToString());
				buffer.Add("(");
				ExportExpression(context, fi.ContextArg, buffer);
				for (int i = 0; i < fi.Args.Length; ++i)
				{
					buffer.Add(", ");
					ExportExpression(context, fi.Args[i], buffer);
				}
				buffer.Add(")");
			}
			else
			{
				throw new Exception(); // TODO
			}
		}

		public static void ExportBreakStatement(Context context, BreakStatement bs, List<string> buffer)
		{
			buffer.Add("break");
		}

		public static void ExportSwitchStatement(Context context, SwitchStatement ss, List<string> buffer)
		{
			buffer.Add("switch (");
			ExportExpression(context, ss.Expression, buffer);
			buffer.Add(")\n");
			buffer.Add(context.Tabs);
			buffer.Add("{\n");
			context.Tab++;

			int length = ss.Cases.Length;
			for (int i = 0; i < length; ++i)
			{
				Node[] caseClusters = ss.Cases[i];
				foreach (Node cases in caseClusters)
				{
					buffer.Add(context.Tabs);
					if (cases == null)
					{
						buffer.Add("default:\n");
					}
					else
					{
						buffer.Add("case ");
						ExportExpression(context, cases, buffer);
						buffer.Add(":\n");
					}
				}

				context.Tab++;
				ExportExecutables(context, ss.Code[i], buffer);
				if (i < length - 1) buffer.Add("\n");
				context.Tab--;
			}

			context.Tab--;
			buffer.Add(context.Tabs);
			buffer.Add("}\n");
		}

		public static void ExportIncrement(Context context, IncrementNode inc, List<string> buffer)
		{
			if (inc.IsPrefix)
			{
				buffer.Add(inc.IncrementToken.Value);
				ExportExpression(context, inc.Expression, buffer);
			}
			else
			{
				ExportExpression(context, inc.Expression, buffer);
				buffer.Add(inc.IncrementToken.Value);
			}
		}

		public static void ExportForLoop(Context context, ForLoop forLoop, List<string> buffer)
		{
			buffer.Add("for (");
			context.InForLoop = true;
			for (int i = 0; i < forLoop.Init.Length; ++i)
			{
				if (i > 0) buffer.Add(", ");
				ExportExecutable(context, forLoop.Init[i], buffer);
			}
			buffer.Add("; ");
			if (forLoop.Condition != null)
			{
				ExportExpression(context, forLoop.Condition, buffer);
			}
			buffer.Add("; ");
			for (int i = 0; i < forLoop.Step.Length; ++i)
			{
				if (i > 0) buffer.Add(", ");
				ExportExecutable(context, forLoop.Step[i], buffer);
			}
			buffer.Add(")\n");
			context.InForLoop = false;

			buffer.Add(context.Tabs);
			buffer.Add("{\n");
			context.Tab++;
			ExportExecutables(context, forLoop.Code, buffer);
			context.Tab--;
			buffer.Add(context.Tabs);
			buffer.Add("}\n");
		}

		public static void ExportVariableDeclaration(Context context, VariableDeclaration vd, List<string> buffer)
		{
			string type = context.CifyType(vd.VariableType);
			buffer.Add(type);
			buffer.Add(" v_");
			buffer.Add(vd.NameToken.Value);
			buffer.Add(" = ");
			switch (type)
			{
				case "int": buffer.Add("0"); break;
				case "double": buffer.Add("0.0"); break;
				default: buffer.Add("NULL"); break;
			}
		}

		public static void ExportArrayIndex(Context context, ArrayIndex bi, List<string> buffer)
		{
			ExportExpression(context, bi.Root, buffer);
			buffer.Add("[");
			ExportExpression(context, bi.Index, buffer);
			buffer.Add("]");
		}

		public static void ExportAssignment(Context context, AssignmentStatement assignment, List<string> buffer)
		{
			if (assignment.TypeDeclaration != null)
			{
				buffer.Add(context.CifyType(assignment.TypeDeclaration));
				buffer.Add(" v_");
				buffer.Add(((Variable)assignment.TargetExpression).Name);
				buffer.Add(" = ");
				ExportExpression(context, assignment.ValueExpression, buffer);
			}
			else
			{
				ExportExpression(context, assignment.TargetExpression, buffer);
				buffer.Add(" ");
				buffer.Add(assignment.OpToken.Value);
				buffer.Add(" ");
				ExportExpression(context, assignment.ValueExpression, buffer);
			}
		}

		public static void ExportIfStatement(Context context, IfStatement ifStatement, List<string> buffer)
		{
			buffer.Add("if (");
			ExportExpression(context, ifStatement.Condition, buffer);
			buffer.Add(")\n");
			buffer.Add(context.Tabs);
			buffer.Add("{\n");
			context.Tab++;
			ExportExecutables(context, ifStatement.TrueCode, buffer);
			context.Tab--;
			buffer.Add(context.Tabs);
			buffer.Add("}\n");
			if (ifStatement.FalseCode.Length > 0)
			{
				buffer.Add(context.Tabs);
				buffer.Add("else");
				if (ifStatement.FalseCode.Length == 1 && ifStatement.FalseCode[0] is IfStatement)
				{
					// it's an else if
					buffer.Add(" ");
					ExportIfStatement(context, (IfStatement) ifStatement.FalseCode[0], buffer);
				}
				else
				{
					buffer.Add("\n");
					buffer.Add(context.Tabs);
					buffer.Add("{\n");
					context.Tab++;
					ExportExecutables(context, ifStatement.FalseCode, buffer);
					context.Tab--;
					buffer.Add(context.Tabs);
					buffer.Add("}\n");
				}
			}
		}
	}
}
