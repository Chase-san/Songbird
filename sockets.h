/**
 *  Copyright (c) 2014, Robert Maupin <chasesan@gmail.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef __SONGBIRD_SOCKETS_H__
#define __SONGBIRD_SOCKETS_H__

#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#endif

#ifdef __cplusplus
/* Not sure why you would want to use this in C++, but just in case. */
extern "C" {
#define __songbird_header_inline__	inline
/* Works even if __STDC_VERSION__ is not defined. */
#elif __STDC_VERSION__ <= 199409L
#define __songbird_header_inline__	static __inline__
#else
#define __songbird_header_inline__	static inline
#endif

/* A generic simple sockets wrapper. Not perfect. */

enum {
	SB_SOCK_OK = 0,
	SB_SOCK_FAIL = 1,
	SB_SOCK_CLOSED = 0,
	SB_SOCK_ERROR = -1
};

/* Client Sockets */

typedef int sb_socket_t;

__songbird_header_inline__	int sb_sockets_start();
__songbird_header_inline__	void sb_sockets_stop();

__songbird_header_inline__	int sb_socket_open(sb_socket_t *, const char *, unsigned short);
__songbird_header_inline__	void sb_socket_close(sb_socket_t *);
__songbird_header_inline__	int sb_socket_write(sb_socket_t *, const char *, unsigned);
__songbird_header_inline__	int sb_socket_read(sb_socket_t *, char *, unsigned);
__songbird_header_inline__	int sb_socket_remote_address(sb_socket_t *, char *, unsigned, unsigned short *);
__songbird_header_inline__	int sb_socket_can_read(sb_socket_t *); /* can be used with server sockets too */

/* Server Sockets */

typedef sb_socket_t sb_ssocket_t;

__songbird_header_inline__	int sb_ssocket_open(sb_ssocket_t *, unsigned short, int queue);
/* use sb_socket_can_read to determine if there is a waiting connection to avoid blocking */
__songbird_header_inline__	int sb_ssocket_accept(sb_ssocket_t *, sb_socket_t *);
/* be sure to close all sockets opened with accept before calling this */
__songbird_header_inline__	void sb_ssocket_close(sb_ssocket_t *);


/* Function definitions. */

__songbird_header_inline__
int sb_sockets_start() {
#ifdef _WIN32
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		return SB_SOCK_ERROR;
	}
#endif
	return SB_SOCK_OK;
}

__songbird_header_inline__
void sb_sockets_stop() {
#ifdef _WIN32
	WSACleanup();
#endif
}

__songbird_header_inline__
int __sb_socket_init(sb_socket_t *sock) {
	*sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(*sock < 0) {
		return SB_SOCK_ERROR;
	}
	
	return SB_SOCK_OK;
}

__songbird_header_inline__
int sb_socket_open(sb_socket_t *sock, const char *ip, unsigned short port) {
	struct sockaddr_in addr;
	if(__sb_socket_init(sock) == SB_SOCK_ERROR) {
		return SB_SOCK_ERROR;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	if(connect(*sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		return SB_SOCK_ERROR;
	}
	return SB_SOCK_OK;
}

__songbird_header_inline__
void sb_socket_close(sb_socket_t *sock) {
#ifdef _WIN32
	shutdown(*sock, SD_BOTH);
	closesocket(*sock);
	/* WSACleanup(); */
#else
	shutdown(*sock, SHUT_RDWR);
	close(*sock);
#endif
	*sock = -1;
}

__songbird_header_inline__
int sb_socket_write(sb_socket_t *sock, const char *buf, unsigned len) {
#ifdef _WIN32
	int sent = send(*sock, buf, len, 0);
	if(sent == SOCKET_ERROR) {
		/* socket crash */
		sb_socket_close(sock);
		return SB_SOCK_ERROR;
	}
#else
	int sent = write(sock, buf, len);
#endif
	if(sent < 0) {
		return SB_SOCK_ERROR;
	}
	return sent;
}

__songbird_header_inline__
int sb_socket_can_read(sb_socket_t *sock) {
	int result;
	struct timeval tv;
	fd_set set;
	FD_ZERO(&set);
	FD_SET(*sock, &set);
	tv.tv_sec = 0;
	tv.tv_usec = 1000; /* 1 ms */
	result = select(*sock+1, &set, NULL, NULL, &tv);
	if(FD_ISSET(*sock , &set)) {
		return SB_SOCK_OK;
	}
	return SB_SOCK_FAIL;
}

__songbird_header_inline__
int sb_socket_read(sb_socket_t *sock, char *buf, unsigned len) {
#ifdef _WIN32
	int result = recv(*sock, buf, len, 0);
#else
	int result = read(*sock, buf, len);
#endif
	if(result < 0) {
		return SB_SOCK_ERROR;
	}
	return result;
}

__songbird_header_inline__
int sb_socket_remote_address(sb_socket_t *sock, char *buf, unsigned len, unsigned short *port) {
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);
	if(getpeername(*sock, (struct sockaddr *) &addr, &addr_len) < 0) {
		return SB_SOCK_ERROR;
	}
	snprintf(buf,len,"%s",inet_ntoa(addr.sin_addr));
	*port = ntohs(addr.sin_port);
	return SB_SOCK_OK;
}

__songbird_header_inline__
int sb_ssocket_open(sb_ssocket_t *ssock, unsigned short port, int queue) {
	struct sockaddr_in addr;
	if(__sb_socket_init(ssock) == SB_SOCK_ERROR) {
		return SB_SOCK_ERROR;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	if(bind(*ssock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		return SB_SOCK_ERROR;
	}
	if(listen(*ssock, queue) < 0) {
		return SB_SOCK_ERROR;
	}
	return SB_SOCK_OK;
}

__songbird_header_inline__
int sb_ssocket_accept(sb_ssocket_t *ssock, sb_socket_t *sock) {
	*sock = accept(*ssock, NULL, NULL);
	if(*sock < 0) {
		return SB_SOCK_ERROR;
	}
	return SB_SOCK_OK;
}

__songbird_header_inline__
void sb_ssocket_close(sb_ssocket_t *ssock) {
	sb_socket_close(ssock);
}

#undef __songbird_header_inline__

#ifdef __cplusplus
}
#endif

#endif /* __SONGBIRD_SOCKETS_H__ */
