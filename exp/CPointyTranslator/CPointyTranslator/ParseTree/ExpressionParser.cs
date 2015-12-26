using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public static class ExpressionParser
	{
		public static Node Parse(TokenStream tokens)
		{
			return ParseTernary(tokens);
		}

		private static Node ParseTernary(TokenStream tokens)
		{
			Node expr = ParseBooleanCombination(tokens);
			if (tokens.PopIfPresent("?"))
			{
				Node trueCode = ParseTernary(tokens);
				tokens.PopExpected(":");
				Node falseCode = ParseTernary(tokens);
				return new TernaryNode(expr, trueCode, falseCode);
			}

			return expr;
		}

		private static Node ParseBooleanCombination(TokenStream tokens)
		{
			Node expr = ParseBitwiseOp(tokens);
			if (tokens.IsNext("&&") || tokens.IsNext("||"))
			{
				List<Node> expressions = new List<Node>() { expr };
				List<Token> ops = new List<Token>();
				while (tokens.IsNext("&&") || tokens.IsNext("||"))
				{
					Token opToken = tokens.Pop();
					ops.Add(opToken);
					expressions.Add(ParseBitwiseOp(tokens));
				}
				return new BinaryOpChain(expressions, ops);
			}
			return expr;
		}

		private static Node ParseBitwiseOp(TokenStream tokens)
		{
			Node expr = ParseEquality(tokens);
			if (tokens.IsNext("&") || tokens.IsNext("|") || tokens.IsNext("^"))
			{
				List<Node> expressions = new List<Node>() { expr };
				List<Token> ops = new List<Token>();
				while (tokens.IsNext("&") || tokens.IsNext("|") || tokens.IsNext("^"))
				{
					ops.Add(tokens.Pop());
					expressions.Add(ParseEquality(tokens));
				}
				return new BinaryOpChain(expressions, ops);
			}
			return expr;
		}

		private static Node ParseEquality(TokenStream tokens)
		{
			Node expr = ParseInequality(tokens);
			if (tokens.IsNext("==") || tokens.IsNext("!="))
			{
				List<Node> expressions = new List<Node>() { expr };
				List<Token> ops = new List<Token>();
				while (tokens.IsNext("==") || tokens.IsNext("!="))
				{
					ops.Add(tokens.Pop());
					expressions.Add(ParseInequality(tokens));
				}
				return new BinaryOpChain(expressions, ops);
			}
			return expr;
		}

		private static Node ParseInequality(TokenStream tokens)
		{
			Node expr = ParseBitShift(tokens);
			if (tokens.IsNext("<") || tokens.IsNext(">") || tokens.IsNext("<=") || tokens.IsNext(">="))
			{
				List<Node> expressions = new List<Node>() { expr };
				List<Token> ops = new List<Token>();
				while (tokens.IsNext("<") || tokens.IsNext(">") || tokens.IsNext("<=") || tokens.IsNext(">="))
				{
					ops.Add(tokens.Pop());
					expressions.Add(ParseBitShift(tokens));
				}
				return new BinaryOpChain(expressions, ops);
			}
			return expr;
		}

		private static Node ParseBitShift(TokenStream tokens)
		{
			Node expr = ParseAddition(tokens);
			if (tokens.IsNext("<<") || tokens.IsNext(">>"))
			{
				List<Node> expressions = new List<Node>() { expr };
				List<Token> ops = new List<Token>();
				while (tokens.IsNext("<<") || tokens.IsNext(">>"))
				{
					ops.Add(tokens.Pop());
					expressions.Add(ParseAddition(tokens));
				}
				return new BinaryOpChain(expressions, ops);
			}
			return expr;
		}

		private static Node ParseAddition(TokenStream tokens)
		{
			Node expr = ParseMultiplication(tokens);
			if (tokens.IsNext("+") || tokens.IsNext("-"))
			{
				List<Node> expressions = new List<Node>() { expr };
				List<Token> ops = new List<Token>();
				while (tokens.IsNext("+") || tokens.IsNext("-"))
				{
					ops.Add(tokens.Pop());
					expressions.Add(ParseMultiplication(tokens));
				}
				return new BinaryOpChain(expressions, ops);
			}
			return expr;
		}

		private static Node ParseMultiplication(TokenStream tokens)
		{
			Node expr = ParseNegate(tokens);
			if (tokens.IsNext("*") || tokens.IsNext("/") || tokens.IsNext("%"))
			{
				List<Node> expressions = new List<Node>() { expr };
				List<Token> ops = new List<Token>();
				while (tokens.IsNext("*") || tokens.IsNext("/") || tokens.IsNext("%"))
				{
					ops.Add(tokens.Pop());
					expressions.Add(ParseNegate(tokens));
				}
				return new BinaryOpChain(expressions, ops);
			}
			return expr;
		}

		private static Node ParseNegate(TokenStream tokens)
		{
			if (tokens.IsNext("-") || tokens.IsNext("!"))
			{
				Token opToken = tokens.Pop();
				Node root = ParseNegate(tokens);
				return new NegateNode(opToken, root);
			}

			return ParseIncrement(tokens);
		}

		private static Node ParseIncrement(TokenStream tokens)
		{
			Token prefixToken = null;
			Token suffixToken = null;
			if (tokens.IsNext("++") || tokens.IsNext("--"))
			{
				prefixToken = tokens.Pop();
			}

			Node expression = ParseEntity(tokens);

			if (prefixToken == null)
			{
				if (tokens.IsNext("++") || tokens.IsNext("--"))
				{
					suffixToken = tokens.Pop();
				}
			}

			if (prefixToken != null)
			{
				return new IncrementNode(prefixToken, expression, prefixToken, true);
			}

			if (suffixToken != null)
			{
				return new IncrementNode(expression.Token, expression, suffixToken, false);
			}

			return expression;
		}

		private static Node ParseEntity(TokenStream tokens)
		{
			Node expression = null;

			if (tokens.PopIfPresent("("))
			{
				expression = Parse(tokens);
				tokens.PopExpected(")");
			}
			else
			{
				Token token = tokens.Pop();
				switch (token.Value)
				{
					case "true":
					case "false":
						expression = new BooleanConstant(token, token.Value == "true");
						break;
					case "null":
						expression = new NullConstant(token);
						break;
					case "new":
						PointyType type = PointyType.Parse(tokens);
						if (tokens.IsNext("["))
						{
							Token openBracket = tokens.Pop();
							Node sizeAllocation = ExpressionParser.Parse(tokens);
							tokens.PopExpected("]");
							expression = new ArrayAllocation(token, openBracket, type, sizeAllocation);
						}
						else
						{
							tokens.PopExpected("(");
							List<Node> args = new List<Node>();
							while (!tokens.PopIfPresent(")"))
							{
								if (args.Count > 0)
								{
									tokens.PopExpected(",");
								}
								args.Add(Parse(tokens));
							}
							expression = new ConstructorInvocation(token, type, args);
						}
						break;
					default:
						break;
				}

				if (expression == null)
				{
					char c = token.Value[0];
					if (c == '"' || c == '\'')
					{
						expression = new StringConstant(token, c == '\'');
					}
					else if (c >= '0' && c <= '9')
					{
						if (token.Value.Contains('.'))
						{
							double floatValue;
							if (double.TryParse(token.Value, out floatValue))
							{
								expression = new FloatConstant(token, floatValue);
							}
							else
							{
								throw new ParserException(token, "Invalid float constant.");
							}
						}
						else
						{
							int intValue;
							if (int.TryParse(token.Value, out intValue))
							{
								expression = new IntegerConstant(token, intValue);
							}
							else
							{
								throw new ParserException(token, "Invalid integer constant.");
							}
						}
					}
					else if (
						(c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						c == '_' || 
						c == '$')
					{
						expression = new Variable(token, token.Value);
					}
				}

				if (expression == null)
				{
					throw new ParserException(token, "Unexpected token: '" + token.Value + "'");
				}
			}

			return ParseEntitySuffix(tokens, expression);
		}

		public static Node ParseEntitySuffix(TokenStream tokens, Node expression)
		{
			while (true)
			{
				switch (tokens.PeekValue())
				{
					case "(":
						Token parenToken = tokens.Pop();
						List<Node> args = new List<Node>();
						while (!tokens.PopIfPresent(")"))
						{
							if (args.Count > 0)
							{
								tokens.PopExpected(",");
							}
							args.Add(Parse(tokens));
						}
						expression = new FunctionInvocation(expression, parenToken, args);
						break;

					case "[":
						Token bracketToken = tokens.Pop();
						Node index = Parse(tokens);
						tokens.PopExpected("]");
						expression = new BracketIndex(expression, bracketToken, index);
						break;

					case ".":
						Token dotToken = tokens.Pop();
						Token fieldToken = tokens.Pop();
						Util.VerifyIdentifier(fieldToken);
						expression = new DotField(expression, dotToken, fieldToken);
						break;

					default:
						return expression;
				}
			}
		}
	}
}
