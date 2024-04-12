#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdbool.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996)
#define DEFAULT_BUFLEN 4096

int ais_id = 126751;
COORD point;
short x = 0;
short y = 8;
//Winsock
WSADATA wsaData;
int iResult;
// Vytvorenie socketu
SOCKET ConnectSocket = INVALID_SOCKET;
FILE* subor;

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
void sifra(char* string) {
    int kluc = 55;
    char odpoved[149];
    int i = 0;

    while (i < 148) {
        odpoved[i] = (string[i] == '\0') ? '7' : (string[i] ^ kluc);
        i++;
    }
    odpoved[i] = '\0';

    printf("Dekodovany Text: ");
    for (int j = 0; odpoved[j] != '\0'; j++) {
        printf("%c", odpoved[j]);
    }
    printf("\n");
}

void prvocisla(char* sifricka) {
    printf("Vysledok je: ");
    int dlzka = strlen(sifricka);
    for (int i = 1; i < dlzka; i++) {
        bool je_prvocislo = true;
        for (int j = 2; j * j <= i + 1; j++) {
            if ((i + 1) % j == 0) {
                je_prvocislo = false;
                break;
            }
        }
        if (je_prvocislo)
            printf("%c", sifricka[i]);
    }
    printf("\n");
}


char odoslanie() {
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Odoslanie dat
    SetConsoleTextAttribute(hConsole, 1);
    printf("USER> ");
    fprintf(subor, "USER> ");
    char sprava[1024];
    scanf(" %[^\n]", &sprava);


    const char* data_to_send = sprava;
    int sendbuf_len = strlen(sprava);
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
    fprintf(subor, " %s\n", sprava);
}

char* prijatie() {
    // Prijatie
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 10);
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    fprintf(subor, "SERVER> ");

    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        recvbuf[iResult] = '\0';
    }
    else if (iResult == 0) {
        printf("====Pripojenie ukoncene serverom====\n");
    }
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    int sirka = csbi.dwSize.X;
    int stred = sirka / 2;
    char* slovo_a_medzera;
    int dlzka_riadku = 0;
    int je_prve_v_riadku = 1;
    char* dalsie_slovo_a_medzera = NULL;

    fprintf(subor, " %s\n", recvbuf);
    slovo_a_medzera = strtok_s(recvbuf, " ", &dalsie_slovo_a_medzera);  // &next_token ako context buffer
    printf("\n");
    for (int i = 0; i < stred; i++) {
        printf(" ");
    }
    printf("SERVER>");
    while (slovo_a_medzera != NULL) {
        //Ukoncenie riadka aby neodseklo slovo
        int dlzka_slova = strlen(slovo_a_medzera);
        if ((dlzka_riadku + dlzka_slova + 1 > stred) || je_prve_v_riadku) {
            printf("\n");
            for (int j = 0; j < stred; j++) {
                printf(" ");
            }
            dlzka_riadku = 0;
            je_prve_v_riadku = 0;
        }

        //Vypisanie slova po zisteni
        for (int k = 0; k < dlzka_slova; k++) {
            printf("%c", slovo_a_medzera[k]);
            Sleep(10);
        }
        printf(" ");

        dlzka_riadku += dlzka_slova + 1;
        slovo_a_medzera = strtok_s(NULL, " ", &dalsie_slovo_a_medzera);
    }
    SetConsoleTextAttribute(hConsole, 8);
    printf("\nPrijate byty: %d\n", iResult);
    if (iResult == 405) {
        int vysledok = pocitanie(ais_id);
        printf("Vysledok je: %d\n", vysledok);
    }
    if (iResult == 148) {
        sifra(recvbuf);
    }
    if (iResult == 23) {
        prvocisla(recvbuf);
    }
    SetConsoleTextAttribute(hConsole, 15);

}


int main() {
    //Winsock
    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleOutputCP(CP_UTF8);
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
    printf("Pripojenie na server bolo uspesne!\n");
    if ((subor = fopen("log.txt", "w")) == 0) {
        printf("Bratku nedal som to a neotvoril som subor :(");
        return -2;
    }
    while (1) {
        if ((subor = fopen("log.txt", "a")) == 0) {
            printf("Bratku nedal som to a neotvoril som subor :(");
            return -2;
        }
        odoslanie();
        prijatie();
        fclose(subor);
    }
    // Ukoncenie
    if (fclose(subor) == EOF)
        printf("Subor sa nepodarilo zatvorit\n");
    closesocket(ConnectSocket);
    freeaddrinfo(result);
    WSACleanup();
    return 0;
}