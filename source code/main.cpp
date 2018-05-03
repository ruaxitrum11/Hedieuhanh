
#include <stdio.h>
#include <winsock2.h>

DWORD WINAPI ClientThread(LPVOID);

SOCKET clients[64];
int numClients;

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8888);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	while (true)
	{
		printf("Waiting for client...\n");
		SOCKET client = accept(listener, NULL, NULL);
		printf("Client accepted: %d\n", client);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}

	closesocket(listener);
	WSACleanup();
    return 0;
}

void RemoveClient(SOCKET client)
{
	int i = 0;
	for (; i < numClients; i++)
		if (clients[i] == client)
			break;
	if (i < numClients - 1)
		clients[i] = clients[numClients - 1];
	numClients--;
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET *)lpParam;
	char buf[256], sendbuf[256];
	int res;

	char cmd[16], id[64], tmp[64];

	while (true)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == SOCKET_ERROR || res == 0)
		{
			closesocket(client);
			return 1;
		}

		// Xu ly buffer
		buf[res] = 0;
		res = sscanf(buf, "%s %s %s", cmd, id, tmp);
		if (res != 2)
		{
			char * msg = "Wrong format. Please send again.\n";
			send(client, msg, strlen(msg), 0);
		}
		else
		{
			if (strcmp(cmd, "client_id:") != 0)
			{
				char * msg = "Wrong format. Please send again.\n";
				send(client, msg, strlen(msg), 0);
			}
			else
			{
				// Correct format
				char * msg = "OK. You can send message now.\n";
				send(client, msg, strlen(msg), 0);

				clients[numClients] = client;
				numClients++;

				break;
			}
		}
	}
	char msg[256];
	sprintf(msg, "%s connected\n", id);
	for (int i = 0; i < numClients; i++)
		if (client != clients[i]) {
			send(clients[i], msg, strlen(msg), 0);
		}
		
	
																													
	while (true)
	{
		res = recv(client, buf, sizeof(buf), 0);
		if (res == SOCKET_ERROR || res == 0)
		{

			RemoveClient(client);
			printf("Client %s exit\n", id);
			
			char msgD[256];
			sprintf(msgD,"%s disconnected\n", id);
			for (int i = 0; i < numClients; i++){
			send(clients[i], msgD, strlen(msgD), 0);
		}
			
		
			closesocket(client);
			return 1;
		}

		// Xu ly buffer
		buf[res] = 0;

		sprintf(sendbuf, "%s: %s", id, buf);
		
		for (int i = 0; i < numClients; i++)
			if (client != clients[i]) {
				send(clients[i], sendbuf, strlen(sendbuf), 0);
			}
	}
	
	return 0;
}
