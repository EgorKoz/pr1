#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <algorithm>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

class Program1 {
private:
	mutex mtx;
	string stroka;
	string stroka1;
	string zatiranie = "";
	bool isCharDigit;
	const char* bukvi = "KB";
	int summa = 0;
	ofstream write;
	ifstream read;
public:
	void getString() 
	{
		cout << "wwedite stroku" << endl;
		cin >> stroka;
	}
	void getStringAgain()
	{
		getString();
		if (checkString())
		{
			sortString();
			changeString();
			writeToFile(stroka);
		}
		else 
		{
			getStringAgain();
		}
	}
	bool checkString()
	{
		for (int i = 0;i < stroka.length();i++) 
		{
			(isdigit(stroka[i]))? isCharDigit = true: isCharDigit = false;
		}
		if (stroka.length() < 65 && isCharDigit)
		{
			cout << "ok" << endl;
			return true;
		}
		else 
		{
			cout << "not ok" << endl;
			return false;
		}
	}
	void sortString()
	{
		sort(stroka.begin(), stroka.end());
		reverse(stroka.begin(),stroka.end());
	}
	void changeString()
	{
		for (int i = 0;i < stroka.length()-1;i=i+3)	
		{
			int n = i+1;
			stroka.replace(n, 1, bukvi);
		}
	}
	void readFromFile()
	{
		read.open("buff.txt");
		read >> stroka1;			
		read.close();
	}
	void writeToFile(string str)
	{
		write.open("buff.txt");
		write << str;
		write.close();
	}
	void printToConsole()
	{
		cout << stroka1 << endl;
	}
	void sumOfNumValues()
	{
		for (int i = 0;i < stroka1.length();i++)
		{
			if (isdigit(stroka1[i]))
			{
				summa=summa+stroka1[i]-48;
			}
		}
	}
	void thread1()
	{
		{
			mtx.lock();
			getString();
			if (checkString())
			{
				sortString();
				changeString();
				writeToFile(stroka);
			}
			else
			{
				getStringAgain();
			}
			mtx.unlock();
		}
	}
	void thread2()
	{
		this_thread::sleep_for(chrono::milliseconds(500));
		{
			mtx.lock();
			readFromFile();
			writeToFile(zatiranie);
			mtx.unlock();
			printToConsole();
			sumOfNumValues();			
		}
	}
	
};

int main()
{
	int sock;
	struct sockaddr_in addr;
	while(true) 
	{
		Program1 p;
		thread t1([&]()
			{
				p.thread1();
			});
		
		thread t2([&]()
			{
				p.thread2();
			});
		t1.join();
		t2.join();
	}
	
	
	return 0;
}
