#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096

int main() {
    // Inicializácia Winsocku
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Informácie o adrese servera
    struct addrinfo hints, * result = NULL, * ptr = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Nahraďte IP adresu a port servera
    const char* server_address = "147.175.115.34";
    const char* server_port = "777";

    iResult = getaddrinfo(server_address, server_port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Vytvorenie socketu
    SOCKET ConnectSocket = INVALID_SOCKET;

    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket creation failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Pripojenie k serveru
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("Connection failed with error: %ld\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Connected to server!\n");

    // Odoslanie dát
    char sendbuf[DEFAULT_BUFLEN];  // Upravte veľkosť buffera podľa potreby
    const char* data_to_send = "Blok3";
    int sendbuf_len = strlen(data_to_send);

    iResult = send(ConnectSocket, sendbuf, sendbuf_len, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %ld\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Sent %d bytes of data.\n", iResult);

    // Prijímanie dát
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
        recvbuf[iResult] = '\0';  // Ukončenie prijatých dát null znakom pre tlač
        printf("Received data: %s\n", recvbuf);
    }
    else if (iResult == 0) {
        printf("Connection closed by server.\n");
    }
    else {
        printf("recv failed with error: %ld\n", WSAGetLastError());
    }

    // Upratovanie
    closesocket(ConnectSocket);
    freeaddrinfo(result);
    WSACleanup();

    return 0;
}
