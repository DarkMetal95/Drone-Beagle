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
	int x_angle_cons = 0;
	int y_angle_cons = 0;
	int z_height_cons = 0;

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

				if(data[1] <= 9)
					x_angle_cons = -((data[0]*4)*data[1])/9;
				else if(data[1] > 9  && data[1] <= 18)
					x_angle_cons =  ((data[0]*4)*(data[1]-18))/9;
				else if(data[1] > 18 && data[1] <= 27)
					x_angle_cons =  ((data[0]*4)*(data[1]-18))/9;
				else if(data[1] > 27 && data[1] <= 35)
					x_angle_cons = -((data[0]*4)*(data[1]-35))/9;

				if(data[1] <= 9)
					y_angle_cons = -((data[0]*4)*(data[1]-9 ))/9;
				else if(data[1] > 9  && data[1] <= 18)
					y_angle_cons = -((data[0]*4)*(data[1]-9 ))/9;
				else if(data[1] > 18 && data[1] <= 27)
					y_angle_cons =  ((data[0]*4)*(data[1]-27))/9;
				else if(data[1] > 27 && data[1] <= 35)
					y_angle_cons =  ((data[0]*4)*(data[1]-27))/9;

				if(data[3] <= 9)
					z_height_cons = -((data[2]*4)*(data[3]-9 ))/9;
				else if(data[3] > 9  && data[3] <= 18)
					z_height_cons = -((data[2]*4)*(data[3]-9 ))/9;
				else if(data[3] > 18 && data[3] <= 27)
					z_height_cons =  ((data[2]*4)*(data[3]-27))/9;
				else if(data[3] > 27 && data[3] <= 35)
					z_height_cons =  ((data[2]*4)*(data[3]-27))/9;


				printf("Consigne x: %d Consigne y: %d Consigne z: %d		-----------------> y\n", x_angle_cons, y_angle_cons, z_height_cons);
			}
		}

		close(client);
	}

	bt_end_session(s, session);

	return 0;
}

