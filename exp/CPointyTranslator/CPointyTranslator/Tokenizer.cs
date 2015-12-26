using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator
{
	static class Tokenizer
	{
		private enum Mode
		{
			NONE,
			STRING,
			COMMENT,
			WORD,
		}

		private static readonly HashSet<string> MULTI_CHAR_TOKENS = new HashSet<string>(
			"++ -- && || == != <= >= << >> += -= *= /= %= &= |= ^= <<= >>=".Split(' '));

		public static TokenStream Tokenize(string filename, string code)
		{
			code = code.Replace("\r\n", "\n").Replace('\r', '\n');

			List<Token> tokens = new List<Token>();

			List<int> lines = new List<int>();
			List<int> columns = new List<int>();
			int line = 0;
			int col = 0;
			for (int i = 0; i <= code.Length; ++i)
			{
				lines.Add(line);
				columns.Add(col);

				if (i < code.Length && code[i] == '\n')
				{
					col = 0;
					line++;
				}
				else
				{
					col++;
				}
			}

			List<string> tokenBuilder = new List<string>();
			int tokenStart = -1;
			Mode mode = Mode.NONE;
			char modeType = '\0';

			for (int i = 0; i <= code.Length; ++i)
			{
				char c = i < code.Length ? code[i] : '\0';
				string c2 = i < code.Length - 1 ? ("" + code[i] + code[i + 1]) : null;
				string c3 = i < code.Length - 2 ? ("" + code[i] + code[ i + 1] + code[i + 2]) : null;

				switch (mode)
				{
					case Mode.NONE:
						bool handled = false;
						switch (c)
						{
							case ' ':
							case '\n':
							case '\t':
							case '\0':
								handled = true;
								break;

							case '/':
								if (c2 == "//" || c2 == "/*")
								{
									mode = Mode.COMMENT;
									modeType = c2[1];
									++i;
									handled = true;
								}
								break;

							case '"':
							case '\'':
								mode = Mode.STRING;
								tokenStart = i;
								modeType = c;
								tokenBuilder.Clear();
								tokenBuilder.Add("" + c);
								handled = true;
								break;

							default:
								if ((c >= '0' && c <= '9') ||
									(c >= 'a' && c <= 'z') ||
									(c >= 'A' && c <= 'Z') ||
									c == '_' ||
									c == '$' ||
									(c == '.' && c2 != null && c2[1] >= '0' && c2[1] <= '9'))
								{
									mode = Mode.WORD;
									tokenStart = i;
									tokenBuilder.Clear();
									--i;
									handled = true;
								}
								break;
						}

						if (!handled)
						{
							string value;
							if (MULTI_CHAR_TOKENS.Contains(c2))
							{
								value = c2;
							}
							else if (MULTI_CHAR_TOKENS.Contains(c3))
							{
								value = c3;
							}
							else
							{
								value = "" + c;
							}
							tokens.Add(new Token() { Column = columns[i], Line = lines[i], Value = value, FileName = filename });
							i += value.Length - 1;
						}
						break;


					case Mode.COMMENT:
						if (modeType == '*')
						{
							if (c2 == "*/")
							{
								mode = Mode.NONE;
								++i;
							}
						}
						else if (modeType == '/')
						{
							if (c == '\n') mode = Mode.NONE;
						}
						break;

					case Mode.STRING:
						if (c == '\\')
						{
							tokenBuilder.Add(c2);
							++i;
						}
						else if (c == modeType)
						{
							mode = Mode.NONE;
							tokenBuilder.Add("" + c);
							tokens.Add(new Token() { Column = columns[tokenStart], Line = lines[tokenStart], Value = string.Join("", tokenBuilder), FileName = filename });
							tokenBuilder.Clear();
						}
						else
						{
							tokenBuilder.Add(c + "");
						}
						break;

					case Mode.WORD:
						if ((c == '.' && c2 != null && c2[1] >= '0' && c2[1] <= '9') ||
							(c >= 'a' && c <= 'z') ||
							(c >= 'A' && c <= 'Z') ||
							(c >= '0' && c <= '9') ||
							c == '_' ||
							c == '$')
						{
							tokenBuilder.Add(c + "");
						}
						else
						{
							tokens.Add(new Token() { Column = columns[tokenStart], Line = lines[tokenStart], Value = string.Join("", tokenBuilder), FileName = filename });
							mode = Mode.NONE;
							tokenBuilder.Clear();
							--i;
						}
						break;

					default: throw new InvalidOperationException();
				}
			}

			return new TokenStream(tokens);
		}
	}
}
