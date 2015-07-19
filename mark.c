/** Wrapper utility to mark all sockets of a program
 *
 * Compile:
 *  gcc -fPIC -c -o mark.o mark.c
 *  gcc -shared -o mark.so mark.o -ldl
 *
 * Use:
 *  LD_PRELOAD="./soft_atimes.so" command
 *
 * @author Steffen Vogel <post@steffenvogel.de>
 * @authir D.J. Capelis
 * @copyright 2014-2015, Steffen Vogel
 * @copyright 2007, Regents of the University of California
 * @license GPLv3
 *********************************************************************************/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <sys/socket.h>

typedef int (*socket_t)(int domain, int type, int protocol);

static socket_t _socket;

int socket(int domain, int type, int protocol)
{	
	if (!_socket)
		_socket = (socket_t) dlsym(RTLD_NEXT, "socket");

	int sd = _socket(domain, type, protocol);

	if (sd >= 0) {
		if (domain == AF_INET || domain == AF_INET6) {
			char * env = getenv("MARK");
			int mark = env ? atoi(env) : 0xCD;
			
			printf("Setting SO_MARK for fd=%u to %#x\n", sd, mark);
		
			setsockopt(sd, SOL_SOCKET, SO_MARK, &mark, sizeof(mark));
		}
	}
	
	return sd;
}