#include <iostream>
#include <string>
#include <fstream>

#pragma warning(disable: 4996)
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>

#include <iomanip>
#include <chrono>

using namespace std;

void recv_file(SOCKET* sock)
{
    char file_size_str[16];
    char file_name[32];

    recv(*sock, file_size_str, 16, 0);
    int file_size = atoi(file_size_str);
    char* bytes = new char[file_size];

    recv(*sock, file_name, 32, 0);

    fstream file;

    //start time of measuring speed
    auto start = chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(false);

    file.open(file_name, ios_base::out | ios_base::binary);

    cout << "Size: " << file_size << endl;
    cout << "Name: " << file_name << endl;

    if (file.is_open())
    {
        recv(*sock, bytes, file_size, 0);

        file.write(bytes, file_size);

        cout << "Ok save" << endl;
    }
    else
        cout << "Error file open\n";

    //end time
    auto end = chrono::high_resolution_clock::now();
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;

    cout << "Speed: " << fixed << file_size / time_taken << setprecision(9);
    cout << " bytes per sec" << endl;

    delete[] bytes;
    file.close();
}

int main(int argc, char* argv[]) {
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cout << "Error" << endl;
        exit(1);
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1027);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

    if (newConnection == 0) {
        cout << "Error #2\n";
    }
    else {
        cout << "Client Connected!"<<endl;

        recv_file(&newConnection);
    }

    system("pause");
    return 0;
}