using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator
{
	public static class Util
	{
		public static void VerifyIdentifier(Token token)
		{
			if (!IsIdentifier(token))
			{
				throw new ParserException(token, "Expected identifier.");
			}
		}

		public static bool IsIdentifier(Token token)
		{
			if (token == null || token.Value.Length == 0) throw new ParserException(token, "Null token");
			string value = token.Value;
			if (value[0] >= '0' && value[0] <= '9') return false;
			for (int i = 0; i < value.Length; ++i)
			{
				char c = value[i];
				if ((c >= '0' && c <= '9') ||
					(c >= 'a' && c <= 'z') ||
					(c >= 'A' && c <= 'Z') ||
					c == '_' ||
					c == '$')
				{
					// This is fine
				}
				else
				{
					return false;
				}
			}
			return true;
		}

		public static string GetStringValueFromStringToken(Token token)
		{
			List<string> value = new List<string>();
			string tokenValue = token.Value;
			for (int i = 1; i < token.Value.Length; ++i)
			{
				char c = token.Value[i];
				if (c == '\\')
				{
					switch (token.Value[i + 1])
					{
						case 'n': value.Add("\n"); break;
						case 'r': value.Add("\r"); break;
						case 't': value.Add("\t"); break;
						case '0': value.Add("\0"); break;
						case '"': value.Add("\""); break;
						case '\'': value.Add("'"); break;
						case '\\': value.Add("\\"); break;
						default: throw new ParserException(token, "Invalid string escape sequence");
					}
				}
				else
				{
					value.Add("" + c);
				}
			}

			return string.Join("", value);
		}

		public static string LoadFileResource(string path)
		{
			string value = System.IO.File.ReadAllText(path);
			return value;
		}

		public static string LoadTextResource(string path)
		{
			string[] resources = typeof(Util).Assembly.GetManifestResourceNames();

			return ":)";
		}
	}
}
