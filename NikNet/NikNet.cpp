#include <iostream>
#include "NikNet.h"

using namespace std;

int main()
{
    NikNet::Server myServer("192.168.0.17", 54000, true);

    cout << myServer.GetErr();
    
    while (myServer.GetErr() == "No error")
    {
        myServer.Running();
    }

    cout << myServer.GetErr();

    cin.get();
    return 0;
}