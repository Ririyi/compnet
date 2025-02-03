#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define SRV_PORT 1234
#define BUF_SIZE 64

const string QUEST = "Who are you?\n";

int main() {
    char buff[1024];

    // Инициализация библиотеки Winsock
    if (WSAStartup(MAKEWORD(2, 2), (WSADATA*)&buff[0])) {
        cout << "Error WSAStartup \n" << WSAGetLastError(); // Ошибка!
        return -1;
    }

    SOCKET s, s_new;
    int from_len;
    char buf[BUF_SIZE] = { 0 };
    sockaddr_in sin, from_sin;

    // Создание сокета
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        cout << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    // Настройка адреса сервера
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY; // Принять любой адрес
    sin.sin_port = htons(SRV_PORT);

    // Привязка сокета
    if (bind(s, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR) {
        cout << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return -1;
    }

    // Ожидание подключения клиентов
    listen(s, 3);

    while (true) {
        from_len = sizeof(from_sin);
        s_new = accept(s, (sockaddr*)&from_sin, &from_len);
        if (s_new == INVALID_SOCKET) {
            cout << "Accept failed: " << WSAGetLastError() << endl;
            continue; // Продолжить ожидание других клиентов
        }

        cout << "New connected client!" << endl;
        string msg = QUEST;

        while (true) {
            // Отправка вопроса клиенту
            send(s_new, msg.c_str(), msg.size(), 0);
            from_len = recv(s_new, buf, BUF_SIZE, 0);

            if (from_len < 0) {
                cout << "Receive failed: " << WSAGetLastError() << endl;
                break; // Выход из внутреннего цикла при ошибке
            }

            buf[from_len] = '\0'; // Завершение строки
            string msg1 = buf; // Полученное сообщение
            cout << msg1 << endl;

            if (msg1 == "Bye") break; // Завершение общения с клиентом

            getline(cin, msg); // Чтение нового сообщения от сервера
        }

        cout << "Client is lost" << endl;
        closesocket(s_new); // Закрытие соединения с клиентом
    }

    closesocket(s); // Закрытие основного сокета
    WSACleanup(); // Освобождение ресурсов Winsock
    return 0;
}
