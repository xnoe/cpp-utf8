// license: cc0

#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <vector>
#include <stdint.h>

class char32 {
	uint32_t fetch32(char** cstr) {
		uint32_t r(0);
		if (!**cstr) return r;
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
	int size = 1;
	uint32_t c;
	char32(const char* s) {c = fetch32((char**)&s);}
	char32(char** s) {c = fetch32(s);}
	char32(uint32_t s) {c = s;}
	bool operator==(char* cs) {return c==fetch32(&cs);}
	bool operator==(char32 cs) {return c==cs.c;}
	bool operator!=(char32 cs) {return !(*this==cs);}
	uint32_t operator>>(int a) const {return c>>a;}

	char* toChar() const {
		char* toReturn = (char*)calloc(size, 1);
		for (int i=0;i<size;i++)
			toReturn[i] = (char)(c>>(8*(size-i-1)));
		return toReturn;
	}

	uint32_t asUTF32() {
		uint32_t toReturn = 0;
		switch (size) {
			case (4):
				toReturn += (c>>24) & 0b00000111;
				toReturn <<= 6;
				toReturn += (c>>16) & 0b00111111;
				toReturn <<= 6;
				toReturn += (c>>8) & 0b00111111;
				toReturn <<= 6;
				toReturn += (c) & 0b00111111;
				break;
			case (3):
				toReturn += (c>>16) & 0b00001111;
				toReturn <<= 6;
				toReturn += (c>>8) & 0b00111111;
				toReturn <<= 6;
				toReturn += (c) & 0b00111111;
				break;
			case (2):
				toReturn += (c>>8) & 0b00011111;
				toReturn <<= 6;
				toReturn += (c) & 0b00111111;
				break;
			case (1):
				toReturn = c;
		}
		return toReturn;
	}
};

char32 fromUTF32 (uint32_t utf32) {
	char32 toReturn = char32((uint32_t)0);
	if (utf32 <= 0x7F) {
		toReturn.c = utf32;
		toReturn.size = 1;
	} else if (utf32 <=   0x07FF) {
		toReturn.c += 0b10000000 + ((utf32)     & 0b00111111);
		toReturn.c <<= 8;
		toReturn.c += 0b11000000 + ((utf32>>6)  & 0b00011111);
		toReturn.size = 2;
	} else if (utf32 <=   0xFFFF) {
		toReturn.c += 0b10000000 + ((char)(utf32)     & 0b00111111);
		toReturn.c <<= 8;
		toReturn.c += 0b10000000 + ((char)(utf32>>6)  & 0b00111111);
		toReturn.c <<= 8;
		toReturn.c += 0b11100000 + ((char)(utf32>>12) & 0b00001111);
		toReturn.size = 3;
	} else if (utf32 <= 0x10FFFF) {
		toReturn.c += 0b10000000 + ((utf32)     & 0b00111111);
		toReturn.c <<= 8;
		toReturn.c += 0b10000000 + ((utf32>>6)  & 0b00111111);
		toReturn.c <<= 8;
		toReturn.c += 0b10000000 + ((utf32>>12) & 0b00111111);
		toReturn.c <<= 8;
		toReturn.c += 0b11110000 + ((utf32>>18) & 0b00000111);
		toReturn.size = 4;
	}
	return toReturn;
}


std::ostream& operator<<(std::ostream& stream, const char32& c32) {
	char* asChar = c32.toChar();
	stream << asChar;
	free(asChar);
	return stream;
}

struct string32 {
	int size = 0;
	std::vector<char32> cs;

	string32(char* sd) {
		while (sd[0]) {
			char32 c32 = char32(&sd);
			cs.push_back(c32);
			size += c32.size;
		}
	}

	string32(std::string s) {
		char* sd = (char*)s.c_str();
		while (sd[0]) {
			char32 c32 = char32(&sd);
			cs.push_back(c32);
			size += c32.size;
		}
	}

	string32(string32 from, int start, int end) {
		cs.insert(cs.begin(), from.cs.begin()+start, from.cs.begin()+end);
		for (char32 c : cs)
			size += c.size;
	}

	string32() {}

  string32 substr(int start, int end=-1) {
  	if (end == -1)
  		end = cs.size();
		return string32(*this, start, end);
  }

	int length() const {
		return cs.size();
	}
	char32 operator[](int i) const {
		return cs[i];
	}
	void operator+=(string32 s) {
		for (char32 c : s.cs) {
			cs.push_back(c);
			size += c.size;
		}
	}
	void operator+=(char* s) {
		for (char32 c : string32(s).cs) {
			cs.push_back(c);
			size += c.size;
		}
	}
	void operator+=(char32 c) {
		cs.push_back(c);
		size += c.size;
	}
	string32 operator+(string32 s) {
		string32 toReturn = *this;
		toReturn += s;
		return toReturn;
	}

	string32 operator+(char* s) {
		string32 toReturn = *this;
		toReturn += s;
		return toReturn;
	}
  bool operator==(string32 s) {
		for (int i(0);i<s.cs.size();i++)
			if (s.cs[i]!=cs[i])
				return false;
		return true;
  }
  bool operator!=(string32 s) {
		return !(*this==s);
  }
	string32 replace(string32 find, string32 with) {
		string32 copyOfSelf = *this;
		int havematched(0);
		int findsize = find.cs.size();
		for (int index(0); index < copyOfSelf.cs.size(); index++) {
			if (copyOfSelf.cs.at(index) == find.cs.at(havematched))
				havematched++;
			else
				havematched = 0;
			if (havematched == findsize) {
				index -= findsize-1;
				copyOfSelf.cs.erase(copyOfSelf.cs.begin()+index, copyOfSelf.cs.begin()+index+findsize);
				copyOfSelf.cs.insert(copyOfSelf.cs.begin()+index, with.cs.begin(), with.cs.end());
				break;
			}
		}
		return copyOfSelf;
	}

	bool replaceSelf(string32 find, string32 with) {
		int havematched(0);
		int findsize = find.cs.size();
		for (int index(0); index < cs.size(); index++) {
			if (cs.at(index) == find.cs.at(havematched))
				havematched++;
			else
				havematched = 0;
			if (havematched == findsize) {
				index -= findsize-1;
				cs.erase(cs.begin()+index, cs.begin()+index+findsize);
				cs.insert(cs.begin()+index, with.cs.begin(), with.cs.end());
				return true;
			}
		}
		return false;
	}

	void replaceSelfAll(string32 findWrapper, string32 withWrapper) {
		while (replaceSelf(findWrapper, withWrapper));
	}

	string32 reverse() {
		string32 tmpStr;
		for (int i(cs.size());i>=0;i--)
			tmpStr += cs[i];
		return tmpStr;
	}

	bool replaceAroundSelfAsym(string32 leftFindWrapper, string32 rightFindWrapper, string32 leftWithWrapper, string32 rightWithWrapper) {
		int i = 0;

		int havematched = 0;
		int tofind (leftFindWrapper.cs.size());
		for (;i<len();i++) {
      if (havematched == tofind)
       	break;
      if (cs[i] == leftFindWrapper[havematched])
        havematched++;
      else
        havematched = 0;
    }

    if (!havematched)
      return false;

    havematched--;
    int si (i-tofind);
    int ei (0);

		havematched = 0;
		int rightTofind = rightFindWrapper.cs.size();
		for (;i<len();i++) {
      if (havematched == rightTofind) {
       	break;
      }
      if (cs[i] == rightFindWrapper[havematched])
        ei = i+1, havematched++;
      else
        havematched = 0;
    }

    if (!havematched)
    	return false;

    string32 inner = string32(*this, si+tofind, ei-rightTofind);
    inner = leftWithWrapper + inner + rightWithWrapper;
    cs.erase (cs.begin()+si, cs.begin()+ei);
    cs.insert(cs.begin()+si, inner.cs.begin(), inner.cs.end());
    rebuildSize();
		return true;
	}

	void replaceAroundSelfAsymAll(string32 leftFind, string32 rightFind, string32 leftWith, string32 rightWith) {
		while (replaceAroundSelfAsym(leftFind, rightFind, leftWith, rightWith));
	}

	bool replaceAroundSelf(string32 findWrapper, string32 withWrapper) {
		return replaceAroundSelfAsym(findWrapper, findWrapper.reverse(), withWrapper, withWrapper);
	}

	void replaceAroundSelfAll(string32 find, string32 with) {
		while (replaceAroundSelf(find, with));
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
		if (temporary.length()>0)
			toReturn.push_back(temporary);
		return toReturn;
	}

	char* asChar() {
		char* toReturn = (char*) calloc(size, 1);
		int index = 0;
		for (char32 c : cs) {
			memcpy(toReturn+index, c.toChar(), c.size);
			index += c.size;
		}
		return toReturn;
	}

	int len() {
		return cs.size();
	}

	void rebuildSize() {
		size = 0;
		for (char32 c : cs)
			size += c.size;
	}
};

string32 operator+(char* s1, string32 s2) {
	string32 toReturn = s1;
	toReturn += s2;
	return toReturn;
}

std::ostream& operator<<(std::ostream& stream, const string32& s32) {
	for (int i=0;i<s32.length();i++)
		stream << s32[i];
	return stream;
}

