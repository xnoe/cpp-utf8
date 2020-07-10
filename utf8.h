// license: cc0

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <stdint.h>

class char32 {
	uint32_t fetch32(char** cstr) {
		uint32_t r(0);
		int i(1);
		unsigned char compare = (unsigned char)**cstr;
		if (compare >> 3 == 0b11110) i = 4;
		if (compare >> 4 == 0b1110) i = 3;
		if (compare >> 5 == 0b110) i = 2;
		for (;i>0;i--) {
			r <<= 8;
			r += (unsigned char)**cstr;
			(*cstr)++;
		}
		return r;
	}
public:
	uint32_t c;
	char32(uint32_t i) {c = i;}
	char32(const char* s) {c = fetch32((char**)&s);}
	char32(char** s) {c = fetch32(s);}
	bool operator==(char* cs) {return c==fetch32(&cs);}
	bool operator==(char32 cs) {return c==cs.c;}
	uint32_t operator>>(int a) const {return c>>a;}
	char* toChar() const {
		return new char[4]{(char)(c>>24), (char)(c>>16), (char)(c>>8), (char)c};
	}
};

std::ostream& operator<<(std::ostream& stream, const char32& c32) {
	char* asChar = c32.toChar();
	for (int i(0);i<4;i++)
		if (asChar[i])
			stream << asChar[i];
	delete asChar;
	return stream;
}

struct string32 {
	std::vector<char32> cs;
	string32(char* sd) {
		while (sd[0])
			cs.push_back(char32(&sd));
	}
	string32(std::vector<char32> c32s) {
		cs = c32s;
	}
	int size() const {
		return cs.size();
	}
	char32 operator[](int i) const {
		return cs[i];
	}
	string32 replace(string32 find, string32 with) {
		string32 copyOfSelf (cs);
		int havematched(0);
		int findsize = find.cs.size();
		for (int index(0); index < copyOfSelf.cs.size(); index++) {
			if (copyOfSelf.cs.at(index) == find.cs.at(havematched))
				havematched++;
			else
				havematched=0;
			if (havematched == findsize) {
				index -= findsize-1;
				copyOfSelf.cs.erase(copyOfSelf.cs.begin()+index, copyOfSelf.cs.begin()+index+findsize);
				copyOfSelf.cs.insert(copyOfSelf.cs.begin()+index, with.cs.begin(), with.cs.end());
				break;
			}
		}
		return copyOfSelf;
	}
};

std::ostream& operator<<(std::ostream& stream, const string32& s32) {
	for (int i=0;i<s32.size();i++)
		stream << s32[i];
	return stream;
}
