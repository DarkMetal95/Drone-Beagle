#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

/* 
 * Defines
 */

#define BT_SERVICE_NAME			"Drone Remote"
#define BT_SERVICE_DESC			"Remote control service"
#define BT_SERVICE_PROV			"P0lyDr0n3"

#define BT_RFCOMM_CHAN			1

/* 
 * Structures
 */

typedef struct sockaddr_rc sockaddr_rc;

/* 
 * Prototypes
 */

sdp_session_t *bt_register_service();
int bt_server_register(sockaddr_rc * loc_addr);
int bt_server_initiate(int socket, sockaddr_rc * rem_addr);
void bt_end_session(int socket, sdp_session_t * session);

#endif
