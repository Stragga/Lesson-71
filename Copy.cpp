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

    tstring d(new_path);
    tstring::size_type i = old_path.rfind(_T('\\'));
    if ((i + 1) == old_path.length()) {
        if ((i = old_path.rfind(_T('\\'), i - 1)) == tstring::npos)
            i = 0;
        else
            ++i;
    }
    else
        ++i;

    if ((d.length() > 0) && (d[d.length() - 1] != _T('\\')))
        d += _T('\\');

    tstring::const_iterator j = old_path.begin() + i;
    while ((j != old_path.end()) && (*j != _T(':')) && (*j != _T('\\')))
        d += *j++;

    tstring p(old_path);
    if ((p.length() > 0) && (p[p.length() - 1] != _T('\\')))
        p += _T('\\');
    p += _T('*');

    fs.push(std::make_pair(p, d));
    p.erase(p.end());

    while (!fs.empty()) {
        p.swap(fs.top().first);
        d.swap(fs.top().second);
        fs.pop();

        fp = FindFirstFile(p.c_str(), &fd);
        if (fp != INVALID_HANDLE_VALUE) {
            p.erase(p.end() - 2, p.end());
            exist = false;

            if (!CreateDirectoryEx(p.c_str(), d.c_str(), NULL)) {
                if (GetLastError() != ERROR_ALREADY_EXISTS) {
                    cout << _T("Не удалось создать папку, код ошибки: \n"), GetLastError();
                    break;
                }
                exist = true;
            }

            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if ((_tcscmp(fd.cFileName, _T(".")) != 0) && (_tcscmp(fd.cFileName, _T("..")) != 0)) {
                        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                            fs.push(std::make_pair(p + _T('\\') + fd.cFileName + _T("\\*"),
                                d + _T('\\') + fd.cFileName));
                        }
                    }
                }
                else {
                    src = p + _T('\\') + fd.cFileName;
                    dst = d + _T('\\') + fd.cFileName;
                    _tprintf(_T("> %s\n"), dst.c_str());

                    if (!all && exist && Exists(dst.c_str())) {
                        op = Help();
                        if (op == 3) {
                            all = true;
                            op = 1;
                        }
                        else if (op == 4) {
                            FindClose(fp);
                            return;
                        }
                    }
                    else
                        op = 1;

                    if ((op == 1) && (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
                        SetFileAttributes(dst.c_str(), FILE_ATTRIBUTE_NORMAL);

                    if (move) {
                        if (op == 1) {
                            fg = (_totupper(src[0]) != _totupper(dst[0])) ? MOVEFILE_COPY_ALLOWED : 0;
                            ret = MoveFileEx(src.c_str(), dst.c_str(), fg | MOVEFILE_REPLACE_EXISTING);
                            if (!ret)
                                cout << _T("Не удалось переместить файл, код ошибки: \n"), GetLastError();
                        }
                    }
                    else {
                        if (op == 1) {
                            if (!CopyFile(src.c_str(), dst.c_str(), FALSE))
                                cout << _T("Не удалось копировать файл, код ошибки: \n"), GetLastError();
                        }
                    }
                }
            } while (FindNextFile(fp, &fd));
            FindClose(fp);

            if (move)
                srm.push(p.c_str());
        }
        else {
            p.erase(p.end() - 1);
            _putts(p.c_str());
            cout << _T("Путь не найден, код ошибки: \n"), GetLastError();
        }
    }

    for (; !srm.empty(); srm.pop())
        RemoveDirectory(srm.top().c_str());
}

void print(LPCTSTR fmt, ...) {
    TCHAR s[128];
    CharToOem(fmt, s);
    va_list args;
    va_start(args, s);
    _vtprintf(s, args);
    va_end(args);
}