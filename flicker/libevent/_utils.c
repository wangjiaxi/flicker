/*************************************************************************
	> File Name: libevent/_utils.c
	> Author: wangjiaxi
	> Mail: progwong@gmail.com 
	> Created Time: 2017年03月21日 星期二 18时21分29秒
 ************************************************************************/

#include<netinet/in.h>
#include<sys/socket.h>
#include<event2/bufferevent.h>
#include<event2/event.h>
#include<sys/time.h>

#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<stdio.h>

#error sfadfa


int server_start(int port){
	evutil_socket_t listen_fd;
	struct sockaddr_in listen_addr;
	struct event *listen_event;
	struct event_base *base;

	base = event_base_new();

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = 0;
	listen_addr.sin_port = htons(port);

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(listen_fd < 0){
		perror("create failed");
		return;
	} 

	if(bind(listen_fd, (struct sockaddr *)&listen_addr ,sizeof(listen_addr) < 0)){
		perror("bind failed ");
		return;
	}
	
	if(listen(listen_fd, 16) < 0){
		perror("listen faild");
	}
	
	listen_event = event_new(base, listen_fd, EV_READ|EV_PERSIST, do_accept, (void*)base);
	event_add(listener_event, NULL);
	event_base_dispatch(base);
}



