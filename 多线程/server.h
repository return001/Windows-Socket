//ͷ�ļ�server.h


#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<WS2tcpip.h>
using namespace std;

#pragma comment (lib,"Ws2_32.lib")

#define ip_buf_size 129
#define default_port 5000
#define message_buf_size 1024

class Server
{
public:
	Server();//���캯�� ��������
	~Server();//�������� ���ٶ���
	Server(const Server &) = delete;//���ú����������ɾ���ĺ������κ���ͼ����������Ϊ�����������ڴ���
	Server & operator=(const Server &) = delete;
	void WaitForClient();
private:
	WORD version;
	WSADATA wsadata;
	SOCKET server;
	SOCKET client;

	//��� ����Ķ���һһ��Ӧ��32λ�޷�������
	HANDLE h_thread;

	//�׽��ֵ�ַ��Ϣ
	SOCKADDR_IN addr_server;
	SOCKADDR_IN addr_client;
	int result;
	int addr_len;
	char ip_buf[ip_buf_size];
};

