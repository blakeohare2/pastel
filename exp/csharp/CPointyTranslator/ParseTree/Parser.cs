using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class Parser
	{
		public static List<Node> Parse(IList<TokenStream> tokenStreams)
		{
			List<Node> output = new List<Node>();

			foreach (TokenStream tokens in tokenStreams)
			{
				while (tokens.HasMore)
				{
					Node node = ExecutableParser.ParseExecutable(tokens, false, true, true);
					output.Add(node);
				}
			}

			return output;
		}
	}
}
