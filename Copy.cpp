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


int _tmain(int argc, LPTSTR argv[]) {
    if (argc > 2) {
        Copy(argv[1], argv[2], false);

        Copy(argv[1], argv[2], true);
    }
    return 0;
}


bool Exists(const tstring& fp) {
    DWORD fg = GetFileAttributes(fp.c_str());
    if ((fg == DWORD(~0)) && (GetLastError() & ERROR_FILE_NOT_FOUND))
        return false;
    return (fg & FILE_ATTRIBUTE_DIRECTORY) ? false : true;
}

int Help(void) {
    TCHAR s[] = _T("1.Перезаписать? 2.Пропустить? 3.Перезаписывать для всех? 4.Отмена?");
    CharToOem(s, s);
    _putts(s);
    int n = 0;
    _tscanf(_T("%d"), &n);
    return n;
}

void Copy(const tstring& old_path, const tstring& new_path, bool move) {
    int     op;
    bool    exist = false, all = false;
    BOOL    ret;
    DWORD   fg;
    HANDLE  fp;
    tstring src, dst;
    WIN32_FIND_DATA fd;
    stack<tstring> srm;
    stack<pair<tstring, tstring> > fs;

    