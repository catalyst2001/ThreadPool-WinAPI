#include "netmsg.h"

#define ARRAY_PUSH(a, x)

bool netmsg_register(short netmsg_id, const char *p_name, short flags, short msg_size, read_netmsg_pfn readfn, write_netmsg_pfn writefn)
{
	struct netmsg_header_s *p_header = (struct netmsg_header_s *)calloc(1, sizeof(struct netmsg_header_s) + msg_size);
	assert(p_header);
	p_header->id = netmsg_id;
	p_header->p_name = p_name;
	p_header->flags = flags;
	p_header->msg_size = msg_size;
	p_header->read_netmsg = readfn;
	p_header->write_netmsg = writefn;
	ARRAY_PUSH(GLOBAL_NETMSGS_ARRAY, p_header); //TODO: !!!!!!!!!
	return true;
}
