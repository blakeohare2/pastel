struct StringBuilder {
	field int capacity;
	field int size;
	field Array<UniChar> characters;
	
	constructor(int capacity) {
		this.capacity = capacity;
		this.size = 0;
		this.characters = new UniChar[this.capacity];
	}
	
	constructor() {
		this.capacity = 10;
		this.size = 0;
		this.characters = new UniChar[this.capacity];
	}
	
	function void ensureCapacity(int newLength) {
		int newCapacity = this.capacity;
		while (newCapacity < newLength) {
			newCapacity = newCapacity * 2 + 1;
		}
		Array<UniChar> newCharacters = new UniChar[newCapacity];
		int i;
		for (i = 0; i < this.size; ++i) {
			newCharacters[i] = this.characters[i];
		}
		// TODO: free this.characters
		this.characters = newCharacters;
		this.capacity = newCapacity;
	}
	
	function void append_char(UniChar c) {
		if (this.size == this.capacity) {
			this.ensureCapacity(this.capacity + 1);
		}
		this.characters[this.size++] = c;
	}
	
	function void append_string(UniString str) {
		int strLength = str.length;
		int newLength = this.size + strLength;
		if (newLength > this.capacity) {
			this.ensureCapacity(newLength);
		}
		// TODO: implicitly add the type declaration variable for for loops before the loop itself, and allow the code to have the type declaration inline in the setup, like Java/C#.
		int i;
		for (i = 0; i < strLength; ++i) {
			this.characters[this.size + i] = str[i];
		}
		this.size = newLength;
	}
	
	function void clear() {
		this.size = 0;
	}
}

