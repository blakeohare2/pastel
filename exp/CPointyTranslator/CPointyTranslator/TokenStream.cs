using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator
{
	public class TokenStream
	{
		private int currentIndex;
		private int length;
		private Token[] tokens;

		public TokenStream(IList<Token> tokens)
		{
			this.tokens = tokens.ToArray();
			this.length = this.tokens.Length;
			this.currentIndex = 0;
		}

		public bool IsNext(string value)
		{
			return this.PeekValue() == value;
		}

		public bool HasMore
		{
			get
			{
				return this.currentIndex < this.length;
			}
		}

		public string PeekValue()
		{
			Token token = this.Peek();
			if (token != null) return token.Value;
			return null;
		}

		public Token Peek()
		{
			if (this.currentIndex < this.length)
			{
				return this.tokens[this.currentIndex];
			}
			return null;
		}

		public Token Peek2()
		{
			if (this.currentIndex + 1 < this.length)
			{
				return this.tokens[this.currentIndex + 1];
			}
			return null;
		}

		public Token Pop()
		{
			Token token = this.Peek();
			if (token != null)
			{
				this.currentIndex++;
				return token;
			}
			throw new ParserException(this.tokens[this.length - 1], "Unexpected EOF");
		}

		public Token PopExpected(string value)
		{
			Token token = this.Pop();
			if (token != null && token.Value == value)
			{
				return token;
			}
			if (token == null)
			{
				throw new ParserException(this.tokens[this.length - 1], "Unexpected EOF");
			}

			throw new ParserException(token, "Unexpected token. Expected '" + value + "'. Found '" + token.Value + "'");
		}

		public bool PopIfPresent(string value)
		{
			if (value == this.PeekValue())
			{
				this.currentIndex++;
				return true;
			}
			return false;
		}
	}
}
