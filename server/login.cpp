#include "login.h"
#include <cstdlib>

using namespace std;

bool SaveInfo(string regName, string regEmail, string regPasswd)
{
	Md5Encode md5;
	if (regName.empty() || regEmail.empty() || regPasswd.empty())
		return false;
	ofstream f;
	f.open(USER_INFO_FILE, ios::out | ios::app);
	if (f.is_open())
	{
		f << regName << endl << regEmail << endl << md5.Encode(regPasswd) << endl;
		f.close();
		return true;
	}
	return false;
}


int VerifyUserLogin(string loginName, string loginPasswd)
{
	Md5Encode md5;
	fstream f;
	f.open(USER_INFO_FILE, ios::in);
	string buf;
	if (f.is_open())
	{
		while (f.good())
		{
			getline(f, buf); // username
			if (buf == loginName)
			{
				getline(f, buf); // email
				getline(f, buf); // passwd
				if (buf == md5.Encode(loginPasswd))
					return LOGIN_SUCCESS;
				else
					return PSW_ERROR; // password error
			}
			else
			{
				getline(f, buf);
				getline(f, buf);
			}
		}
		return INVALID_USERNAME; // no such user
	}
	return FILE_ERROR;
}

bool GetbackPasswd(string userEmail, string newp)
{
	Md5Encode md5;
	fstream f(USER_INFO_FILE);
	ofstream tmp;
	tmp.open("tmp.txt", ios::out | ios::app);
	string buf;
	if (f.is_open() && tmp.is_open())
	{
		while (f.good() && tmp.good())
		{
			getline(f, buf); // username
			tmp << buf << endl;
			getline(f, buf); // email
			tmp << buf << endl;
			if (buf == userEmail)
			{
				getline(f, buf); // email
				tmp << md5.Encode(newp) << endl;
			}
			else
			{
				getline(f, buf); // passwd
				tmp << buf << endl;
			}
		}
		f.close();
		tmp.close();
		system("del info.txt");
		system("ren tmp.txt info.txt");
		return true;
	}
	return false;
}
