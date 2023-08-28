#include <iostream>
#include "../Tests/implementation.cpp"
#include "MyStore.h"

typedef std::vector<Client> ClientList;

struct MyActionHandler : ActionHandler {

	void onWorkerSend(int minute, ResourceType resource) override {
		std::cout << "W " << minute << ' ' << (resource == ResourceType::banana ? "banana" : "schweppes") << std::endl;
	}

	void onWorkerBack(int minute, ResourceType resource) override {
		std::cout << "D " << minute << ' ' << (resource == ResourceType::banana ? "banana" : "schweppes") << std::endl;
	}

	void onClientDepart(int index, int minute, int banana, int schweppes) override {
		std::cout << index << ' ' << minute << ' ' << banana << ' ' << schweppes << std::endl;
	}
};

int main()
{
	MyStore myStore;
	MyActionHandler store;

	myStore.setActionHandler(&store);

	std::cout << "input:\n";
	int workersCount, clientsCount;
	std::cin >> workersCount >> clientsCount;

	myStore.init(workersCount, 0, 0);

	ClientList clients;

	for (int i = 0; i < clientsCount; i++)
	{
		int index, banana, schweppes, waitTime;
		std::cin >> index >> banana >> schweppes >> waitTime;
		clients.push_back({ index, banana, schweppes, waitTime });
	}

	myStore.addClients(clients.data(), clientsCount);

	std::cout << "output:\n";
	try
	{
		myStore.advanceTo(INT_MAX);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
		return 1;
	}

	return 0;
}