# Welcome to this awful thing I wrote for the fun of it.

This is an attempt to bodge proper UTF-8 support in to C++ because it seemed like a fun project.

I expect the performance to be abysmally slow and for it to not really be worth anyone's time trying to use (there has to be better libraries, eh), but if you want to use this bad code written (mostly) from 1am to 3am in the morning by someone 100% unqualified to be handling pointers, this is the "library" for you. I wouldn't be surprised if there were major security holes either, to be honest...

To use it, just include "utf8.h" or whatever the file name is at the current moment in time. I'll /probably (not)?/ update this in the future.

CC0 because I don't deserve credit for how awful this is.

Some example code because why not.

Printing a string:
```cpp
#include <iostream>
#include "utf8.h"

int main() {
  string32 test = "こんにちは！";
  std::cout << test << "\n";
}
```

Getting the third character of a string, without breaking UTF-8 strings.
```cpp
#include <iostream>
#include "utf8.h"

int main() {
  string32 test = "こんにちは！";
  std::cout << "The third character of `" << test << "` is: " << test[2] << "\n";
}
```

Iterating over a string:
```cpp
#include <iostream>
#include "utf8.h"

int main() {
  string32 test = "こんにちは！";
  for (int i(0);i<test.size();i++)
    std::cout << "Character at index (" << i << ") is: " << test[i] << ".\n";
}
```
