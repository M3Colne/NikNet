#include <iostream>
#include "NikNet.h"

using namespace std;

int main()
{
    NikNet::Server myServer(54000, false);

    cout << myServer.GetErr();
    
    while (myServer.GetErr() == "No error")
    {
        myServer.Running();
    }

    cout << myServer.GetErr();

    cin.get();
    return 0;
}