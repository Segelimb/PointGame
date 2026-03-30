
#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")


int main()
{
    std::cout << "TCP Client is run\n";


    int clientsock;  // сокет для клиента
    struct sockaddr_in addr; // адрес, на котром  будем "слушать", тут сразу будет и IP, и порт
    char buf[256]; // буфер для приёма, 256 от балды
    int bytes_read; // счётчик принятых байтиков
    WSAData wsad;

    WSAStartup(0x0202, &wsad);


    // описываем (создаём) сокет
    //      AF_INET - тип адреса в виде IP+порт
    //      SOCK_STREAM - тип сокета потоковый
    //      IPPROTO_TCP - протокол TCP
    clientsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientsock < 0)
    {
        perror("pochemuto socket not created");
        exit(1);
    }

    // заполняем структуру с адресом сервера
    // !!! вот тут х.з. нужно ли под ARM вызывать функции htons() и htonl() - Host-To-Net-Short и Host-To-Net-Long
    // !!! которые переставляют байты из "host" в "net" поледовательность (младший-старший)
    addr.sin_family = AF_INET; //тип нашего адреса в виде IP+порт
    addr.sin_port = htons(306); // порт
    addr.sin_addr.s_addr = 0x0100007F; // 127.0.0.1

    // подключаемся к серверу
    if (connect(clientsock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("pochemuto socket not connected");
        exit(2);
    }


    // цикл приёма/передачи
    // по одному TCP-соединению можно слать данные в двух направлениях
    while (1)
    {
            std::cout << "Insert message: ";
            std::cin >> buf;

        send(clientsock, buf, 256, 0);

        // ждём данные синхронно (т.е. "висим" пока хоть что-то не придёт)
        bytes_read = recv(clientsock, buf, 256, 0); // 0 - режим работы по умолчанию, есть и другие

        // выходим из цикла если очень долго ничего не приходило (0) или случилась ошибка (<0)
        if (bytes_read <= 0) break;

        std::cout << "TCPServer say : " << buf << "\n";
        // по идее тут может быть подсчёт и обработка полученных данных,
        // но для примера и отладки просто делаем эхо-возврат клиенту всех считанных данных 
    }

    // если попали сюда, значит клиент "отпал" или долго ничего не передаёт
    // закрываем сокет этого клиента и уходим на очередной круг для ожидания нового сеанса
    shutdown(clientsock, 0);
    closesocket(clientsock);

    return 0;

}
