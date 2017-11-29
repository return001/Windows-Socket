
#include"server.h"
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;
#pragma comment (lib,"Ws2_32.lib")

#define ip_buf_size 129
#define default_port 5000
#define message_buf_size 1024

Server::Server()
{
	cout << "��ʼ����������..." << endl;

	version = MAKEWORD(2, 2);
	addr_len = sizeof(SOCKADDR_IN);

	addr_server.sin_family = AF_INET;
	//�ṹ��SOCKADDR_IN�е�sin_port����Ҫ�����������ݸ�ʽ����ͨʮ����������Ҫ�Ƚ���ת��
	addr_server.sin_port = ::htons(default_port);
	//sin_port ͨ�Ŷ˿ںű����������ֽ����ݸ�ʽhost to network short
	//�ṹ��SOCKADDR_IN�л���һ��struct in_addr ����Ĭ����һ���ṹ�����Ϊ��sin_addr
	//struct in_addr�ṹ���У�ʹ������union������Ĭ�ϴ���һ��unionΪS_un
	//S_un�����ַ�ʽ������IP��ַ��Ϣ��
	//����IP��ַ������һ��4�ֽڵ��޷������ͣ��˽ṹ��Ҳ�Ͷ�Ӧ�����ֱ��淽ʽ��
	//1.S_un_b���ֽڱ��棻
	//2.S_un_w��˫�ֽڱ��棻
	//3.S_addr��4�ֽڱ��棻
	//����S_addr��Ҳ����4�ֽ�ֱ�ӱ���IP��ַ��Ϣ
	addr_server.sin_addr.S_un.S_addr = ADDR_ANY;
	memset(ip_buf, 0, ip_buf_size);

	result = ::WSAStartup(version, &wsadata);
	if (result != 0)
	{
		cout << "WSAStartup() function failed with error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "Windows Socket Application Programming Interface ��ʼ���ɹ�..." << endl;

	server = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET)
	{
		cout << "socket() function failed wit error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "���������׽��ִ����ɹ�..." << endl;

	result = ::bind(server, (SOCKADDR *)&addr_server, addr_len);
	if (result != 0)
	{
		cout << "bind() function failed with error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "�׽��ֺ��׽��ֵ�ַ��Ϣ�󶨳ɹ�..." << endl;

	result = ::listen(server, SOMAXCONN);
	if (result != 0)
	{
		cout << "listen() function failed with error: " << ::WSAGetLastError() << endl;
		::WSACleanup();
		system("pause");
		exit(1);
	}
	cout << "��ʼ����..." << endl;
	cout << "�������˴������..." << endl;
}
Server::~Server()
{
	::closesocket(server);
	::closesocket(client);
	::WSACleanup();
	cout << "�ر��׽���..." << endl;
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter);
//DWORD��double word��wordΪ2�ֽڣ���DWORDΪ4�ֽ�
//�����̺߳���CreateClientThread
//LPVOID��long point void ������ָ��
//lpParameter��

void Server::WaitForClient()
{
	while (true)
	{
		client = ::accept(server, (SOCKADDR *)&addr_client, &addr_len);
		if (client == INVALID_SOCKET)
		{
			cout << "accept() function failed with error: " << ::WSAGetLastError() << endl;
			::WSACleanup();
			system("pause");
			exit(1);
		}
		inet_ntop(addr_client.sin_family, &addr_client, ip_buf, ip_buf_size);
		//::InetNtop(addr_client.sin_family, &addr_client, PWSTR(ip_buf), ip_buf_size);
		cout << "һ���µ��û��Ѿ�����...IP��ַΪ: " << endl << ip_buf << endl << "�˿ں�Ϊ: " << endl <<
			::ntohs(addr_client.sin_port) << endl;

		h_thread = ::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)client, 0, nullptr);
		if (h_thread == NULL)
		{
			cout << "CreateThread() function failed with error: " << ::WSAGetLastError() << endl;
			::WSACleanup();
			system("pasue");
			exit(1);
		}
		::CloseHandle(h_thread);
	}
}

DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
	SOCKET client = (SOCKET)lpParameter;
	char message_buf[message_buf_size];
	int result = 0;
	int send_result = 0;
	do
	{
		memset(message_buf, 0, message_buf_size);
		result = ::recv(client, message_buf, message_buf_size, 0);
		if (result > 0)
		{
			if (strcmp(message_buf, "exit") == 0)
			{
				cout << "�û�����ر�����..." << endl;
				break;
			}
			cout << "�յ���Ϣ��" << message_buf << endl;
			send_result = ::send(client, message_buf, message_buf_size, 0);
			if (send_result == SOCKET_ERROR)
			{
				cout << "send() function failed with error: " << ::WSAGetLastError() << endl;
				::WSACleanup();
				::closesocket(client);
				system("pause");
				return 1;
			}
		}
		else if (result == 0)
		{
			cout << "���ӹر�..." << endl;
		}
		else
		{
			cout << "recv() function failed with error: " << ::WSAGetLastError() << endl;
			::closesocket(client);
			::WSACleanup();
			system("pause");
			return 1;
		}
	} while (result > 0);
	result = ::shutdown(client, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		cout << "shutdown() function failed with error: " << ::WSAGetLastError() << endl;
		::closesocket(client);
		::WSACleanup();
		system("pause");
		return 1;
	}
	return 0;
}

