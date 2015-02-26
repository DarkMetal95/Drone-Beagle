#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/libbluetooth.h"

/* 
 * Initiates session
 */
sdp_session_t *bt_register_service()
{
	uint32_t svc_uuid_int[] = { 0, 0, 0, 0 };
	uint8_t rfcomm_channel = BT_RFCOMM_CHAN;
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
	sdp_set_info_attr(record, BT_SERVICE_NAME, BT_SERVICE_PROV,
					  BT_SERVICE_DESC);

	// connect to the local SDP server, register the service record, and 
	// disconnect
	session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
	/* if (session == NULL) perror("Unable to open SDK"); else */
	sdp_record_register(session, record, 0);

	// cleanup
	// sdp_data_free( channel );
	sdp_list_free(l2cap_list, 0);
	sdp_list_free(rfcomm_list, 0);
	sdp_list_free(root_list, 0);
	sdp_list_free(access_proto_list, 0);

	return session;
}

/* 
 * Returns socket
 */
int bt_server_register(sockaddr_rc * loc_addr)
{
	// allocate socket
	int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// bind socket to port 1 of the first available 
	// local bluetooth adapter
	loc_addr->rc_family = AF_BLUETOOTH;
	loc_addr->rc_bdaddr = *BDADDR_ANY;
	loc_addr->rc_channel = (uint8_t) 1;
	bind(s, (const sockaddr_rc *)loc_addr, sizeof(*loc_addr));

	return s;
}

/* 
 * Binds the socket given as parameter and accepts one connection
 */
int bt_server_initiate(int socket, sockaddr_rc * rem_addr)
{
	char buffer[1024] = { 0 };
	socklen_t opt = sizeof(*rem_addr);

	// put socket into listening mode
	listen(socket, 1);

	// accept one connection
	int client = accept(socket, (struct sockaddr *)&rem_addr, &opt);

	ba2str(&rem_addr->rc_bdaddr, buffer);
	fprintf(stderr, "Accepted connection from %s\n", buffer);
	memset(buffer, 0, sizeof(buffer));

	return client;
}

/* 
 * Closes socket and session
 */
void bt_end_session(int socket, sdp_session_t * session)
{
	close(socket);
	sdp_close(session);
}
