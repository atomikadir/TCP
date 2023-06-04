#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "127.0.0.1"  // IP adresi 
#define BUFLEN 1024  // mesajin max uzunlugu
#define PORT 6161  // port 

void sendd(SOCKET* atom_client,mutex* mtx);
void recvv(SOCKET* atom_client,mutex* mtx);

int main() {
    system("title TCP Client"); // cmd basligibi degistirir.

    WSADATA wsaData;
    // winsock baslatilir.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Baslatilmadi. Hata kodu:%d\n", WSAGetLastError());
        //return 1;
    }
    printf("Baslatildi.\n");

    // soket olusturma
    SOCKET atom_client;
    atom_client = socket(AF_INET, SOCK_STREAM, 0);  
    if (atom_client == INVALID_SOCKET) {
        printf("Istemci soketi olusturulamadi. Hata Kodu: %d\n", WSAGetLastError());
        WSACleanup();
        //return 1;
    }
    printf("soket olustu\n");

    //server ayarlari
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(SERVER);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // dinlemekte olan server'a baglanir.
    if (connect(atom_client, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("baglanti kurulamadi. Hata kodu: %d", WSAGetLastError());
        closesocket(atom_client);
        WSACleanup();
        //return 1;
    }
    printf("baglanti kuruldu.\n");

    mutex mtx;

    thread first(sendd, &atom_client,&mtx);
    thread second(recvv, &atom_client,&mtx); 

    first.join(); 
    second.join();

    WSACleanup();
    closesocket(atom_client);
    return 0;
}

void sendd(SOCKET* atom_client,mutex* mtx) {
    bool running = true;
    while (running) {
        char message[BUFLEN] = {};
        
        printf("Client:");

        //gönderilecek mesaj kullanicidan alinir.
        cin.getline(message, BUFLEN);

        // eger cikis yapilmak istenirse 'q' yollanir. 
        if (strcmp(message, "quit") == 0)
        {
            running = false;
            WSACleanup(); 
            closesocket(*atom_client);
            continue;
        }
        //kullanicidan alinan mesaj gonderilir.
        if (send(*atom_client, message, strlen(message)+1, 0) == SOCKET_ERROR)
        {
            printf("mesaj gonderilemedi. Hata kodu: %d", WSAGetLastError());
        } 
    }

}

void recvv(SOCKET* atom_client,mutex* mtx) {
     
    while (true) {
        // Olurda bir cevap gelirse answer temizlenir. ve gelen cevap answer tamponunda tutulur.
        char answer[BUFLEN];
        memset(answer, 0, sizeof(answer));
        // gelen cevap recvfrom ile alinir. yanit gelene kadar program burada bekler.
        if (recv(*atom_client, answer, sizeof(answer) - 1, 0) == SOCKET_ERROR)
        {
            printf("mesaj alinamadi. Hata kodu: %d", WSAGetLastError());
        }
        //gelen cevap ekrana yazilir
        cout << "Server: " << answer << "\n" << "Client:";
    }
}
