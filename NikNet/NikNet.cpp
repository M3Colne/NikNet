#include <iostream>
#include "NikNet.h"

using namespace std;

int main()
{
    NikNet::Server myServer(54000, true);
    
    while (myServer.GetErr() == "No error")
    {
        myServer.Running();
    }

    cin.get();
    return 0;
}