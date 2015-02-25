#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <time.h>

#include "../include/libi2c.h"

int buf[2];
int s, client;
sdp_session_t *session;

struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

socklen_t opt = sizeof(rem_addr);

sdp_session_t *register_service()
{
	int err = 0;
	uint32_t svc_uuid_int[] = { 0, 0, 0, 0 };
	uint8_t rfcomm_channel = 11;
	const char *service_name = "Drone Remote";
	const char *service_dsc = "Remote control service";
	const char *service_prov = "P0lyDr0n3";
	sdp_session_t *session = 0;

	uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;
	sdp_list_t *l2cap_list = 0,
		*rfcomm_list = 0,
		*root_list = 0, *proto_list = 0, *access_proto_list = 0;
	sdp_data_t *channel = 0;

	sdp_record_t *record = sdp_record_alloc();

	// set the general service ID
	sdp_uuid128_create(&svc_uuid, &svc_uuid_int);
	sdp_set_service_id(record, svc_uuid);

	// set the service class ID
	sdp_list_t service_class = { NULL, &svc_uuid };
	sdp_set_service_classes(record, &service_class);

	// make the service record publicly browsable
	sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root_list = sdp_list_append(0, &root_uuid);
	sdp_set_browse_groups(record, root_list);

	// set l2cap information
	sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
	l2cap_list = sdp_list_append(0, &l2cap_uuid);
	proto_list = sdp_list_append(0, l2cap_list);

	// set rfcomm information
	sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
	channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
	rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
	sdp_list_append(rfcomm_list, channel);
	sdp_list_append(proto_list, rfcomm_list);

	// attach protocol information to service record
	access_proto_list = sdp_list_append(0, proto_list);
	sdp_set_access_protos(record, access_proto_list);

	// set the name, provider, and description
	sdp_set_info_attr(record, service_name, service_prov, service_dsc);

	// connect to the local SDP server, register the service record, and 
	// disconnect
	session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
	/* if (session == NULL) perror("Unable to open SDK"); else */
	err = sdp_record_register(session, record, 0);

	// cleanup
	// sdp_data_free( channel );
	sdp_list_free(l2cap_list, 0);
	sdp_list_free(rfcomm_list, 0);
	sdp_list_free(root_list, 0);
	sdp_list_free(access_proto_list, 0);

	return session;
}

void list_bt_partner()
{
	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };

	dev_id = hci_get_route(NULL);
	sock = hci_open_dev(dev_id);

	if (dev_id < 0 || sock < 0)
	{
		perror("opening socket");
		exit(1);
	}

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info *) malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);

	if (num_rsp < 0)
		perror("hci_inquiry");

	for (i = 0; i < num_rsp; i++)
	{
		ba2str(&(ii + i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name
			(sock, &(ii + i)->bdaddr, sizeof(name), name, 0) < 0)
			strcpy(name, "[unknown]");
		printf("%s  %s\n", addr, name);
	}

	free(ii);
	close(sock);
}

void bt_server_register()
{
	// allocate socket
	session = register_service();
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// bind socket to port 1 of the first available 
	// local bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) 1;
	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
}

void bt_server_initiate()
{
	char buffer[1024] = { 0 };
	// put socket into listening mode
	listen(s, 1);

	// accept one connection
	client = accept(s, (struct sockaddr *)&rem_addr, &opt);

	ba2str(&rem_addr.rc_bdaddr, buffer);
	fprintf(stderr, "Accepted connection from %s\n", buffer);
	memset(buffer, 0, sizeof(buffer));
}

void end()
{
	close(s);
	sdp_close(session);
}

int main()
{
	clock_t start, stop;
	double start_time, stop_time;
	char data_char[6];
	int accx, accy, accz;
	int gyrox, gyroy, gyroz;
	int flag = 0;
	int i2c;

	int i = 0;

	i2c = setup_i2c();
	bt_server_register();

	while (1)
	{
		bt_server_initiate();

		// list_bt_partner();

		while (flag >= 0)
		{
			start = clock();

			buf[0] = 0x3b;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accx = buf[0] * 256;

			buf[0] = 0x3c;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accx += buf[0];

			buf[0] = 0x3d;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accy = buf[0] * 256;

			buf[0] = 0x3e;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accy += buf[0];

			buf[0] = 0x3f;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accz = buf[0] * 256;

			buf[0] = 0x40;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			accz += buf[0];

			buf[0] = 0x43;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyrox = buf[0] * 256;

			buf[0] = 0x44;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyrox += buf[0];

			buf[0] = 0x45;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroy = buf[0] * 256;

			buf[0] = 0x46;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroy += buf[0];

			buf[0] = 0x47;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroz = buf[0] * 256;

			buf[0] = 0x48;
			write(i2c, buf, 1);
			read(i2c, buf, 1);
			gyroz += buf[0];

			// printf("AccX = %f\t AccY = %f\t AccZ = %f\nGyroX = %f\t GyroY = 
			// 
			// 
			// 
			// %f\t GyroZ = %f\n\n",accx, accy, accz, gyrox, gyroy, gyroz);

			snprintf(data_char, sizeof(data_char), "%d", accx);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", accy);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", accz);
			write(client, data_char, 6);

			snprintf(data_char, sizeof(data_char), "%d", gyrox);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", gyroy);
			write(client, data_char, 6);
			snprintf(data_char, sizeof(data_char), "%d", gyroz);
			flag = write(client, data_char, 6);

			// prev = stop;
			stop = clock();

			start_time = (double)start / (CLOCKS_PER_SEC / 1000000);
			stop_time = (double)stop / (CLOCKS_PER_SEC / 1000000);
			// prev_time = (double) prev / (CLOCKS_PER_SEC / 1000000);

			usleep(((1 / I2C_FREQ) * 1000000) - (stop_time - start_time));
			i++;
			printf("Paquets: %d\r", i);
			// sample_frequency = 1/((stop_time/1000000)-(prev_time/1000000));

			// printf("Start: %f\tStop: %f\t Prev: %f\t Sampling frequency:
			// %f\n", start_time, stop_time, prev_time, sample_frequency);
		}

		i = 0;
		flag = 0;
		close(client);
		printf("\nDisconnected\n");
	}
	end();
	return 0;
}
