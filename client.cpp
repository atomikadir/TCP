#include <WinSock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server 172.16.26.71
#define BUFLEN 1024  // max length of answer
#define PORT 6161  // the port on which to listen for incoming data

int main() {
    system("title TCP Client");
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Baslatilmadi. Hata kodu:%d\n", WSAGetLastError());
        return 1;
    }
    printf("Baslatildi.\n");

    SOCKET atom_client;
    atom_client = socket(AF_INET, SOCK_STREAM, 0);
    if (atom_client == INVALID_SOCKET) {
        printf("Ýstemci soketi oluþturulamadý. Hata Kodu: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    printf("soket olustu\n");

    //server ayarlari
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (connect(atom_client, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("baglanti kurulamadi. Hata kodu: %d", WSAGetLastError());
        closesocket(atom_client);
        WSACleanup();
        return 1;
    }
    printf("baglanti kuruldu.\n");

    bool running = true;

    while (running) {
        char message[BUFLEN] = {};
        printf("Client:");
        //gönderilecek mesaj kullanicidan alinir.
        cin.getline(message, BUFLEN);

        // eger cikis yapilmak istenirse 'q' yollanir. 
        if (strcmp(message, "q") == 0)
        {
            running = false;
            continue;
        }

        //kullanicidan alinan mesaj gonderilir.
        if (send(atom_client, message, sizeof(message), 0) == SOCKET_ERROR)
        {
            printf("mesaj gonderilemedi. Hata kodu: %d", WSAGetLastError());
            return 3;
        }

        // Olurda bir cevap gelirse answer temizlenir. ve gelen cevap answer tamponunda tutulur.
        char answer[BUFLEN];
        memset(answer, 0, sizeof(answer));
        // gelen cevap recvfrom ile alinir. yanit gelene kadar program burada bekler.
        if (recv(atom_client, answer, sizeof(answer)-1, 0) == SOCKET_ERROR)
        {
            printf("mesaj alinamadi. Hata kodu: %d", WSAGetLastError());
            exit(0);
        }
        //gelen cevap ekrana yazilir
        cout << "Server: " << answer << "\n";
    }
    WSACleanup();
    closesocket(atom_client);
    return 0;
}