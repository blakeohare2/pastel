using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class ExecutableParser
	{
		private static readonly HashSet<string> ASSIGNMENT_OPS = new HashSet<string>("= += -= *= /= %= ^= &= |=".Split(' '));

		public static Node ParseExecutable(TokenStream tokens, bool simpleOnly, bool semicolonRequired, bool isRoot)
		{
			if (!tokens.HasMore) return null;

			string next = tokens.PeekValue();
			if (isRoot)
			{
				switch (next)
				{
					case "struct": return ParseStruct(tokens);
					case "function": return ParseFunction(tokens);
					case "enum": return ParseEnum(tokens);
					default: break;
				}
			}
			else
			{
				switch (next)
				{
					case "for": return ParseFor(tokens);
					case "while": return ParseWhile(tokens);
					case "if": return ParseIf(tokens);
					case "return": return ParseReturn(tokens);
					case "switch": return ParseSwitch(tokens);
					default: break;
				}
			}

			if (!isRoot)
			{
				Token token1 = tokens.Peek();
				Token token2 = tokens.Peek2();
				Node expression = null;

				if (!simpleOnly && Util.IsIdentifier(token1) && token2 != null && (token2.Value == "<" || Util.IsIdentifier(token2)))
				{
					PointyType assignmentType = PointyType.Parse(tokens);
					Token targetToken = tokens.Pop();
					Util.VerifyIdentifier(targetToken);
					if (tokens.IsNext("="))
					{
						Token equalsToken = tokens.PopExpected("=");
						Node value = ExpressionParser.Parse(tokens);
						expression = new AssignmentStatement(token1, new Variable(targetToken, targetToken.Value), value, equalsToken, assignmentType);
					}
					else
					{
						expression = new VariableDeclaration(token1, assignmentType, targetToken);
					}
				}
				else
				{
					expression = ExpressionParser.Parse(tokens);

					string assignmentOp = tokens.PeekValue();
					if (ASSIGNMENT_OPS.Contains(assignmentOp))
					{
						Token assignmentToken = tokens.Pop();
						Node expressionValue = ExpressionParser.Parse(tokens);
						expression = new AssignmentStatement(token1, expression, expressionValue, assignmentToken, null);
					}
				}

				if (semicolonRequired)
				{
					tokens.PopExpected(";");
				}

				return expression;
			}

			throw new ParserException(tokens.Peek(), "Unexpected thing.");
		}

		public static SwitchStatement ParseSwitch(TokenStream tokens)
		{
			Token switchToken = tokens.PopExpected("switch");
			tokens.PopExpected("(");
			Node expression = ExpressionParser.Parse(tokens);
			tokens.PopExpected(")");
			tokens.PopExpected("{");

			List<Node> codeAndCases = new List<Node>();
			List<int> type = new List<int>(); // 0 - executable, 1 - case, 2 - default (value will be null)
			
			while (!tokens.PopIfPresent("}"))
			{
				if (tokens.PopIfPresent("case"))
				{
					codeAndCases.Add(ExpressionParser.Parse(tokens));
					tokens.PopExpected(":");
					type.Add(1);
				}
				else if (tokens.PopIfPresent("default"))
				{
					codeAndCases.Add(null);
					tokens.PopExpected(":");
					type.Add(2);
				}
				else
				{
					codeAndCases.Add(ExecutableParser.ParseExecutable(tokens, false, true, false));
					type.Add(0);
				}
			}

			// do an initial pass to make sure switch statement is well-formed.
			bool defaultFound = false;
			bool caseFound = false;
			for (int i = 0; i < codeAndCases.Count; ++i)
			{
				switch (type[i])
				{
					case 0:
						if (!caseFound && !defaultFound)
						{
							throw new ParserException(codeAndCases[0].Token, "Expected case or default");
						}
						break;
					case 1:
						if (defaultFound)
						{
							throw new ParserException(codeAndCases[i].Token, "Cannot have a case after a default.");
						}
						caseFound = true;
						break;
					default:
						if (!caseFound)
						{
							throw new ParserException(codeAndCases[i].Token, "Cannot have default without cases.");
						}
						defaultFound = true;
						break;
				}
			}

			List<Node[]> cases = new List<Node[]>(); // null will represent default
			List<Node[]> codePerCase = new List<Node[]>();

			List<Node> currentCode = new List<Node>();
			List<Node> currentCases = new List<Node>();

			for (int i = 0; i < codeAndCases.Count; ++i)
			{
				switch (type[i])
				{
					case 0:
						if (currentCases.Count > 0)
						{
							cases.Add(currentCases.ToArray());
							currentCases.Clear();
						}
						currentCode.Add(codeAndCases[i]);
						break;
					case 1:
					default:
						if (currentCode.Count > 0)
						{
							codePerCase.Add(currentCode.ToArray());
							currentCode.Clear();
						}
						currentCases.Add(codeAndCases[i]);
						break;
				}
			}

			if (currentCases.Count != 0)
			{
				throw new ParserException(switchToken, "Switch statement has empty final case.");
			}

			codePerCase.Add(currentCode.ToArray());

			return new SwitchStatement(switchToken, expression, cases, codePerCase);
		}

		public static ReturnStatement ParseReturn(TokenStream tokens)
		{
			Token returnToken = tokens.PopExpected("return");
			Node expression = null;
			if (!tokens.PopIfPresent(";"))
			{
				expression = ExpressionParser.Parse(tokens);
				tokens.PopExpected(";");
			}
			return new ReturnStatement(returnToken, expression);
		}

		public static WhileLoop ParseWhile(TokenStream tokens)
		{
			Token whileToken = tokens.PopExpected("while");
			tokens.PopExpected("(");
			Node condition = ExpressionParser.Parse(tokens);
			tokens.PopExpected(")");
			List<Node> code = ParseBlock(tokens, false);
			return new WhileLoop(whileToken, condition, code);
		}

		public static ForLoop ParseFor(TokenStream tokens)
		{
			Token forToken = tokens.PopExpected("for");
			tokens.PopExpected("(");
			List<Node> init = new List<Node>();
			Node condition = null;
			List<Node> step = new List<Node>();
			while (!tokens.PopIfPresent(";"))
			{
				if (init.Count > 0)
				{
					tokens.PopExpected(",");
				}
				init.Add(ExecutableParser.ParseExecutable(tokens, true, false, false));
			}
			if (!tokens.IsNext(";"))
			{
				condition = ExpressionParser.Parse(tokens);
			}
			tokens.PopExpected(";");
			while (!tokens.PopIfPresent(")"))
			{
				if (step.Count > 0)
				{
					tokens.PopExpected(",");
				}
				step.Add(ExecutableParser.ParseExecutable(tokens, true, false, false));
			}

			List<Node> code = ParseBlock(tokens, false);

			return new ForLoop(forToken, init, condition, step, code);
		}

		public static IfStatement ParseIf(TokenStream tokens)
		{
			Token ifToken = tokens.PopExpected("if");
			tokens.PopExpected("(");
			Node condition = ExpressionParser.Parse(tokens);
			tokens.PopExpected(")");
			List<Node> trueCode = ParseBlock(tokens, false);
			List<Node> falseCode;
			if (tokens.PopIfPresent("else"))
			{
				falseCode = ParseBlock(tokens, false);
			}
			else
			{
				falseCode = new List<Node>();
			}

			return new IfStatement(ifToken, condition, trueCode, falseCode);
		}

		public static EnumDeclaration ParseEnum(TokenStream tokens)
		{
			Token enumToken = tokens.PopExpected("enum");
			Token nameToken = tokens.Pop();
			Util.VerifyIdentifier(nameToken);
			tokens.PopExpected("{");
			List<Token> itemTokens = new List<Token>();
			List<Node> valueExpressions = new List<Node>();
			bool commaMissing = false;
			while (!tokens.PopIfPresent("}"))
			{
				if (commaMissing) tokens.PopExpected(","); // throws error

				Token itemToken = tokens.Pop();
				itemTokens.Add(itemToken);
				Util.VerifyIdentifier(itemToken);
				if (tokens.PopIfPresent("="))
				{
					valueExpressions.Add(ExpressionParser.Parse(tokens));
				}
				else
				{
					valueExpressions.Add(null);
				}
				commaMissing = !tokens.PopIfPresent(",");
			}

			return new EnumDeclaration(enumToken, nameToken, itemTokens, valueExpressions);
		}

		public static FunctionDefinition ParseFunction(TokenStream tokens)
		{
			Token functionToken = tokens.PopExpected("function");
			PointyType type = PointyType.Parse(tokens);
			Token nameToken = tokens.Pop();
			Util.VerifyIdentifier(nameToken);
			tokens.PopExpected("(");
			List<PointyType> argTypes = new List<PointyType>();
			List<Token> argNames = new List<Token>();
			while (!tokens.PopIfPresent(")"))
			{
				if (argTypes.Count > 0)
				{
					tokens.PopExpected(",");
				}

				argTypes.Add(PointyType.Parse(tokens));
				Token argName = tokens.Pop();
				Util.VerifyIdentifier(argName);
				argNames.Add(argName);
			}

			List<Node> code = ExecutableParser.ParseBlock(tokens, true);

			return new FunctionDefinition(functionToken, type, nameToken, argTypes, argNames, code);
		}

		public static List<Node> ParseBlock(TokenStream tokens, bool bracketsExpected)
		{
			bool bracketsPresent = bracketsExpected || tokens.PeekValue() == "{";

			List<Node> output = new List<Node>();
			if (bracketsPresent)
			{
				tokens.PopExpected("{");
				while (!tokens.PopIfPresent("}"))
				{
					output.Add(ExecutableParser.ParseExecutable(tokens, false, true, false));
				}
			}
			else
			{
				if (!tokens.PopIfPresent(";"))
				{
					output.Add(ExecutableParser.ParseExecutable(tokens, false, true, false));
				}
			}

			return output;
		}

		public static StructDefinition ParseStruct(TokenStream tokens)
		{
			Token structToken = tokens.PopExpected("struct");
			Token nameToken = tokens.Pop();
			Util.VerifyIdentifier(nameToken);
			tokens.PopExpected("{");
			List<Token> fieldNameTokens = new List<Token>();
			List<PointyType> types = new List<PointyType>();

			while (!tokens.PopIfPresent("}"))
			{
				tokens.PopExpected("field");
				types.Add(PointyType.Parse(tokens));
				Token fieldToken = tokens.Pop();
				Util.VerifyIdentifier(fieldToken);
				fieldNameTokens.Add(fieldToken);
				tokens.PopExpected(";");
			}

			return new StructDefinition(structToken, nameToken.Value, fieldNameTokens, types);
		}
	}
}
