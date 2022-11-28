#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#include <string>
#include <fstream>
#include <experimental/filesystem>

#pragma warning(disable: 4996)
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>

#include <iomanip>
#include <chrono>

using namespace std;

void send_file(SOCKET* sock, const string& file_name)
{
    fstream file;

    //start time of measuring speed
    auto start = chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(false);

    file.open(file_name, ios_base::in | ios_base::binary);

    if (file.is_open())
    {
        int file_size = experimental::filesystem::file_size(file_name) + 1;

        char* bytes = new char[file_size];

        file.read(bytes, file_size);

        cout << "Size: " << file_size << endl;
        cout << "Name: " << file_name << endl;

        send(*sock, to_string(file_size).c_str(), 16, 0);
        send(*sock, file_name.c_str(), 32, 0);
        send(*sock, bytes, file_size, 0);

        //end time
        auto end = chrono::high_resolution_clock::now();
        double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        time_taken *= 1e-9;
        
        cout << "Speed: " << fixed << file_size/time_taken << setprecision(9);
        cout << " bytes per sec" << endl;

        delete[] bytes;
    }
    else
        cout << "Error file open\n";

    file.close();
}

int main(int argc, char* argv[]) {

    auto start = chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(false);

    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);

    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        cout << "Error" << endl;
        exit(1);
    }

    SOCKADDR_IN addr;
    memset(&addr, 0, sizeof(SOCKADDR_IN));

    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1027);
    addr.sin_family = AF_INET;

    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);

    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) == 0)
    {
        cout << "Connected!" << endl;
        string path;
        string who;

        cout << "Do you want to open information about the project?" << endl;
        cin >> who;
        if (who == "Who")
        {
            cout << "Kseniya Gladkih Variant 2 File Transferring" << endl;
            cout << "Enter name of file to open" << endl;
        }
        else cout << "Enter name of file to open" << endl;

        cin >> path;
        send_file(&Connection, path);
    }
    else
        cout << "Error: failed connect to server.\n";

    auto end = chrono::high_resolution_clock::now();
    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    time_taken *= 1e-9;
    
    cout << "Execution: " << fixed << time_taken << setprecision(5) << " sec " << endl;

    system("pause");

    return 0;
}