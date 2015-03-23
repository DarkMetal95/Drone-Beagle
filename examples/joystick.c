#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "../include/libbluetooth.h"

int main()
{
	int s, client;
	char key[1024] = { 0x00 };
	char data[4];
	sdp_session_t *session = NULL;
	sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

	session = bt_register_service();
	s = bt_server_register(&loc_addr);

	int shut = 1;

	while(shut)
	{
		printf("Waiting for connection\n");
		client = bt_server_initiate(s, &rem_addr);
		printf("Connected! \n");

		while(shut)
		{
			read(client, key, 1);
			
			if(key[0] == 'U')
			{
				read(client, data, 4);
				printf("%s\n",data );
			}
			else
				shut = 0;

		}

		close(client);
	}

	bt_end_session(s, session);

	return 0;
}

