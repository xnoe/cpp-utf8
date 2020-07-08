#include <iostream>
#include <string>
#include <cstdio>
#include <vector>

class char32 {
  unsigned int fetch32(char** cstr) {
    unsigned int r(0);
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
  unsigned int c;
  char32(unsigned int i) {c = i;}
  char32(const char* s) {c = fetch32((char**)&s);}
  char32(char** s) {c = fetch32(s);}
  bool operator==(char* cs) {return c==fetch32(&cs);}
  bool operator==(char32 cs) {return c==cs.c;}
  unsigned int operator>>(int a) const {return c>>a;}
};

std::ostream& operator<<(std::ostream& stream, const char32& c32) {
  stream << (char)(c32>>24) << (char)(c32>>16) << (char)(c32>>8) << (char)c32.c;
  return stream;
}

struct string32 {
  std::vector<char32> cs;
  string32(char* sd) {
    while (sd[0])
      cs.push_back(char32(&sd));
  }
  int size() const {return cs.size();}
  char32 operator[](int i) const {return cs[i];}
  void replace(string32 find, string32 with) {
		int havematched(0);
		int findsize = find.cs.size();
		for (int index(0); index < cs.size(); index++) {
			if (cs.at(index) == find.cs.at(havematched))
				havematched++;
			if (havematched == findsize) {
				index -= findsize-1;
				cs.erase(cs.begin()+index, cs.begin()+index+findsize);
				cs.insert(cs.begin()+index, with.cs.begin(), with.cs.end());
				break;
			}
		}
  }
};

std::ostream& operator<<(std::ostream& stream, const string32& s32) {
  for (int i=0;i<s32.size();i++)
    stream << s32[i];
return stream;
}
