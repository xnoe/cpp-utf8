// license: cc0

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <stdint.h>

class char32 {
	int size = 1;
	uint32_t fetch32(char** cstr) {
		uint32_t r(0);
		if (!**cstr)
			return r;
		unsigned char compare = (unsigned char)**cstr;
		if (compare >> 3 == 0b11110) size = 4;
		if (compare >> 4 == 0b1110) size = 3;
		if (compare >> 5 == 0b110) size = 2;
		int i = size;
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
		char* toReturn = new char[size];
		int i = size;
		for (;i>=0;i--)
			toReturn[i] = (char)(c>>(8*i));
		return toReturn;
	}
};

std::ostream& operator<<(std::ostream& stream, const char32& c32) {
	char* asChar = c32.toChar();
	stream << asChar;
	delete asChar;
	return stream;
}

struct string32 {
	std::vector<char32> cs;
	string32(char* sd) {
		while (sd[0])
			cs.push_back(char32(&sd));
	}
	string32() {}
	int size() const {
		return cs.size();
	}
	char32 operator[](int i) const {
		return cs[i];
	}
	void operator+=(string32 s) {
		cs.insert(cs.end(), s.cs.begin(), s.cs.end());
	}
	void operator+=(char32 c) {
		cs.push_back(c);
	}
	string32 replace(string32 find, string32 with) {
		string32 copyOfSelf = *this;
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

	std::vector<string32> split(string32 delim) {
		std::vector<string32> toReturn;
		string32 temporary, throwaway;
		int havematched(0);
		int findsize = delim.cs.size();
		for (int index(0); index < cs.size(); index++) {
			if (cs.at(index) == delim.cs.at(havematched)) {
				throwaway += cs[index];
				havematched++;
			}	else {
				temporary += throwaway;
				throwaway = "";
				temporary += cs[index];
				havematched=0;
			}
			if (havematched == findsize) {
				toReturn.push_back(temporary);
				temporary = "";
				throwaway = "";
				havematched = 0;
			}
		}
		if (temporary.size()>0)
			toReturn.push_back(temporary);
		return toReturn;
	}
};

std::ostream& operator<<(std::ostream& stream, const string32& s32) {
	for (int i=0;i<s32.size();i++)
		stream << s32[i];
	return stream;
}
