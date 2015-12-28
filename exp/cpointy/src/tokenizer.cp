enum TokenizerMode {
	NONE,
	WORD,
	STRING,
	COMMENT,
}

struct Token {
	field UniString value;
	field int line;
	field int col;
	field UniString filename;
}

struct TokenStream {
	field int current;
	field int length;
	field Array<Token> tokens;
}

function TokenStream tokenize(UniString filename, UniString code) {
	int length = code.length;
	Array<int> lines = new int[length + 1];
	Array<int> columns = new int[length + 1];
	List<Token> tokens = new List<Token>();
	int line = 0;
	int col = 0;
	int i;
	UniChar c;
	UniChar c2;
	UniChar c3;
	for (i = 0; i <= length; ++i) {
		lines[i] = line;
		columns[i] = col;
		if (i < length && code[i] == '\n') {
			line++;
			col = 0;
		} else {
			col++;
		}
	}
	
	StringBuilder token_builder = new StringBuilder();
	int token_start = 0;
	UniChar modeType;
	int mode = TokenizerMode.NONE;
	bool handled;
	for (i = 0; i <= length; ++i) {
		c = i < length ? code[i] : '\0';
		switch (mode) {
			case TokenizerMode.NONE:
				handled = false;
				switch (c) {
					case ' ':
					case '\n':
					case '\t':
					case '\r':
						handled = true;
						break;
						
					case '/':
						if (i < length - 1) {
							c2 = code[i + 1];
							if (c2 == '*' || c2 == '/') {
								mode = TokenizerMode.COMMENT;
								modeType = c2;
								handled = true;
								i++;
							}
						}
						break;
						
					case '"':
					case '\'':
						token_start = i;
						token_builder.append_char(c);
						mode = TokenizerMode.STRING;
						modeType = c;
						break;
						
					default:
						if ((c >= 'a' && c <= 'z') ||
							(c >= 'A' && c <= 'Z') ||
							(c >= '0' && c <= '9') ||
							c == '_' ||
							c == '$') {
							
							token_start = i;
							token_builder.clear();
							mode = TokenizerMode.WORD;
							--i;
							handled = true;
						}
						break;
				}
				if (!handled) {
					Token token = new Token();
					token.line = lines[i];
					token.col = columns[i];
					token.value = new UniString(c);
					token.filename = filename;
					tokens.add(token);
				}
				break;
				
			case TokenizerMode.COMMENT:
				
			case TokenizerMode.STRING:
				
			case TokenizerMode.WORD:
				
			default:
				break;
		
		}
	}
}