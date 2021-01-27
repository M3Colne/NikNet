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
        //cout << "Finished networking ";
    }

    cout << myServer.GetErr();

    cin.get();
    return 0;
}