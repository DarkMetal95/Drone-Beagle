#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "../include/libbluetooth.h"

int main()
{
	int s, client;
	char key[1];
	char data[4];
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

	session = bt_register_service();
	s = bt_server_register(&loc_addr);

	int shut = 1;

	key[0] = 'a';
	while(shut)
	{
		printf("Waiting for connection\n");
		client = bt_server_initiate(s, &rem_addr);
		printf("Connected! \n");

		while(shut >= 0)
		{
			read(client,key,sizeof(key));
			if(key[0] == 'U')
			{
				read(client, data, sizeof(data));
				printf("readed: data1: %d data2: %d data3: %d data4: %d \n", data[0], data[1], data[2], data[3]);
			}
		}

		close(client);
	}

	bt_end_session(s, session);

	return 0;
}

