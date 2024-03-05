#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

void ErrorHandling(char *message);
void ShowSocketBufSize(SOCKET sock);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    SOCKET hsock;
    int sndBuf, rcvBuf, state;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hsock = socket(PF_INET, SOCK_STREAM, 0);

    ShowSocketBufSize(hsock);

    sndBuf = 1024 * 3, rcvBuf = 1024 * 3;

    state = setsockopt(hsock, SOL_SOCKET, SO_SNDBUF, (char *)&sndBuf, sizeof(sndBuf));
    if (state == SOCKET_ERROR)
        ErrorHandling("getsockopt() error!");

    state = setsockopt(hsock, SOL_SOCKET, SO_RCVBUF, (void *)&rcvBuf, sizeof(rcvBuf));
    if (state == SOCKET_ERROR)
        ErrorHandling("getsockopt() error!");

    ShowSocketBufSize(hsock);
    closesocket(hsock);
    WSACleanup();
    return 0;
}

void ShowSocketBufSize(SOCKET sock)
{
    int sndBuf, rcvBuf, state, len;

    len = sizeof(sndBuf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&sndBuf, &len);
    if (state == SOCKET_ERROR)
        ErrorHandling("getsockopt() error!");

    len = sizeof(rcvBuf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&rcvBuf, &len);
    if (state == SOCKET_ERROR)
        ErrorHandling("getsockopt() error!");

    printf("Input buffer size: %d \n", rcvBuf);
    printf("Output buffer size: %d \n", sndBuf);
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}