#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996)

#define BUFLEN 1024
#define PORT 6161

void sendd(SOCKET* atom_client, mutex* mtx);
void recvv(SOCKET* atom_client, mutex* mtx);

int main() {
	system("title TCP Server"); // cmd basligi degistirir.

	WSADATA wsaData;
	// winsok baslatilir.
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed. Hata kodu:%d\n", WSAGetLastError());
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

	//listen handshake 1
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
	// handshake 3. baglanan client'ı kabul eder.
	atom_client = accept(atom_server, (sockaddr*)&client, &clientLength);
	if (atom_client == INVALID_SOCKET) {
		printf("baglanamadi.Hata kodu:%d\n", WSAGetLastError());
		closesocket(atom_client);
		WSACleanup();
		return 1;
	}
	printf("kabul edildi.\n");

	mutex mtx;

	thread first(recvv, &atom_client, &mtx);
	thread second(sendd, &atom_client, &mtx);

	first.join();
	second.join();

	//istemci baglantisi kapat
	closesocket(atom_server);
	closesocket(atom_client);
	WSACleanup();

	return 0;
}

void sendd(SOCKET* atom_client, mutex* mtx) {
	bool running = true;
	while (running) {
		char answer[BUFLEN] = {};

		printf("Server:");
		// gönderilecek mesaj kullanicidan alinir.
		cin.getline(answer, BUFLEN);

		// eger cikis yapilmak istenirse 'q' yollanir. 
		if (strcmp(answer, "quit") == 0)
		{
			running = false;
			WSACleanup();
			closesocket(*atom_client);
			continue;
		}

		//kullanicidan alinan mesaj gönderilir.
		if (send(*atom_client, answer, strlen(answer) + 1, 0) == SOCKET_ERROR) {
			printf("mesaj gonderilemedi. Hata kodu: %d", WSAGetLastError());
		}
	}
}
void recvv(SOCKET* atom_client, mutex* mtx) {

	// mesaj alıp gonderme
	while (true) {
		char message[BUFLEN] = {};

		// gelen cevap recvfrom ile alinir. yanit gelene kadar program burada bekler
		if (recv(*atom_client, message, sizeof(message) - 1, 0) == SOCKET_ERROR) {
			printf("mesaj alinamadi. Hata kodu: %d", WSAGetLastError());
		}
		printf("Client: %s\n", message);
		printf("Server:");
	}
}
