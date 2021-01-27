#include <iostream>
#include "NikNet.h"

using namespace std;

int main()
{
    NikNet::Server myServer("127.0.0.1", 54000);

    cout << myServer.GetErr();
    
    while (myServer.GetErr() == "No error")
    {
        myServer.Running();
        for (int i = 0; i < myServer.GetNClients(); i++)
        {
            std::string ip = myServer.GetClientAddress(i);
            std::string msg = "The client # " + std::to_string(i) + " has the ip:" + ip + "\n";
            OutputDebugStringA(msg.c_str());
        }
    }

    cout << myServer.GetErr();

    cin.get();
    return 0;
}