#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#define DEFAULT_BUFLEN 4096
int ais_id=126751;
COORD point;
short x = 0;
short y = 8;


//Winsock
WSADATA wsaData;
int iResult;
// Vytvorenie socketu
SOCKET ConnectSocket = INVALID_SOCKET;

int pocitanie(number) {
    int rozdelene_cisla = number / 10;
    int posledne_cislo = rozdelene_cisla % 10;
    int delitel = (posledne_cislo == 0) ? 9 : posledne_cislo;
    int suma = 0;

    while (rozdelene_cisla > 0) {
        suma += rozdelene_cisla % 10;
        rozdelene_cisla /= 10;
    }

    int zvysok = (suma + posledne_cislo) % delitel;
    return zvysok;
}
void sifra() {
    char* cipher_text = "dRTERCZRDDVPRYX";
    int key = 55;

    char decrypted_text[148];

    int i = 0;
    while (cipher_text[i] != '\0') {
        decrypted_text[i] = cipher_text[i] ^ key;
        i++;
    }
    decrypted_text[i] = '\0';

    printf("Decrypted Text: %s\n", decrypted_text);
}

void odoslanie() {
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Odoslanie dat
    SetConsoleTextAttribute(hConsole, 1);
    printf("USER> ");
    
    char sprava[1024];
    int size = 1024;
    fgets(sprava, size, stdin);

    char sendbuf[DEFAULT_BUFLEN];
    const char* data_to_send = sprava;
    int sendbuf_len = strlen(data_to_send);
    SetConsoleTextAttribute(hConsole, 15);

    iResult = send(ConnectSocket, sprava, sendbuf_len, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %ld\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    SetConsoleTextAttribute(hConsole, 8);
    printf("\nPoslane byty: %d\n", iResult);
    SetConsoleTextAttribute(hConsole, 15);
}

void prijatie() {
    x = 55;
    y += 5;
    
    // Prijatie
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD point = { x, y };
    SetConsoleCursorPosition(hConsole, point);


    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        recvbuf[iResult] = '\0';
    }
    if (iResult == 405) {
        int vysledok = pocitanie(ais_id);
        printf("Vysledok je: %d", vysledok);
    }
    else if (iResult == 0) {
        printf("====Pripojenie ukoncene serverom====\n");
    }
    x++;
    y++;
    SetConsoleTextAttribute(hConsole, 10);
    short x1 = 0;
    printf("SERVER>");
    for (int i = 0; recvbuf[i] != '\0'; i++) {
        x = 55 + x1;
        x1++;
        
        COORD point = { x , y };
        SetConsoleCursorPosition(hConsole, point);
        

        putc(*(recvbuf + i), stdout);
        fflush(stdout);
        Sleep(10);
        if (x > 117) 
        {
            y++;
            x1 = 0;
        }
    
    }
    x = 0;
    y += 1;
    putchar('\n');
}


int main() {
    //Winsock
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001 > nul");
    WSADATA wsaData;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    // Info o adrese servera
    struct addrinfo hints, * result = NULL, * ptr = NULL;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // IP adresa server
    const char* server_address = "147.175.115.34";
    const char* server_port = "777";

    iResult = getaddrinfo(server_address, server_port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }


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
    printf("===============================================\n");
    printf(" _______ _________               _______  _______ \n");
    printf("(  ____ \\\\__   __/|\\     /|     (  ___  )(  ____ )\n");
    printf("| (    \\/   ) (   | )   ( |     | (   ) || (    )|\n");
    printf("| (_____    | |   | |   | |     | (___) || (____)|\n");
    printf("(_____  )   | |   | |   | |     |  ___  ||  _____)\n");
    printf("      ) |   | |   | |   | |     | (   ) || (      \n");
    printf("/\\____) |   | |   | (___) |     | )   ( || )      \n");
    printf("\\_______)   )_(   (_______)     |/     \\||/       \n");
    printf("===============================================\n");
    sifra();
    printf("Pripojenie na server bolo uspesne!\n");
    odoslanie();
    prijatie();
    //753422
    
    while (1) {
        COORD point = { x, y };
        odoslanie();
        prijatie();
    }
    



    // Ukoncenie
    closesocket(ConnectSocket);
    freeaddrinfo(result);
    WSACleanup();

    return 0;
}
