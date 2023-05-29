#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define BUFLEN 1024
#define PORT 6161
#define SERVER "127.0.0.1"

int main() {
	system("title TCP Server");

	WSADATA wsaData;
	
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed. Hata kodu:%d\n",WSAGetLastError());
		return 1;
	}
	printf("WSAStartup Success.\n");

	//soket olusturma
	SOCKET atom_server;
	atom_server = socket(AF_INET, SOCK_STREAM, 0);
	if (atom_server == INVALID_SOCKET) {
		printf("soket olusturulamadi. Hata kodu:%d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("soket olusturuldu.\n");
	
	//server ayarlari
	sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	//bind
	if (bind(atom_server, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("baglanamadi.Hata kodu:%d\n", WSAGetLastError());
		closesocket(atom_server);
		WSACleanup();
		return 1;
	}
	printf("baglandi.\n");
	
	//listen
	if (listen(atom_server, 1) == SOCKET_ERROR) {
		printf("dinleme basarisiz. Hata kodu:%d\n", WSAGetLastError());
		closesocket(atom_server);
		WSACleanup();
		return 1;
	}
	printf("dinlendi.\n");

	SOCKET atom_client;
	sockaddr_in client;
	int clientLength = sizeof(client);

	printf("baglanti bekleniyor..\n");

	atom_client = accept(atom_server, (sockaddr*)&client, &clientLength);
	if (atom_client == INVALID_SOCKET) {
		printf("baglanamadi.Hata kodu:%d\n", WSAGetLastError());
		closesocket(atom_client);
		WSACleanup();
		return 1;
	}
	printf("kabul edildi.\n");
	//ntohs(client.sin_port);

	bool running = true;

	while (true) {
		char message[BUFLEN] = {};
		//memset(message, 0, sizeof(message));
		// gelen cevap recvfrom ile alinir. yanit gelene kadar program burada bekler
		if (recv(atom_client, message, sizeof(message)-1,0 ) == SOCKET_ERROR) {
			printf("mesaj alinamadi. Hata kodu: %d", WSAGetLastError());
			exit(0);
		}
		printf("Client: %s\n", message);
		printf("Server:");

		char answer[BUFLEN] = {};
		// gönderilecek mesaj kullanicidan alinir.
		cin.getline(answer, BUFLEN);

		// eger cikis yapilmak istenirse 'q' yollanir. 
		if (strcmp(message, "q") == 0)
		{
			running = false;
			continue;
		}

		//kullanicidan alinan mesaj gönderilir.
		if (send(atom_client, answer, strlen(answer), 0) == SOCKET_ERROR) {
			printf("mesaj gonderilemedi. Hata kodu: %d", WSAGetLastError());
			return 1;
		}
	}
	//istemci baglantisi kapat
	closesocket(atom_server);
	closesocket(atom_client);
	WSACleanup();

	return 0;
}