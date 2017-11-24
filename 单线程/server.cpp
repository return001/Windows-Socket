// server.cpp : �������̨Ӧ�ó������ڵ㡣

#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")//��ʾ���ӵ�ws2_32.lib����⣬ws2_32��winsock2�Ŀ��ļ� �����ǿ��ļ�������
//#pragma comment(compiler)
//winsock2�൱������ϵͳ����ʹ�õ����֮�佻����һ���ӿ�
//CSocket��WSock32.dll��ʵ��
//Socket API��ws2_32.dll��ʵ��
//������dll��������ͬ��εĽӿڣ����ù�ϵ���£�
//����Ӧ�ó���>>CSocket(Wsock32.dll)>>Socket API(ws2_32)>>���紫��������
//dll��dynamic link library����̬���ӿ��ļ���Ӧ�ó�����չ����Windows�У�
//���Ӧ�ó�����һ�������Ŀ�ִ���ļ����ᱻ�ָ��һЩ��Զ����Ķ�̬���ӿ⣨dll����
//��ִ��ĳһ������ʱ����Ӧ��dll�ͻᱻ����

const char default_port[] = "6000";//Ĭ�϶˿�
const int recieve_buf_size = 256;//Ĭ�ϻ�������С
const int ip_buf_size = 65;

int main()
{
	int i_result = 0;

	WORD Version = MAKEWORD(2, 2);//MAKEWORD(2,2)ʹ��Winsock2.2�汾

								  //Windows Socket Asynchronous Data�ṹ��struct������WSAStartup()�����󷵻ص�Windows Socket���ݣ�
								  //����Winsock.dllִ�еĽ��
	WSADATA wsadata;

	//WSAStartup()���������ϵͳ˵����Ҫʹ���ĸ����ļ����øÿ��ļ��뵱ǰ��Ӧ�ó���󶨣�
	//�Ӷ����Ե��øð汾��socket�ĸ��ֺ���
	i_result = WSAStartup(Version, &wsadata);//wsadata����Windows socketִ�е���Ϣ
											 //����winsock�Ⲣȷ��winsock�汾��ϵͳ�����������wsaData��  

	if (i_result != 0)//WSAStartup()��������һ��int��ָ��ͨ�����ֵ����ʼ���Ƿ�ɹ�
	{
		cout << "WSAStartup() function failed with error :" << WSAGetLastError() << endl;
		WSACleanup();
		system("pause");
		return 0;
	}

	addrinfo *result;//�洢��ַ��Ϣ������
	addrinfo hints;//�Ƕ�result�İ�ʾ��ϣ��getaddrinfo()��������result��ֵ����ô���ģ��͸�hints���ó�ʲô����

	memset(&hints, 0, sizeof(hints));

	hints.ai_flags = AI_PASSIVE;//AI_PASSIVE���˱�־��λʱ����ʾ�����߽���bind()����������ʹ�÷��صĵ�ַ�ṹ�����˱�־����λʱ����ʾ����connect()����������ʹ��
	hints.ai_family = AF_INET;//IPv4Э��ջ
	hints.ai_socktype = SOCK_STREAM;//socket���ͣ���ʽ�׽���
	hints.ai_protocol = IPPROTO_TCP;//ai_protocolָ��Э������,��ȡ��ֵ��ȡ����ai_address��ai_socktype��ֵ

	SOCKET server = INVALID_SOCKET;
	SOCKET client = INVALID_SOCKET;

	//getaddrinfo()���ص�ַ��Ϣ����ַ��Ϣ�����addrinfo�ṹ��result�� 
	//ԭ�ͣ�int i_result=getaddrinfo(*hostname,*service,struct addrinfo *hints, struct addrinfo **result );
	//service��������������ʮ���ƵĶ˿ںţ�Ҳ�������Ѷ���ķ������ƣ���ftp��http��
	i_result = getaddrinfo(nullptr, default_port, &hints, &result);
	//��hostname������ΪNULL��AI_PASSIVE��λ���򷵻صĵ�ַ����ͨ���ַ��ͨ���ַ��ȫ0�����������ַ

	if (i_result != 0)
	{
		cout << "getaddrinfo() function failed with error :" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		system("pause");
		return 0;
	}
	//ԭ�ͣ�int socket( int af, int type, int protocol)����������ַ��Ϣresult����һ���������˵��׽���SOCKET server
	//���ɹ�����INVALID_SOCKET
	server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//ԭ�ͣ�SOCKET server=socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (server == INVALID_SOCKET)
	{
		cout << "socket() function failed with error :" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		system("pause");
		return 0;
	}
	//���������׽������ַ�����,��ַ��Ϣ�����addrinfo�ṹ��result��
	i_result = bind(server, result->ai_addr, result->ai_addrlen);
	//��������ʹ�ã��൱�ڿͻ��˵�connect()����
	//ԭ�ͣ�int i_result=bind(SOCKET server, result->ai_addr, result->ai_addrlen);
	if (i_result != 0)
	{
		cout << "bind() function failed with error :" << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(server);
		WSACleanup();
		system("pause");
		return 0;
	}
	freeaddrinfo(result);//�ͷŴ洢��ַ��Ϣ�Ŀռ�

	cout << "�������˽����ɹ�..." << endl;
	cout << "��ʼ����..." << endl;

	i_result = listen(server, SOMAXCONN);//���׽���server���ڼ������������������״̬
										 //�ȴ����Ӷ��е���󳤶ȣ�һ������ΪϵͳֵSOMAXCONN
	//ԭ�ͣ�int i_result=listen(SOCKET server, SOMAXCONN);
	if (i_result != 0)
	{
		cout << "listen() function failed with error :" << WSAGetLastError() << endl;
		closesocket(server);
		WSACleanup();
		system("pause");
		return 0;
	}

	cout << "��ʼ�����û�..." << endl;

	SOCKADDR_IN addr_client;//���������û��ĵ�ַ��Ϣ

	int len_addr = sizeof(addr_client);
	client = accept(server, (SOCKADDR *)&addr_client, &len_addr);//���׽ӿڽ���һ�����ӣ���ֵ��һ��SOCKET client
	//����ԭ�ͣ�SOCKET client=accept(SOCKET server, ��struct SOCKEADDR * �� &addr_client, &sizeof(addr_client));

	if (client == INVALID_SOCKET)
	{
		cout << "accept() function failed with error : " << WSAGetLastError() << endl;
		closesocket(server);
		WSACleanup();
		system("pause");
		return 0;
	}
	cout << "�û�����..." << endl;
	cout << "ip��ַת��..." << endl;

	char ip_buf[ip_buf_size];
	memset(ip_buf, 0, sizeof(ip_buf_size));
	inet_ntop(AF_INET, &addr_client, ip_buf, ip_buf_size);
	//numeric_to_presentation��IP��ַ����ֵ��ʽ�Ķ�������ʽת���ɵ��ʮ���Ƶ���ʽ

	cout << "�û�IP��ַ��" << ip_buf << endl;

	char recieve_buf[recieve_buf_size];//������Ϣ�Ļ�������
	int send_result = 0;
	do
	{
		memset(recieve_buf, 0, sizeof(recieve_buf));//������������0
		i_result = recv(client, ip_buf, ip_buf_size, 0);//���������ӵ����ݱ�����ʽ�׽ӿڽ������ݵĽ���
		//����ԭ��:int i_result=recv( SOCKET client,ip_buf, ip_buf_size, int flags);
		//flagһ������Ϊ0
		if (i_result > 0)
		{
			if (strcmp(ip_buf, "exit"))//exit��ʾ�ͻ�������Ͽ�����
			{
				cout << "�û�����ر�����..." << endl;
				break;
			}
			cout << "���յ����ֽ�����" << i_result << endl;
			cout << "���յ�����Ϣ��" << recieve_buf << endl;

			send_result = send(client, recieve_buf, i_result, 0);
			//����ԭ�ͣ�int send_result= send( SOCKET client, recieve_buf, int len, int flags);
			//len��ʾ�յ����ݵ��ֽ��������Բ���ʹ��recieve_buf_size������С�����ֵ
			//flag��ʾ���õķ�ʽ
			if (send_result!=0)
			{
				cout << "send() function failed with error: " << WSAGetLastError() << endl;
				closesocket(client);
				WSACleanup();
				system("pause");
				return 0;
			}
		}
		else if (i_result == 0)
		{
			cout << "�û����ӹر�..." << endl;
		}
		else
		{
			cout << "recv() function failed with error: " << WSAGetLastError() << endl;
			closesocket(client);
			WSACleanup();
			system("pause");
			return 0;
		}
	} while (i_result > 0);

	i_result = shutdown(client, SD_SEND);
	//����ԭ�ͣ�int i_result=shutdown( SOCKET client, int how);
	//how����־������������ֹ��Щ����
	//ȡֵΪ��SD_RECEIVE��SD_SEND��SD_BOTH(SD=shutdown)
	if (i_result!=0)
	{
		cout << "shutdown() function failed with error: " << WSAGetLastError() << endl;
		closesocket(client);
		WSACleanup();
		system("pause");
		return 0;
	}

	closesocket(server);
	WSACleanup();
	cout << "�ر�socket..." << endl;
	system("pause");
	return 0;
}

