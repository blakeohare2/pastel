using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CPointyTranslator.ParseTree
{
	public class PointyType
	{
		public string Name { get; set; }
		public Token NameToken { get; set; }
		public PointyType[] Generics { get; set; }
		public StructDefinition StructHint { get; set; }
		public string[] DotRefHint { get; set; }

		public static readonly PointyType BOOLEAN = new PointyType() { Name = "bool" };
		public static readonly PointyType FLOAT = new PointyType() { Name = "float" };
		public static readonly PointyType INTEGER = new PointyType() { Name = "int" };
		public static readonly PointyType NULL_POINTER = new PointyType() { Name = "NULL" };
		public static readonly PointyType UNI_STRING = new PointyType() { Name = "UniString" };
		public static readonly PointyType UNI_CHAR = new PointyType() { Name = "UniChar" };
		public static readonly PointyType UNKNOWN_FUNCTION_POINTER = new PointyType() { Name = "FP?" };
		public static readonly PointyType VOID = new PointyType() { Name = "void" };

		public static PointyType NewArrayType(PointyType type)
		{
			return new PointyType() { Name = "Array", Generics = new PointyType[] { type } };
		}

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
			return "Type: " + this.ToQuickString();
		}

		public string ToQuickString()
		{
			string output = this.Name;
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

		public override bool Equals(object obj)
		{
			if (obj is PointyType)
			{
				PointyType other = (PointyType)obj;
				if (other.Name != this.Name) return false;
				if (other.Generics == null && this.Generics == null) return true;
				if (other.Generics == null || this.Generics == null) return false;

				if (other.Name == this.Name && this.Generics.Length == other.Generics.Length)
				{
					for (int i = 0; i < this.Generics.Length; ++i)
					{
						if (!this.Generics[i].Equals(other.Generics[i]))
						{
							return false;
						}
					}
					return true;
				}
			}
			return false;
		}

		public override int GetHashCode()
		{
			int hash = this.Name.GetHashCode();
			if (this.Generics != null)
			{
				foreach (PointyType g in this.Generics)
				{
					hash = hash * 31 + g.GetHashCode();
				}
			}
			return hash;
		}

		public bool IsArray
		{
			get { return this.Name == "Array"; }
		}

		public bool IsIntLike
		{
			get { return this.Name == "int" || this.Name == "UniChar"; }
		}
	}
}