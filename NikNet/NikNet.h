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
		int nik_send(SOCKET s, char* buf, int len)
		{
			//The first 4 bytes is how much to tell how much we are sending
			{
				const int bytesSent = send(s, reinterpret_cast<char*>(&len), sizeof(int), 0);
				if (bytesSent <= 0)
				{
					return bytesSent;
				}
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

			return len;
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

			return len;
		}
		int nik_sendto(SOCKET s, char* buf, int len, sockaddr* to, int tolen)
		{
			//The first 4 bytes is how much to tell how much we are sending
			{
				const int bytesSent = sendto(s, reinterpret_cast<char*>(&len), sizeof(int), 0, to, tolen);
				if (bytesSent <= 0)
				{
					return bytesSent;
				}
			}

			int total = 0;        // how many bytes we've sent
			int bytesleft = len; // how many we have left to send
			int n;

			while (total < len) {
				n = sendto(s, buf + total, bytesleft, 0, to, tolen);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return len;
		}
		int nik_recvfrom(SOCKET s, char* buf, int len, sockaddr* from, int* fromlen)
		{
			//Receive the number of how many bytes we need to receive
			int bytesToReceive = 0;
			{
				const int bytesReceived = recvfrom(s, reinterpret_cast<char*>(&bytesToReceive), sizeof(int), 0, from, fromlen);
				if (bytesReceived <= 0)
				{
					return bytesReceived;
				}
			}

			int total = 0;				    // how many bytes we've received
			int bytesleft = bytesToReceive; // how many we have left to receive
			int n;

			while (total < bytesToReceive) {
				n = recvfrom(s, buf + total, bytesleft, 0, from, fromlen);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return len;
		}
	public:
		std::string GetErr() const
		{
			return error;
		}
		void Running()
		{
			char testByte = 0;
			if (nik_send(serverSocket, &testByte, 1) <= 0)
			{
				error = "Couldn't send the test byte";
				return;
			}
			else
			{
				//Use only the functions nik_send and nik_recv to send and recv data
				//Don't forget about error checking
				//---
			}
		}

		Client(const char* ipAddress, unsigned int port, bool UDP0_TCPP1)
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
			hint.ai_socktype = UDP0_TCPP1 == true ? SOCK_STREAM : SOCK_DGRAM;
			hint.ai_flags = AI_PASSIVE;
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

			if (bind(serverSocket, peerAddress->ai_addr, peerAddress->ai_addrlen))
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
		TIMEVAL timeVal;
		std::vector<sockaddr> clientAddresses;
		std::string error = "No error";
	private:
		int nik_send(SOCKET s, char* buf, int len)
		{
			//The first 4 bytes is how much to tell how much we are sending
			{
				const int bytesSent = send(s, reinterpret_cast<char*>(&len), sizeof(int), 0);
				if (bytesSent <= 0)
				{
					return bytesSent;
				}
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

			return len;
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

			return len;
		}
		int nik_sendto(SOCKET s, char* buf, int len, sockaddr* to, int tolen)
		{
			//The first 4 bytes is how much to tell how much we are sending
			{
				const int bytesSent = sendto(s, reinterpret_cast<char*>(&len), sizeof(int), 0, to, tolen);
				if (bytesSent <= 0)
				{
					return bytesSent;
				}
			}

			int total = 0;        // how many bytes we've sent
			int bytesleft = len; // how many we have left to send
			int n;

			while (total < len) {
				n = sendto(s, buf + total, bytesleft, 0, to, tolen);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return len;
		}
		int nik_recvfrom(SOCKET s, char* buf, int len, sockaddr* from, int *fromlen)
		{
			//Receive the number of how many bytes we need to receive
			int bytesToReceive = 0;
			{
				const int bytesReceived = recvfrom(s, reinterpret_cast<char*>(&bytesToReceive), sizeof(int), 0, from, fromlen);
				if (bytesReceived <= 0)
				{
					return bytesReceived;
				}
			}

			int total = 0;				    // how many bytes we've received
			int bytesleft = bytesToReceive; // how many we have left to receive
			int n;

			while (total < bytesToReceive) {
				n = recvfrom(s, buf + total, bytesleft, 0, from, fromlen);
				if (n <= 0)
				{
					return n;
				}
				total += n;
				bytesleft -= n;
			}

			return len;
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
			const unsigned int socketCount = select(0, &copy, nullptr, nullptr, &timeVal);

			//For each socket that we read from
			for (unsigned int i = 0; i < socketCount; i++)
			{
				const SOCKET sock = copy.fd_array[i];
				if (sock == serverSocket)
				{
					sockaddr clientAddress;
					ZeroMemory(&clientAddress, sizeof(clientAddress));
					int clientAddressSize = 0;
					const SOCKET client = accept(serverSocket, &clientAddress, &clientAddressSize);

					if (client == INVALID_SOCKET)
					{
						error = "An error occured while accepting the client";
						return;
					}
					FD_SET(client, &clientSet);
					clientAddresses.push_back(clientAddress);

					//Writting a message that a client has connected
					char ip[INET_ADDRSTRLEN] = {};
					inet_ntop(AF_INET, &clientAddress, ip, INET_ADDRSTRLEN);
					std::string msg = "A new client connected with the ip of ";
					msg += ip;
					OutputDebugStringA(msg.c_str());

					//OTHER BEHAVIOUR YOU WANT TO GIVE WHEN THE SERVER ACCEPTS A NEW CLIENT GOES HERE
					//Use only the functions nik_send and nik_recv to send and recv data
					//Don't forget about error checking
					//For example: Send to all the clients the message that someone connected
					//---
				}
				else
				{
					char msg[6] = "Hello";
					const int bytesSend = nik_send(sock, msg, 6);
					if (bytesSend <= 0)
					{
						error = "Couldn't send msg";
						return;
					}

					char buffer[5] = {};
					const int bytesReceived = nik_recv(sock, buffer, 5);
					if (bytesReceived <= 0)
					{
						error = "Couldn't receive";
						return;
					}
					std::cout.write(buffer, 5);
				}
			}
		}
		int GetN_Client() const
		{
			return clientSet.fd_count;
		}
		std::string GetClientAddress(int whichOne)
		{
			char ip[INET_ADDRSTRLEN] = {};
			inet_ntop(AF_INET, &clientAddresses.at(whichOne), ip, INET_ADDRSTRLEN);
			return ip;
		}

		Server(unsigned int port, bool UDP0_TCP1)
		{
			//Initialize the timeVal
			ZeroMemory(&timeVal, sizeof(timeVal));
			timeVal.tv_usec = 500;

			WSADATA wsData;
			if (WSAStartup(MAKEWORD(2, 2), &wsData))
			{
				error = std::to_string(WSAGetLastError());
				return;
			}
			addrinfo* address;
			addrinfo hint;
			ZeroMemory(&hint, sizeof(hint));
			hint.ai_family = AF_UNSPEC;
			hint.ai_socktype = UDP0_TCP1 == true ? SOCK_STREAM : SOCK_DGRAM;
			hint.ai_flags = AI_PASSIVE;
			if (getaddrinfo(NULL, std::to_string(port).c_str(), &hint, &address))
			{
				error = std::to_string(WSAGetLastError());
				return;
			}

			serverSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
			if (serverSocket == INVALID_SOCKET)
			{
				error = std::to_string(WSAGetLastError());
				return;
			}

			if (UDP0_TCP1)
			{
				if (bind(serverSocket, address->ai_addr, address->ai_addrlen))
				{
					error = std::to_string(WSAGetLastError());
					return;
				}
			}

			//Set the serversocket to be non-blocking
			//setsockopt

			u_long myLong = 2048;
			ioctlsocket(serverSocket, 4, &myLong);

			freeaddrinfo(address);

			if (UDP0_TCP1)
			{
				if (listen(serverSocket, SOMAXCONN) == -1)
				{
					error = std::to_string(WSAGetLastError());
					return;
				}
			}

			FD_ZERO(&clientSet);
			FD_SET(serverSocket, &clientSet);
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