#pragma once

#include <string>
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

namespace NikNet
{
	class Client
	{
	private:
		SOCKET serverSocket = 0;
		std::string error = "No error";
	private:
		void Error()
		{
			error = std::to_string(WSAGetLastError());
		}
		void Error(const char* message)
		{
			error = message;
		}

		template<typename T>int nik_sendStruct(SOCKET s, T* buf, int len)
		{
			//Send each member variable individually
			//Don't forget to use htons, ntohs, htonl, ntohl and if you want to send floats you can use the my functions

			int bytesSend = 0;
			//Uncomment this
			//bytesSend = nik_send(s, buf.member_variable1,
			//sizeof(buf.member_variable1)); 
			//if(bytesSend <= 0){return bytesSend;}
			//bytesSend = nik_send(s, buf.member_variable2, 
			//sizeof(buf.member_variable2));
			//if(bytesSend <= 0){return bytesSend;}
			//bytesSend = nik_send(s, buf.member_variable2,
			//sizeof(buf.member_variable3));
			//if(bytesSend <= 0){return bytesSend;}
			//.
			//.
			//.
			return 0;
		}
		template<typename T>int nik_recvStruct(SOCKET s, T* buf, int len)
		{
			//Receive each member variable individually
			//Don't forget to use ntohs, ntohl and if you want to receive floats you can use the my functions

			int bytesReceive = 0;
			//Uncommen this
			//bytesRecv = nik_recv(s, buf.member_variable1,
			//sizeof(buf.member_variable1)); 
			//if(bytesRecv <= 0){return bytesRecv;}
			//bytesRecv = nik_recv(s, buf.member_variable2, 
			//sizeof(buf.member_variable2));
			//if(bytesRecv <= 0){return bytesRecv;}
			//bytesRecv = nik_recv(s, buf.member_variable2,
			//sizeof(buf.member_variable3));
			//if(bytesRecv <= 0){return bytesRecv;}
			//.
			//.
			//.
			return 0;
		}
		int nik_send(SOCKET s, char* buf, int len)
		{
			//The first 4 bytes is how much to tell how much we are sending
			{
				len = htons(len);
				const int bytesSent = send(s, reinterpret_cast<char*>(&len), sizeof(int), 0);
				if (bytesSent <= 0)
				{
					return bytesSent;
				}
				len = ntohs(len);
			}

			int total = 0;        // how many bytes we've sent
			int bytesleft = len; // how many we have left to send
			int n;

			while (total < len) {
				n = send(s, buf + total, bytesleft, 0);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return total;
		}
		int nik_recv(SOCKET s, char* buf, int len)
		{
			//Receive the number of how many bytes we need to receive
			int bytesToReceive = 0;
			{
				const int bytesReceived = recv(s, reinterpret_cast<char*>(&bytesToReceive), sizeof(int), 0);
				if (bytesReceived <= 0)
				{
					return bytesReceived;
				}
				bytesToReceive = ntohs(bytesToReceive);
			}

			int total = 0;				    // how many bytes we've received
			int bytesleft = bytesToReceive; // how many we have left to receive
			int n;

			while (total < bytesToReceive) {
				n = recv(s, buf + total, bytesleft, 0);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return total;
		}
	public:
		std::string GetErr() const
		{
			return error;
		}
		void Running()
		{
			//Traffic of data

			//README:
			//You must first send and then recv on the client sides, if you recv first the server will still wait for you send a message
			//but you can't because you got blocked from the recv (that's how your arhitecture will be *shrugs*)
			//If you want to send a struct you have to use the nik_sendStruct and nik_recvStruct but you have to do them yourself
			//Go to their declaration and send each member individually with nik_send
			//Basically use any private function you want to do networking
			//Don't forget to do error checking

			//Here is an example code:
			char msg[] = "Hello, I am the client";
			const int bytesSend = nik_send(serverSocket, msg, sizeof(msg));
			if (bytesSend <= 0)
			{
				error = "Couldn't send msg";
				return;
			}

			const int maxBufferSize = 100;
			char buffer[maxBufferSize] = {};
			const int bytesReceived = nik_recv(serverSocket, buffer, maxBufferSize);
			if (bytesReceived <= 0)
			{
				error = "Couldn't receive";
				return;
			}
			std::cout.write(buffer, 6);
		}

		Client(const char* ipAddress, unsigned int port)
		{
			WSADATA wsData;
			if (WSAStartup(MAKEWORD(2, 2), &wsData))
			{
				error = std::to_string(WSAGetLastError());
			}
			addrinfo* peerAddress = nullptr;
			addrinfo hint;
			ZeroMemory(&hint, sizeof(hint));
			hint.ai_family = AF_UNSPEC;
			hint.ai_socktype = SOCK_STREAM;
			if (getaddrinfo(ipAddress, std::to_string(port).c_str(), &hint, &peerAddress))
			{
				error = std::to_string(WSAGetLastError());
				return;
			}

			serverSocket = socket(peerAddress->ai_family, peerAddress->ai_socktype, peerAddress->ai_protocol);
			if (serverSocket == INVALID_SOCKET)
			{
				error = std::to_string(WSAGetLastError());
				return;
			}

			if (connect(serverSocket, peerAddress->ai_addr, peerAddress->ai_addrlen))
			{
				error = std::to_string(WSAGetLastError());
				return;
			}

			freeaddrinfo(peerAddress);

			OutputDebugStringA("Succesfully connected to the server!");
		}
		Client(const Client& other) = delete;
		Client operator=(const Client& other) = delete;
		Client(const Client&& other) = delete;
		Client& operator=(const Client&& other) = delete;
		~Client()
		{
			if (WSACleanup())
			{
				error = std::to_string(WSAGetLastError());
			}
		}
	};

	class Server
	{
	private:
		SOCKET serverSocket = 0;
		fd_set clientSet;
		std::vector<sockaddr> clientAddresses;;
		std::string error = "No error";
	private:
		void Error()
		{
			error = std::to_string(WSAGetLastError());
		}
		void Error(const char* message)
		{
			error = message;
		}

		int GetSockId(SOCKET s)
		{
			for (unsigned int i = 0; i < clientAddresses.size(); i++)
			{
				if (clientSet.fd_array[i] == s)
				{
					return i;
				}
			}
			return -1;
		}
		void DropClient(SOCKET s)
		{
			clientAddresses.erase(clientAddresses.cbegin() + GetSockId(s));
			FD_CLR(s, &clientSet);
		}
		template<typename T>int nik_sendStruct(SOCKET s, T* buf, int len)
		{
			//Send each member variable individually
			//Don't forget to use htons, ntohs, htonl, ntohl and if you want to send floats you can use the my functions

			int bytesSend = 0;
			//Uncomment this
			//bytesSend = nik_send(s, buf.member_variable1,
			//sizeof(buf.member_variable1)); 
			//if(bytesSend <= 0){return bytesSend;}
			//bytesSend = nik_send(s, buf.member_variable2, 
			//sizeof(buf.member_variable2));
			//if(bytesSend <= 0){return bytesSend;}
			//bytesSend = nik_send(s, buf.member_variable2,
			//sizeof(buf.member_variable3));
			//if(bytesSend <= 0){return bytesSend;}
			//.
			//.
			//.
			return 0;
		}
		template<typename T>int nik_recvStruct(SOCKET s, T* buf, int len)
		{
			//Receive each member variable individually
			//Don't forget to use ntohs, ntohl and if you want to receive floats you can use the my functions

			int bytesReceive = 0;
			//Uncommen this
			//bytesRecv = nik_recv(s, buf.member_variable1,
			//sizeof(buf.member_variable1)); 
			//if(bytesRecv <= 0){return bytesRecv;}
			//bytesRecv = nik_recv(s, buf.member_variable2, 
			//sizeof(buf.member_variable2));
			//if(bytesRecv <= 0){return bytesRecv;}
			//bytesRecv = nik_recv(s, buf.member_variable2,
			//sizeof(buf.member_variable3));
			//if(bytesRecv <= 0){return bytesRecv;}
			//.
			//.
			//.
			return 0;
		}
		int nik_send(SOCKET s, char* buf, int len)
		{
			//The first 4 bytes is how much to tell how much we are sending
			{
				len = htons(len);
				const int bytesSent = send(s, reinterpret_cast<char*>(&len), sizeof(int), 0);
				if (bytesSent <= 0)
				{
					return bytesSent;
				}
				len = ntohs(len);
			}

			int total = 0;        // how many bytes we've sent
			int bytesleft = len; // how many we have left to send
			int n;

			while (total < len) {
				n = send(s, buf + total, bytesleft, 0);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return total;
		}
		int nik_recv(SOCKET s, char* buf, int len)
		{
			//Receive the number of how many bytes we need to receive
			int bytesToReceive = 0;
			{
				const int bytesReceived = recv(s, reinterpret_cast<char*>(&bytesToReceive), sizeof(int), 0);
				if (bytesReceived <= 0)
				{
					return bytesReceived;
				}
				bytesToReceive = ntohs(bytesToReceive);
			}

			int total = 0;				    // how many bytes we've received
			int bytesleft = bytesToReceive; // how many we have left to receive
			int n;

			while (total < bytesToReceive) {
				n = recv(s, buf + total, bytesleft, 0);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return total;
		}
	public:
		std::string GetErr() const
		{
			return error;
		}
		void Running()
		{
			//Multiple client architecture
			fd_set copy = clientSet;
			const TIMEVAL timeVal = { 0,0 };
			const unsigned int socketCount = select(0, &copy, nullptr, nullptr, &timeVal);

			if (socketCount == SOCKET_ERROR)
			{
				Error();
				return;
			}

			//For each socket that we read from
			for (unsigned int i = 0; i < socketCount; i++)
			{
				const SOCKET sock = copy.fd_array[i];
				if (sock == serverSocket)
				{
					sockaddr clientAddress;
					ZeroMemory(&clientAddress, sizeof(clientAddress));
					int clientAddressSize = sizeof(clientAddress);

					SOCKET client = 0;
					client = accept(serverSocket, &clientAddress, &clientAddressSize);

					if (client == INVALID_SOCKET)
					{
						Error();
						return;
					}
					FD_SET(client, &clientSet);
					clientAddresses.push_back(clientAddress);

					//Writting a message that a client has connected
					char ip[INET_ADDRSTRLEN] = {};
					std::string msg = "A new client connected with the ip of ";
					OutputDebugStringA((msg + inet_ntop(AF_INET, &clientAddress, ip, INET_ADDRSTRLEN)).c_str());

					//OTHER BEHAVIOUR YOU WANT TO GIVE WHEN THE SERVER ACCEPTS A NEW CLIENT GOES HERE
					//Use only the functions nik_send and nik_recv to send and recv data
					//Don't forget about error checking
					//For example: Send to all the clients the message that someone connected
					//---
				}
				else
				{
					//Traffic of data

					//README:
					//If you want to send a struct you have to use the nik_sendStruct and nik_recvStruct but you have to do them yourself
					//Go to their declaration and send and receive each member individually with nik_send
					//Use any private function you want to do networking
					//Don't forget to do error checking

					//Here is an example code:
					char msg[] = "Hello, I am the server!";
					if (nik_send(sock, msg, sizeof(msg) + 1) <= 0) // + 1 because we need to send the null terminal as well
					{
						DropClient(sock);
						return;
					}

					const int maxBufferSize = 100;
					char buffer[maxBufferSize] = {};
					if (nik_recv(sock, buffer, maxBufferSize) <= 0)
					{
						DropClient(sock);
						return;
					}
					std::cout.write(buffer, 26);
				}
			}
		}
		int GetNClients() const
		{
			return clientAddresses.size() - 1; //- 1 because we also have both listeners in there, which aren't really clients
		}
		std::string GetClientAddress(int whichOne)
		{
			char ip[INET_ADDRSTRLEN] = {};
			inet_ntop(AF_INET, &clientAddresses.at(whichOne + 1), ip, INET_ADDRSTRLEN); // +1 because we want to skip the server
			return ip;
		}

		Server(const char* ipAddress, unsigned int port)
		{
			WSADATA wsData;
			if (WSAStartup(MAKEWORD(2, 2), &wsData))
			{
				Error();
				return;
			}
			addrinfo* address;
			addrinfo hint;
			ZeroMemory(&hint, sizeof(hint));
			hint.ai_family = AF_UNSPEC;
			hint.ai_socktype = SOCK_STREAM;
			if (getaddrinfo(ipAddress, std::to_string(port).c_str(), &hint, &address))
			{
				Error();
				return;
			}

			serverSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
			if (serverSocket == INVALID_SOCKET)
			{
				Error();
				return;
			}

			if (bind(serverSocket, address->ai_addr, address->ai_addrlen))
			{
				Error();
				return;
			}

			if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				Error();
				return;
			}

			FD_ZERO(&clientSet);
			FD_SET(serverSocket, &clientSet);
			clientAddresses.push_back(*address->ai_addr);
			freeaddrinfo(address);
		}
		Server(const Server& other) = delete;
		Server& operator=(const Server& other) = delete;
		Server(const Server&& other) = delete;
		Server& operator=(const Server&& other) = delete;
		~Server()
		{
			WSACleanup();
		}
	};
}