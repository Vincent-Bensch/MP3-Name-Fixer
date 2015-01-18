#include <sstream>
#include <string>
#include <id3lib>
#include <fstream>
#include <iostream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <Windows.h>
#include <strsafe.h>
#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 3000 > Nul & Del \"%s\"")

void DelMe()
{
	TCHAR szModuleName[MAX_PATH];
	TCHAR szCmd[2 * MAX_PATH];
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	GetModuleFileName(NULL, szModuleName, MAX_PATH);

	StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);

	CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

using namespace std;

string alphabet = """abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'""";
string pre = """[]{}()- """;
//dir /s /b /o:gn>filelist.txt
vector<string> SplitFilename(const std::string& str)
{
	vector<string> out;
	unsigned found = str.find_last_of("/\\");
	out.push_back(str.substr(0, found));
	out.push_back(str.substr(found + 1));
	return out;
}

string newname(string name)
{
	int pos;
	int minpos = name.length();
	
	for (int i = 0; i < alphabet.length(); i++)
	{
		pos = name.find(alphabet[i]);
		if (pos != string::npos)
		{
			minpos = min(pos, minpos);
		}
	}
	int i = 1;
	name[0] = toupper(name[0]);
	while (name[i])
	{
		if (name[i] == '_'){ name[i] = ' '; }
		if (pre.find(name[i - 1]) != string::npos){name[i] = toupper(name[i]);}
		else{name[i] = tolower(name[i]); }
		i++;
	}
	return name.substr(minpos);
}

void main()
{
	ofstream log("log.txt");
	string line;
	string strpath;
	string stroldname;
	string strnewname;
	int hreturn;
	int ireturn;
	const char *chroldname;
	const char *chrnewname;
	const char *chrdir;
	vector<string> temp;
	int ia=1;

		cout << "Loop: " << ia << endl;
		system("dir / s / b / o:gn>list.txt");
		cout << "Made list" << endl;
		ifstream filelist("list.txt");
		if (filelist.is_open())
		{
			cout << "List open" << endl;
			while (getline(filelist, line))
			{
				temp = SplitFilename(line);
				strpath = temp[0];
				stroldname = temp[1];
				temp.clear();
				strnewname = newname(stroldname);
				if (stroldname.compare(strnewname) != 0)
				{

					log << "Renaming: " << stroldname << " ---> " << strnewname << "...";
					chroldname = stroldname.c_str();
					chrnewname = strnewname.c_str();
					chrdir = strpath.c_str();
					_chdir(chrdir);
					//MoveFileEx(chroldname, chrnewname);
					ireturn = rename(chroldname, chrnewname);
					if (ireturn == 0){log << "renamed" << endl;}
					else
					{
						hreturn = remove(chroldname);
						if (hreturn == 0){ log << "deleted" << endl; }
						else{ log << "fail" << endl; }
					}
				}
			}
			filelist.close();
		}
		cout << "List closed" << endl;

	cout << "Done" << endl;
	cin.ignore();
	log.close();
	remove("list.txt");
	DelMe();
	return;
}

