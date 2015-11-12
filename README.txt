The goal of this project is to make a small wrapper for win32 processes in the vein of Qt's QProcess.

I had a project idea specifically for windows (at least for now) and I didn't want to have Qt as dependency just for that, so I wrote this class.

Requirements: a compiler supporting the c++11 standard. I don't make much use of it, and the parts that do make use of it could be modified to not use it.

I've included a basic test (I've only tested this with mingw-w64).
To compile the test
g++ process.cpp test.cpp -std=c++11

Contributions welcome!
