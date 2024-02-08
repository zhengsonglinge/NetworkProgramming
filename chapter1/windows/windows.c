#include <winsock2.h>

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    //...
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");
    //...

    WSACleanup();
    return 0;
}
