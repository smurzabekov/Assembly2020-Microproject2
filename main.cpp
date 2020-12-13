#include <windows.h>
#include <iostream>
#include <vector>
#include <queue>
#include <conio.h>
#define MAXASSORT 10
using namespace std;
struct Pokupatel
{
	int id;		//����� ����������
	HANDLE pokupka;		//������� ��� ������� ���������
	vector<int> list;	//������ �������
};

struct Otdel
{
	int id;	//�������� ������
	vector<int> assort;	//����������� �������
	queue<Pokupatel*> q;	//������� �����������	
	HANDLE wakeup;		//������� ���������� ��������
	HANDLE och;			//������� ������� � �������
};

//�������� ���� �� ����� � �������
bool is_in_vector(vector<int> &v, int val)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == val)
			return true;
	}
	return false;
}
Otdel a, b;
HANDLE display,otpusk;
//�����, ������������ ��������� ��������
DWORD WINAPI Prodavec(PVOID param)
{
	Otdel *o = (Otdel *)param;
		while (true)
		{
		//	WaitForSingleObject(o->wakeup, INFINITE);
			WaitForSingleObject(o->och, INFINITE);	//�������������� ������ � �������
			WaitForSingleObject(otpusk, INFINITE);	//��������� ����� � ���� � �� �� ����� ����� ������ ���� ��������
			int sz = o->q.size();	
			if (sz)	//���� ������� �� �����
			{
				WaitForSingleObject(display, INFINITE);
				cout << "�������� ������ " << o->id << " ��������� ����� ���������� " << o->q.front()->id << endl;
				ReleaseMutex(display);
				SetEvent(o->q.front()->pokupka);	//�������������� ���������� ��� ����� �������
				o->q.pop();	//������� ���������� �� �������

			}
			ReleaseMutex(otpusk);	//���������� ��������
			ReleaseMutex(o->och);
			Sleep(1000);

	}
	return 0;
}
//�����, ������������ ��������� ����������
DWORD WINAPI Pokup(PVOID param)
{
	Pokupatel *p = (Pokupatel *)param;
	int i = 0;
	//���� �� ������ �������
	while (i < p->list.size())
	{
		//���� ��������� ����� �� ������ ���� � ������ �
		while (i < p->list.size() && is_in_vector(a.assort, p->list[i]))// a->check_assort(buy->list[i]))
		{
			//��������� � ������� � ���� �����
			WaitForSingleObject(a.och, INFINITE);
			a.q.push(p);
			ReleaseMutex(a.och);
			WaitForSingleObject(display, INFINITE);
			cout << "���������� " << p->id << " ���� � ������� � ����� 1 �� ������� " << p->list[i] << endl;
			ReleaseMutex(display);
//			SetEvent(a.wakeup);
			i++;
		}
		//���� ��������� ����� �� ������ ���� � ������ b
		while (i < p->list.size() && is_in_vector(b.assort, p->list[i]))
		{
			//��������� � ������� � ���� �����
			WaitForSingleObject(b.och, INFINITE);
			b.q.push(p);
			ReleaseMutex(b.och);
			WaitForSingleObject(display, INFINITE);
			cout << "���������� " << p->id << " ���� � ������� � ����� 2 �� ������� " << p->list[i] << endl;
			ReleaseMutex(display);
//			SetEvent(b.wakeup);
			i++;
		}
	}
	//����� � �������� � �������� ������
	for (int i = 0; i < p->list.size(); i++)
	{
		WaitForSingleObject(p->pokupka, INFINITE);	//������� ��������� �������

		WaitForSingleObject(display, INFINITE);
		cout << "���������� " << p->id << " ������� ����� " << p->list[i] << endl;
		ReleaseMutex(display);
	}
	//��� ������
	WaitForSingleObject(display, INFINITE);
	cout << "���������� " << p->id << " ��������� �������� � ������" << endl;
	ReleaseMutex(display);
	delete p;
	return 0;
}
int main()
{
	DWORD id;
	setlocale(LC_ALL, "Russian");//������� ������
	display = CreateMutex(NULL, FALSE, NULL);	//������� ������� � �������
	otpusk = CreateMutex(NULL, FALSE, NULL);	//������� ������� ������� ������
	a.och = CreateMutex(NULL, FALSE, NULL);		//�������� ������� � �������� �������
	b.och = CreateMutex(NULL, FALSE, NULL);
	a.wakeup = CreateEvent(NULL, FALSE, FALSE, NULL);	//������� ����������� ���������
	b.wakeup = CreateEvent(NULL, FALSE, FALSE, NULL);
	a.id = 1;
	//�������� ������������ ��� ������� ������
	cout << "����� 1 ����� ����������� �������:";
	for (int i = 0; i < MAXASSORT / 2; i++)
	{
		int z;
		do
		{
			z = rand() % MAXASSORT;
		} while (is_in_vector(a.assort, z));	//����� ������ ���������� ������������ �������
		a.assort.push_back(z);	//��������� ��
		cout << " " << a.assort.back();
	}
	cout << endl;
	CreateThread(NULL, 0, Prodavec, (LPVOID)&a, 0, &id);	//������� ����� �������� 1 ������
	b.id = 2;
	cout << "����� 2 ����� ����������� �������:";
	//�������� ������������ ��� ������� ������
	for (int i = 0; i < MAXASSORT / 2; i++)
	{
		int z;
		do
		{
			z = rand() % MAXASSORT;
		} while (is_in_vector(a.assort, z) || is_in_vector(b.assort, z));	//����� ������ ���������� ������������ ������� � ����� �� �� ���� � ������ ������
		b.assort.push_back(z);	//��������� ��
		cout << " " << b.assort.back();
	}
	cout << endl;
	CreateThread(NULL, 0, Prodavec, (LPVOID)&b, 0, &id);	//������� ����� �������� 2 ������
	cout << "Esc - �����, ������ ������� ��������� ���������� � �������" << endl;
	int npookup = 0;

	Pokupatel *p;
	srand(GetTickCount());
	while (_getch() != 27)		//�������� ������� �������
	{
		p = new Pokupatel();
		p->id = ++npookup;		
		int n = rand() % 5 + 5;	//������ ������ �������
		//������� ������ �������
		WaitForSingleObject(display, INFINITE);
		cout << "������ ���������� " << p->id << " �� ������� �������:";
		for (int i = 0; i < n; i++)	//���������� ������ �������
		{
			p->list.push_back(rand() % MAXASSORT);
			cout << " " << p->list.back();
		}
		cout << endl;
		ReleaseMutex(display);
		p->pokupka = CreateEvent(NULL, FALSE, FALSE, NULL);
		CreateThread(NULL, 0, Pokup, (LPVOID)p, 0, &id);
	}
	return 0;
}