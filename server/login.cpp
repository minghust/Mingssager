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
				{
					return LOGIN_SUCCESS;
				}	
				else
					return PSW_ERROR; // password error
			}
			else
			{
				getline(f, buf); // email
				getline(f, buf); // passwd
			}
		}
		return INVALID_USERNAME; // no such user
	}
	return FILE_ERROR;
}

bool GetbackPasswd(string userEmail, string newp)
{
	Md5Encode md5;
	string buf;
	fstream t(USER_INFO_FILE);
	if (t.is_open())
	{
		while (t.good())
		{
			getline(t, buf);
			if (buf.empty()) // no record
			{
				t.close();
				return false;
			}
			else
				break;
		}
	}
	t.close();

	fstream f(USER_INFO_FILE);
	ofstream tmp;
	tmp.open("tmp.txt", ios::out);
	if (f.is_open() && tmp.is_open())
	{
		while (f.good() && tmp.good())
		{
			getline(f, buf); // username
			if (buf.empty()) break; // get rid of the effect of the last '\n'
			tmp << buf << endl;
			getline(f, buf); // email
			tmp << buf << endl;
			if (buf == userEmail)
			{
				getline(f, buf); // passwd
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

void UpdateUserList(std::vector<UserList> &ul)
{
	// there must have records in info.txt
	ifstream f;
	string buf1;
	string buf2;
	f.open(USER_INFO_FILE, ios::in);
	if (f.is_open())
	{
		if (ul.size() == 0)
		{
			while (f.good())
			{
				getline(f, buf1); // name
				if (buf1.empty()) return;
				getline(f, buf2);
				getline(f, buf2);
				UserList tmp = { buf1, "xxxxx", false };
				ul.push_back(tmp);
			}
			return;
		}
		else
		{
			while (f.good())
			{
				getline(f, buf1); // name
				getline(f, buf2);
				getline(f, buf2);
				if (buf1 == ul[ul.size() - 1].name)
				{
					getline(f, buf1); // name
					getline(f, buf2);
					getline(f, buf2);
					while (f.good() && !buf1.empty()) // not the last user
					{
						UserList tmp = { buf1, "xxxxx", false };
						ul.push_back(tmp);
						getline(f, buf1); // name
						getline(f, buf2);
						getline(f, buf2);
					}
					return;
				}
			}
			return;
		}
	}
	return;
}