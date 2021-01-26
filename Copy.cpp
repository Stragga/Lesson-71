#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <stack>
#include <string>

using namespace std;

typedef basic_string<TCHAR> tstring;
bool Exists(const tstring& fp);
int  Help(void);
void Copy(const tstring& old_path, const tstring& new_path, bool move);
void print(LPCTSTR fmt, ...);


