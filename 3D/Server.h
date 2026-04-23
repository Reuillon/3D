#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include <String>

class Server
{
	private:
		unsigned short port = 54000;
		std::array<float, sizeof(float) * 4> clientData = { 0 };
		int clientSize = clientData.size() * sizeof(float);
		int serverSize = Data.size() * sizeof(float);
		sf::UdpSocket client;
		sf::IpAddress recipient;
		std::optional<sf::IpAddress> sender;
		std::size_t received;

	public:
		
		std::array<float, sizeof(float) * 4> Data = { 0 };
		Server() : recipient(127, 0, 0, 1)
		{
			client.setBlocking(0);
			//UNUSED BYTES
			clientData[0] = -FLT_MAX;
			clientData[1] = -FLT_MAX;
		};
		
		std::string getGameState()
		{
			if (client.send(clientData.data(), clientSize, recipient, port) != sf::Socket::Status::Done)
			{

			}
			if (client.receive(Data.data(), serverSize, received, sender, port) != sf::Socket::Status::Done)
			{

			}

			return "Enemy Player Position: " + std::to_string(Data[0]) + " " + std::to_string(Data[1]);
		}
};

