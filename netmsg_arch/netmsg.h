#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef bool(*read_netmsg_pfn)(void *p_thismsg, void *p_buffer, size_t size);
typedef bool(*write_netmsg_pfn)(void *p_thismsg, void *p_buffer, size_t size);

// 
// General network message header
// 
struct netmsg_header_s {
	short msg_size;
	short flags;
	const char *p_name;
	read_netmsg_pfn read_netmsg;
	write_netmsg_pfn write_netmsg;
};

// 
// EXAMPLE NETMSG
// 
typedef struct test_netmsg_s {
	int item1;
	int item2;
} test_netmsg_t;
//NETMSG SIZE = sizeof(netmsg_header_s) + sizeof(test_netmsg_t);

//USE xxHash

bool                    netmsg_init(short msgs_reserved_number, short active_msgs_number);
bool                    netmsg_register(short netmsg_id, const char *p_name, short flags, short msg_size, read_netmsg_pfn readfn, write_netmsg_pfn writefn);
bool                    netmsg_unregister(short netmsg_id);
read_netmsg_pfn         netmsg_read_hook(read_netmsg_pfn p_readhook_fn);
write_netmsg_pfn        netmsg_write_hook(write_netmsg_pfn p_readhook_fn);
struct netmsg_header_s *netmsg_get_header(short netmsg_id);
bool                    netmsg_shutdown();