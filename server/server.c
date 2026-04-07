#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "server_logic.h"

int main() {
	AppContext ctx;

	if(init_server_application(&ctx) != 0) {
		return 1;
	}

	wait_for_players(&ctx);

	while(ctx.status != SERVER_SHUTDOWN) {
		manage_connections(&ctx);	// handles clients connecting/disconnecting
		if (ctx.status == SERVER_WAITING) {
			wait_for_players(&ctx);
			update_new_players(&ctx);
		}
		process_client_packets(&ctx);
		update_server_state(&ctx);
		broadcast_updates(&ctx);
	}

	cleanup_server(&ctx);
}


