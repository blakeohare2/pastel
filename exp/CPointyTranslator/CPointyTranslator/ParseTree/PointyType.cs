using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	class PointyType
	{
		public string Name { get; set; }
		public Token NameToken { get; set; }
		public PointyType[] Generics { get; set; }

		public static PointyType Parse(TokenStream tokens)
		{
			Token typeName = tokens.Pop();
			Util.VerifyIdentifier(typeName);

			if (tokens.PopIfPresent("<"))
			{
				List<PointyType> generics = new List<PointyType>();
				while (!tokens.PopIfPresent(">"))
				{
					if (generics.Count > 0)
					{
						tokens.PopExpected(",");
					}

					generics.Add(PointyType.Parse(tokens));
				}

				return new PointyType() { Name = typeName.Value, NameToken = typeName, Generics = generics.ToArray() };
			}
			return new PointyType() { Name = typeName.Value, NameToken = typeName, Generics = null };
		}

		public override string ToString()
		{
			string output = "Type: " + this.Name;
			if (this.Generics != null && this.Generics.Length > 0)
			{
				output += "<";
				for (int i = 0; i < this.Generics.Length; ++i)
				{
					if (i > 0) output += ", ";
					output += this.Generics[i].ToString();
				}
				output += ">";
			}
			return output;
		}
	}
}
