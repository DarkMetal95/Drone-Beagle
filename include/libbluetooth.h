extern int s, client;
extern sdp_session_t *session;
extern struct sockaddr_rc loc_addr, rem_addr;
extern socklen_t opt;

sdp_session_t *register_service();
void bt_server_register();
void bt_server_initiate();
void end();