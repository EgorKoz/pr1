#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <algorithm>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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
	void getString() //ввод строки
	{
		cout << "Enter a string with numbers" << endl;
		cin >> stroka;
	}
	void getStringAgain() //метод для повторного ввода строки
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
	bool checkString() //проверка строки на условия
	{
		if (stroka.length()<65)
		{
			for (int i = 0;i < stroka.length();i++) 
			{
				if(isdigit(stroka[i]))
				{
					isCharDigit = true;
					continue;
				}
				else
				{
					isCharDigit = false;
					break;
				} 
			}
			if (isCharDigit)
			{
				cout << "Ok" << endl;
				return true;
			}
			else 
			{
				cout << "Error! Please repeat input" << endl;
				return false;
			}
		}
		else
		{
			cout << "Error! Please repeat input" << endl;
			return false;
		}
	}
	void sortString()  //сортировка по убыванию
	{
		sort(stroka.begin(), stroka.end());
		reverse(stroka.begin(),stroka.end());
	}
	void changeString()  //замена чётных элементов на "KB"
	{
		for (int i = 0;i < stroka.length()-1;i=i+3)	
		{
			int n = i+1;
			stroka.replace(n, 1, bukvi);
		}
	}
	void readFromFile() //чтение из буфера
	{
		read.open("buff.txt");
		read >> stroka1;			
		read.close();
	}
	void writeToFile(string str)   //запись в буфер
	{
		write.open("buff.txt");
		write << str;
		write.close();
	}
	void printToConsole()    //вывод на консоль
	{
		cout <<"Data: "<< stroka1 << endl;
	}
	void sumOfNumValues()    //получение суммы численных значений
	{
		for (int i = 0;i < stroka1.length();i++)
		{
			if (isdigit(stroka1[i]))
			{
				summa=summa+stroka1[i]-48;
			}
		}
	}
	void sendTo(int socked) //передача значения 2 программе
	{
		char str[5];
		sprintf(str,"%d",summa);		
		send(socked, str, sizeof(str), 0);
	}
	void thread1()   //функция для 1 потока
	{
		{
			mtx.lock();    //мьютекс для синхронизации потоков. разделяет общий буфер
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
	void thread2(int socket) //функция для 2 потока
	{
		this_thread::sleep_for(chrono::milliseconds(500));
		{
			mtx.lock();
			readFromFile();
			writeToFile(zatiranie);
			mtx.unlock();
			printToConsole();
			sumOfNumValues();	
			sendTo(socket);		
		}
	}
	
};

int main()
{
	int sock;
	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	//connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    	{
        	perror("connect");
        	exit(2);
    	}

	while(true) 
	{
		Program1 p;
		thread t1([&]()
			{
				p.thread1();
			});
		thread t2([&]()
			{
				p.thread2(sock);
			});
		t1.join();
		t2.join();
	}
	
	
	return 0;
}
