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
			}

			structs = NodeSorter.SortByTypeDependency(structs, enums);

			List<Node> all = new List<Node>();
			all.AddRange(structs.Cast<Node>());
			all.AddRange(functions.Cast<Node>());

			List<string> mainFileBuffer = new List<string>();

			mainFileBuffer.Add(Util.LoadTextResource("Templates/headers.c.txt"));
			mainFileBuffer.Add(Util.LoadTextResource("Templates/unistring.c.txt"));

			ExportNodes(context, all, mainFileBuffer);

			mainFileBuffer.Add(Util.LoadFileResource("Templates/main.c.txt"));


			output["main.c"] = string.Join("", mainFileBuffer);

			return output;
		}

		public static void ExportNodes(Context context, List<Node> nodes, List<string> buffer)
		{
			foreach (Node node in nodes)
			{
				ExportNode(context, node, buffer);
			}
		}

		public static void ExportNode(Context context, Node node, List<string> buffer)
		{
			if (node is FunctionDefinition) ExportFunctionDefinition(context, (FunctionDefinition)node, buffer);
			else if (node is StructDefinition) ExportStructDefinition(context, (StructDefinition)node, buffer);
			else throw new ParserException(node.Token, "I don't have an exporter defined for this syntax yet.");
		}

		public static void ExportFunctionDefinition(Context context, FunctionDefinition func, List<string> buffer)
		{
			buffer.Add("\n");
			buffer.Add(context.CifyType(func.ReturnType));
			buffer.Add(" ");
			buffer.Add(func.NameToken.Value);
			buffer.Add("(");
			for (int i = 0; i < func.ArgNames.Length; ++i)
			{
				if (i > 0) buffer.Add(", ");
				buffer.Add(context.CifyType(func.ArgTypes[i]));
				buffer.Add(" ");
				buffer.Add(func.ArgNames[i].Value);
			}
			buffer.Add(")\n{");
			context.Tab = 1;

			ExportExecutables(context, func.Code, buffer);

			buffer.Add("}\n");
			context.Tab = 0;
		}

		public static void ExportStructDefinition(Context context, StructDefinition sd, List<string> buffer)
		{
			buffer.Add("\ntypedef struct ");
			buffer.Add(sd.Name);
			buffer.Add(" {");
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
				case NodeType.ARRAY_INDEX: ExportArrayIndex(context, (BracketIndex)node, buffer); return;
				case NodeType.DOT_FIELD: throw new ParserException(node.Token, "Dot field was not resolved.");
				default: throw new ParserException(node.Token, "I don't know what this is.");
			}
		}

		public static void ExportExecutable(Context context, Node node, List<string> buffer)
		{
			buffer.Add(context.Tabs);
			switch (node.Type)
			{
				// These end in semicolons
				case NodeType.ASSIGNMENT: ExportAssignment(context, (AssignmentStatement)node, buffer); break;

				// These don't.
				case NodeType.IF_STATEMENT: ExportIfStatement(context, (IfStatement)node, buffer); return;

				case NodeType.ARRAY_INDEX:
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

		public static void ExportArrayIndex(Context context, BracketIndex bi, List<string> buffer)
		{
			throw new ParserException(bi.Token, "TODO: this");
		}

		public static void ExportAssignment(Context context, AssignmentStatement assignment, List<string> buffer)
		{
			if (assignment.TypeDeclaration != null)
			{
				buffer.Add(context.CifyType(assignment.TypeDeclaration));
				buffer.Add(" ");
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
			throw new ParserException(ifStatement.Token, "TODO: this");
		}
	}
}
