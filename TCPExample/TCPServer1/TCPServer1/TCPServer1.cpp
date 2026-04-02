
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include "windows.h"
using namespace std;

#pragma comment(lib, "ws2_32.lib")





// 0 - пустота; " "
// 1 - вертикаль; 179
// 2 - горизонталь; 196
// 3 - перекрестие; 197
// 4 - точка; 4
// 5 - точка другого игрока; 4

void MovingCursor(int x, int y)
{
    COORD coord = { x , y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

vector <vector<int>> CreateField(vector <vector<int>> Field, int x, int y)
{
    int a, b;
    for (size_t i = 0; i < y * 2 + 1; ++i)
    {
        if (i % 2 != 0)
        {
            a = 3;
            b = 2;
        }
        else
        {
            a = 1;
            b = 0;
        }
        std::vector<int> temp;
        for (size_t j = 0; j < x * 2 + 1; ++j)
        {
            if (j % 2 != 0) temp.push_back(a);
            else temp.push_back(b);
        }
        Field.push_back(temp);
    }
    return Field;
}

void SetColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void PrintField(vector <vector<int>> Field, bool Motion)
{
    system("cls");
    if (Motion) cout << "Motion: Server" << endl << endl;
    else cout << "Motion: Client" << endl << endl;
    cout << " " << char(179);
    char score = 'A';
    for (int i = 0; i < Field.size(); ++i)
        cout << (i % 2 != 0 ? score++ : ' ');
    cout << endl << char(196) << char(197);
    for (int i = 0; i < Field.size(); ++i)
        cout << char(196);
    int sim;
    score = 'A';
    for (int i = 0; i < Field.size(); ++i)
    {
        cout << endl << (i % 2 != 0 ? score++ : ' ') << char(179);
        for (int j = 0; j < Field[i].size(); ++j)
        {
            switch (Field[i][j])
            {
            case 0:
                cout << char(32);
                break;
            case 1:
                cout << char(179);
                break;
            case 2:
                cout << char(196);
                break;
            case 3:
                cout << char(197);
                break;
            case 4:
                SetColor(9);
                cout << char(4);
                SetColor(7);
                break;
            case 5:
                SetColor(12);
                cout << char(4);
                SetColor(7);
                break;
            default:
                cout << char(63);
                break;
            }
        }
    }
    cout << endl << endl;
}

void PrintMenu(int x, int y)
{
    MovingCursor((x * 2 + 1) + 10, 2);
    cout << "MENU:";
    MovingCursor((x * 2 + 1) + 10, 4);
    cout << "[1] Give up";
    MovingCursor((x * 2 + 1) + 10, 6);
    cout << "[2] Suggest a draw";
    MovingCursor((x * 2 + 1) + 10, 8);
    cout << "[3] End the game";
    MovingCursor(0, 5 + (y * 2 + 1));
}





int main()
{
    std::cout << "TCP Server is run\n";


    int listener, sock;  // сокет для прослушки (ожидания) клиента и сокет для работы с сеансом
    struct sockaddr_in addr; // адрес, на котром  будем "слушать", тут сразу будет и IP, и порт
    char buf[256]; // буфер для приёма, 256 от балды
    int bytes_read; // счётчик принятых байтиков
    WSAData wsad;

    WSAStartup(0x0202, &wsad);


    // описываем (создаём) сокет для прослушки
    //      AF_INET - тип адреса в виде IP+порт
    //      SOCK_STREAM - тип сокета потоковый
    //      IPPROTO_TCP - протокол TCP
    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener < 0)
    {
        perror("pochemuto socket not created");
        exit(1);
    }

    // заполняем структуру с адресом
    // !!! вот тут х.з. нужно ли под ARM вызывать функции htons() и htonl() - Host-To-Net-Short и Host-To-Net-Long
    // !!! которые переставляют байты из "host" в "net" поледовательность (младший-старший)
    addr.sin_family = AF_INET; //тип нашего адреса в виде IP+порт
    addr.sin_port = htons(306); // порт
    addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY - будем принимать с любого доступного сетевого интерфейса (IP-адреса)

    // связываем ("биндим") сокет прослушки с адресом, т.е. иными словами открываем адрес для ожидания клиента
    if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("pochemuto socket not bind");
        exit(2);
    }

    // переводим сокет в режим ожидания клиента
    // замечание - тут ещё не происходит блокировки, а только задаётся режим сокета
    //       1 - количество ождидаемых клиентов, можно и больше ставить, но взяться им неоткуда в наших условиях
    listen(listener, 1);

    // цикл обработки клиента
    // замечание - далее ВСЕ сетевые функции блокирующие (синхронные)
    while (1)
    {

        // получаем сокет для работы с клиентом
        // это работает с блокировкой (синхронно), т.е. accept() завершится когда подключится клиент,
        // ну или по оооочень большому таймауту выйдем с ошибкой, если клиента долго не будет
        //      второй и третий параметры для переменной и её длины, в которую
        //      вернётся адрес подключившегося клиента, но нам это безразлично, потому NULL и NULL
        sock = accept(listener, NULL, NULL);
        if (sock < 0)
        {
            perror("pochemuto client not accepted");
            exit(3);
        }

        char Xch;
        int x, y;
        cout << "Enter the field size: ";
        cin >> Xch;
        x = (int)Xch - 48;
        y = x;
        char size[2] = { Xch, Xch };
        send(sock, size, 2, 0);
        bool Motion = 0;
        vector <vector<int>> Field;
        Field = CreateField(Field, x, y);
        char XCoordPoint, YCoordPoint;
        // цикл приёма/передачи
        // по одному TCP-соединению можно слать данные в двух направлениях
        while (1)
        {
            Motion = (Motion ? 0 : 1);
            PrintField(Field, Motion);
            PrintMenu(x, y);
            char coords[2];
            if (Motion)
            {
                cout << "Enter the coordinates of the point: ";
                cin >> XCoordPoint;
                switch (XCoordPoint)
                {
                case '1':
                    coords[0] = 0;
                    coords[1] = 0;
                    send(sock, coords, 2, 0);
                    break;
                case '2':
                    coords[0] = 1;
                    coords[1] = 0;
                    send(sock, coords, 2, 0);
                    break;
                case '3':
                    coords[0] = 0;
                    coords[1] = 2;
                    send(sock, coords, 2, 0);
                    break;
                default:
                    cin >> YCoordPoint;
                    XCoordPoint = toupper(XCoordPoint);
                    YCoordPoint = toupper(YCoordPoint);
                    while ((XCoordPoint < 65 || YCoordPoint < 65) || (XCoordPoint >= (x + 65) || YCoordPoint >= (y + 65)))
                    {
                        cout << "Enter valid coordinates: ";
                        cin >> XCoordPoint >> YCoordPoint;
                        XCoordPoint = toupper(XCoordPoint);
                        YCoordPoint = toupper(YCoordPoint);
                    }
                    coords[0] = XCoordPoint;
                    coords[1] = YCoordPoint;
                    Field[(XCoordPoint - 64.5) * 2][(YCoordPoint - 64.5) * 2] = (Motion ? 4 : 5);
                    send(sock, coords, 2, 0);
                    break;
                }
            }
            else
            {
                cout << "Wait for the client's move";
                bytes_read = recv(sock, coords, 2, 0);
                if (coords[0] == 0 && coords[1] == 0)
                {
                    system("cls");
                    cout << "The client has given up, you have won!";
                    break;
                }
                else if (coords[0] == 1 && coords[1] == 0)
                {
                    system("cls");
                    cout << "You have been offered a draw";
                    break;
                }
                else if (coords[0] == 0 && coords[1] == 2)
                {
                    system("cls");
                    cout << "The game is over";
                    break;
                }
                else
                Field[(coords[0] - 64.5) * 2][(coords[1] - 64.5) * 2] = (Motion ? 4 : 5);
            }
            
            
                        //// ждём данные синхронно (т.е. "висим" пока хоть что-то не придёт)
            //bytes_read = recv(sock, buf, 256, 0); // 0 - режим работы по умолчанию, есть и другие

            //// выходим из цикла если очень долго ничего не приходило (0) или случилась ошибка (<0)
            //if (bytes_read <= 0) break;
            //
            //std::cout << "TCPClient say : " << buf << "\n";

            //std::cout << "Insert message: ";
            //std::cin >> buf;
            //// по идее тут может быть подсчёт и обработка полученных данных,
            //// но для примера и отладки просто делаем эхо-возврат клиенту всех считанных данных 
            //send(sock, buf, 256, 0);
        }

        // если попали сюда, значит клиент "отпал" или долго ничего не передаёт
        // закрываем сокет этого клиента и уходим на очередной круг для ожидания нового сеанса
        shutdown(sock, 0);
        closesocket(sock);
    }

    //хоть цикл выше и бесконечный, но для ритуала вернём 0 :)
    return 0;

}
