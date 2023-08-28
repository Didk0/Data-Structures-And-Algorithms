#pragma once
#include "../Tests/interface.h"
#include "DoubleLinkedList.h"
#include <vector>
#include <climits>

struct MyStore : Store
{
	ActionHandler* actionHandler = nullptr;

	void setActionHandler(ActionHandler* handler) override
	{
		actionHandler = handler;
	}

	void init(int workerCount, int startBanana, int startSchweppes) override
	{
		workers = workerCount;

		banana = startBanana;
		schweppes = startSchweppes;

		expectedBanana = startBanana;
		expectedSchweppes = startSchweppes;
	}

	void addClients(const Client* clients, int count) override
	{
		for (int i = 0; i < count; i++)
		{
			clientsVec.push_back({ i, clients[i].arriveMinute, clients[i].banana, clients[i].schweppes, clients[i].maxWaitTime });
		}

		currTime = (clientsVec.empty() ? 0 : clientsVec.front().arriveMinute);
	}

	void advanceTo(int minute) override
	{
		if (!clientsVec.empty())
		{
			std::vector<MyClient>::iterator curr = clientsVec.begin();

			while (curr != clientsVec.end() && currTime <= minute)
			{
				// give client products or send workers
				if (curr->arriveMinute == currTime)
				{
					// enough resourses
					if (banana >= curr->banana && schweppes >= curr->schweppes)
					{
						expectedBanana -= curr->banana;
						expectedSchweppes -= curr->schweppes;
						banana -= curr->banana;
						schweppes -= curr->schweppes;
						actionHandler->onClientDepart(curr->index, currTime, curr->banana, curr->schweppes);
					}
					else
					{
						// sending workers if needed
						sendWorkers(curr);

						//put client in the waiting list
						if (curr->arriveMinute == currTime)
							waitingClientsList.push_back({ curr->index,curr->arriveMinute, curr->banana, curr->schweppes,curr->maxWaitTime });
					}
				}

				// remove workers from list at currTime
				removeWorkers();

				// remove waiting clients if there are enough resourses on delivery or if they waited max wait-time
				if (!waitingClientsList.empty())
					removeWaitingClients();

				// get the time of the next event
				currTime = getNextTime(curr);

				// increment curr to next client
				if ((curr + 1 != clientsVec.end() && currTime == (curr + 1)->arriveMinute)
					|| (curr + 1 == clientsVec.end() && waitingClientsList.empty() && workersList.empty()))
					++curr;
			}
		}
	}

	virtual int getBanana() const {
		return banana;
	}

	virtual int getSchweppes() const {
		return schweppes;
	}

private:
	struct MyWorker
	{
		int arriveMinutes;
		ResourceType resource;
	};

	struct MyClient
	{
		int index;
		int arriveMinute;
		int banana;
		int schweppes;
		int maxWaitTime;
	};

private:
	/// @brief Gets the time of the next event
	/// @param curr Iterator to the current client
	/// @return The wanted time
	int getNextTime(std::vector<MyClient>::iterator curr)
	{
		int minLeaveTime = MAX_TIME;
		Node<MyClient>* currWaitingClient = waitingClientsList.begin();

		while (currWaitingClient)
		{
			if ((currWaitingClient->person.arriveMinute + currWaitingClient->person.maxWaitTime) < minLeaveTime)
			{
				minLeaveTime = currWaitingClient->person.arriveMinute + currWaitingClient->person.maxWaitTime;
			}
			currWaitingClient = currWaitingClient->next;
		}

		int res = currTime;

		if (minLeaveTime != MAX_TIME)
			res = minLeaveTime;

		if (!workersList.empty())
			res = std::min(minLeaveTime, workersList.front().arriveMinutes);

		if (curr + 1 != clientsVec.end())
			res = std::min((curr + 1)->arriveMinute, res);

		if ((curr + 1) != clientsVec.end() && minLeaveTime == MAX_TIME && workersList.empty())
			res = (curr + 1)->arriveMinute;

		return res;
	}

	/// @brief Removes waiting clients from the waiting list if they can be served or their max wait time is over
	void removeWaitingClients()
	{
		Node<MyClient>* currWaitingClient = waitingClientsList.begin();

		while (currWaitingClient)
		{
			if ((banana >= currWaitingClient->person.banana && schweppes >= currWaitingClient->person.schweppes)
				|| (currTime == (currWaitingClient->person.arriveMinute + currWaitingClient->person.maxWaitTime)))
			{
				int bananaToGive = banana > currWaitingClient->person.banana ? currWaitingClient->person.banana : banana;
				int schweppesToGive = schweppes > currWaitingClient->person.schweppes ? currWaitingClient->person.schweppes : schweppes;

				expectedBanana -= bananaToGive;
				expectedSchweppes -= schweppesToGive;

				banana -= bananaToGive;
				schweppes -= schweppesToGive;

				actionHandler->onClientDepart(currWaitingClient->person.index, currTime, bananaToGive, schweppesToGive);
				currWaitingClient = waitingClientsList.erase(currWaitingClient);
			}
			else
			{
				currWaitingClient = currWaitingClient->next;
			}
		}
	}

	/// @brief Sends workers for the current client if needed
	/// @param curr Iterator to the current client
	void sendWorkers(std::vector<MyClient>::iterator curr)
	{
		int wantedBananas = curr->banana;
		int wantedSchweppes = curr->schweppes;

		while ((wantedBananas > 0 || wantedSchweppes > 0)
			&& workersList.size() < workers)
		{
			if (wantedSchweppes - schweppes > wantedBananas - banana)
			{
				if (expectedSchweppes < curr->schweppes)
				{
					workersList.push_back({ currTime + DELIVERY_TIME, ResourceType::schweppes });
					expectedSchweppes += DELIVERY_AMMOUNT;
				}
				wantedSchweppes = wantedSchweppes - DELIVERY_AMMOUNT < 0 ? 0 : wantedSchweppes - DELIVERY_AMMOUNT;
			}
			else
			{
				if (expectedBanana < curr->banana)
				{
					actionHandler->onWorkerSend(currTime, ResourceType::banana);
					workersList.push_back({ currTime + DELIVERY_TIME, ResourceType::banana });
					expectedBanana += DELIVERY_AMMOUNT;
				}
				wantedBananas = wantedBananas - DELIVERY_AMMOUNT < 0 ? 0 : wantedBananas - DELIVERY_AMMOUNT;
			}

			if (expectedBanana >= curr->banana && expectedSchweppes >= curr->schweppes)
				break;
		}

		//second while for printing in the correct order
		if (curr->index == (clientsVec.end() - 1)->index || curr->arriveMinute != (curr + 1)->arriveMinute)
		{
			Node<MyWorker>* currWorker = workersList.begin();
			while (currWorker)
			{
				if (currWorker->person.resource == ResourceType::schweppes
					&& currWorker->person.arriveMinutes - DELIVERY_TIME == currTime)
				{
					actionHandler->onWorkerSend(currTime, ResourceType::schweppes);
				}
				currWorker = currWorker->next;
			}
		}
	}

	/// @brief Removes workers from the worker list on delivery and increases the resource ammount
	void removeWorkers()
	{
		int counter = 0;
		int prevSize = workersList.size();
		while (!workersList.empty() && workersList.front().arriveMinutes == currTime)
		{
			if (workersList.front().resource == ResourceType::banana)
			{
				actionHandler->onWorkerBack(workersList.front().arriveMinutes, workersList.front().resource);
				banana += DELIVERY_AMMOUNT;
			}
			else
			{
				schweppes += DELIVERY_AMMOUNT;
				workersList.push_back(workersList.front());
			}
			workersList.pop_front();
			counter++;
			if (counter == prevSize) break;
		}

		// second while for printing in the correct order
		while (!workersList.empty() && workersList.back().arriveMinutes == currTime)
		{
			actionHandler->onWorkerBack(workersList.back().arriveMinutes, workersList.back().resource);
			workersList.pop_back();
		}
	}

private:
	std::vector<MyClient> clientsVec;
	DoubleLinkedList<MyClient> waitingClientsList;
	DoubleLinkedList<MyWorker> workersList;

	int banana = 0;
	int schweppes = 0;

	int expectedBanana = 0;
	int expectedSchweppes = 0;

	int currTime = 0;
	int workers = 0;

private:
	static const int MAX_TIME = INT_MAX;
	static const int DELIVERY_TIME = 60;
	static const int DELIVERY_AMMOUNT = 100;
};

