#include <sstream>
#include <string>
//#include <id3lib>
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

using namespace std;

string alphabet = """abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'""";
string prefixes = """[]{}()- """;
ofstream logfile("log.txt");

void SelfDestruct()
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

vector<string> SplitFilename(const std::string& str)
{
	vector<string> out;
	unsigned found = str.find_last_of("/\\");
	out.push_back(str.substr(0, found));
	out.push_back(str.substr(found + 1));
	return out;
}

string ExePath() {
	char buffer[MAX_PATH];
	wchar_t temp[260];
	mbstowcs(temp, buffer, strlen(buffer) + 1);//Plus null
	LPWSTR ptr = temp;
	GetModuleFileName(NULL, temp, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

string remove_non_letters(string name)
{
	int firstletter = name.length();

	for (int i = 0; i < alphabet.length(); i++)
	{
		if (name.find(alphabet[i]) != string::npos){ firstletter = min(name.find(alphabet[i]), firstletter); }
	}
	return name.substr(firstletter);
}

string fix_capitalization(string name)
{
	name[0] = toupper(name[0]);

	for (int i; i < name.length(); i++)
	{
		if (name[i] == '_'){ name[i] = ' '; }
		if (prefixes.find(name[i - 1]) != string::npos){ name[i] = toupper(name[i]); }
		else{ name[i] = tolower(name[i]); }
	}
	return name;
}

void fixname(string path)
{
	vector<string> currentfile = SplitFilename(path);
	string strpath = currentfile[0];
	string stroldname = currentfile[1];
	string strnewname = fix_capitalization(stroldname);

	if (stroldname.compare(strnewname) != 0)
	{
		logfile << "Renaming: " << stroldname << " ---> " << strnewname << "...";

		_chdir(strpath.c_str());
		if (rename(stroldname.c_str(), strnewname.c_str()) == 0){ logfile << "renamed" << endl; }
		else if (remove(stroldname.c_str()) == 0){ logfile << "deleted" << endl; }
		else { logfile << "failed" << endl; }
	}
	return;
}



void main()
{
	string starting_directory;
	string line;

	for (int i; i < 5; i++)
	{
		cout << "Loop: " << i << endl;
		system("dir / s / b / o:gn>list.txt");
		cout << "Made list" << endl;

		ifstream filelist("list.txt");
		if (filelist.is_open())
		{
			cout << "List open" << endl;
			while (getline(filelist, line))
			{
				fixname(line);
			}
		}
		filelist.close();
	}
	cout << "List closed" << endl;
	
	cout << "Done" << endl;
	cin.ignore();
	logfile.close();
	remove("list.txt");
	SelfDestruct();
	return;
}
