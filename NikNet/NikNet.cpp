#include <iostream>
#include "NikNet.h"

using namespace std;

int main()
{
    NikNet::Server myServer("127.0.0.1", 54000, true);

    cout << myServer.GetErr();
    
    while (myServer.GetErr() == "No error")
    {
        myServer.Running();
    }

    cout << myServer.GetErr();

    cin.get();
    return 0;
}