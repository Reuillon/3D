#pragma once
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <SFML/Network.hpp>
#include <iostream>
#include <String>

class Server
{
	private:
		unsigned short port = 54000;
		sf::UdpSocket client;
		sf::IpAddress recipient;
		std::optional<sf::IpAddress> sender;
		std::size_t received;

	public:
		std::array<float, 16> clientData = { 0 };
		std::array<float, 16> Data = { 0 };
		int clientSize = clientData.size() * sizeof(float);
		int serverSize = Data.size() * sizeof(float);

		Server() : recipient(99, 246, 161, 173)
		{
			client.setBlocking(0);
			//UNUSED BYTES
			//clientData[4] = FLT_MAX;
			//clientData[5] = FLT_MAX;
			//clientData[6] = FLT_MAX;
			//clientData[7] = FLT_MAX;
			
		};
		
		void getGameState()
		{
			if (client.send(clientData.data(), clientSize, recipient, port) != sf::Socket::Status::Done)
			{
				//std::cout << "NO DATA SENT\n";
			}
			if (client.receive(Data.data(), serverSize, received, sender, port) != sf::Socket::Status::Done)
			{
				//std::cout << "NO DATA RECIEVED\n";
			}

		}

};

