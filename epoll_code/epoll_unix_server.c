#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/epoll.h>
 
#define MAXLINE 80

char *socket_path = "server-socket";

int main()
{
    struct sockaddr_un serun, cliun;
    socklen_t cliun_len;
    int listenfd, connfd, size;
    char buf[MAXLINE];
    int i, n;

    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&serun, 0, sizeof(serun));
    serun.sun_family = AF_UNIX;
    strncpy(serun.sun_path,socket_path ,

	
    int ret = bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }
    // 3. 监听
    ret = listen(listenfd, 20);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }
    
    // 4. 创建epoll树
    int epfd = epoll_create(1000);//1000并没有什么意义
    if(epfd == -1)
    {
        perror("epoll_create");
        exit(-1);
    }
    //5、将用于监听的lfd挂的epoll树上（红黑树）
    struct epoll_event ev;//这个结构体记录了检测什么文件描述符的什么事件
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);//ev里面记录了检测lfd的什么事件
    // 循环检测 委托内核去处理
    struct epoll_event events[1024];//当内核检测到事件到来时会将事件写到这个结构体数组里
    while(1)
    {
        int num = epoll_wait(epfd, events, sizeof(events)/sizeof(events[0]), -1);//最后一个参数表示阻塞
        //遍历事件，并进行处理
        for(int i=0; i<num; i++)
        {
            if(events[i].data.fd == listenfd)//有连接请求到来
            {
                struct sockaddr_in client_sock;
                int len = sizeof(client_sock);
                int connfd = accept(listenfd, (struct sockaddr *)&client_sock, &len);
                if(connfd == -1)
                {
                    perror("accept");
                    exit(-1);
                }
                printf("a new client connected! ");
                //将用于通信的文件描述符挂到epoll树上
                ev.data.fd = connfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
            }
            else//通信
            {
                //通信也有可能是写事件
                if(events[i].events & EPOLLOUT)
                {
                    //这里先忽略写事件
                    continue;
                }
                char buf[1024]={0};
                int count = read(events[i].data.fd, buf, sizeof(buf));
                if(count == 0)//客户端关闭了连接
                {
                    printf("客户端关闭了连接。。。。\n");
                    //将对应的文件描述符从epoll树上取下
                    close(events->data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, events->data.fd, NULL);
                }
                else
                {
                    if(count == -1)
                    {
                        perror("read");
                        exit(-1);
                    }
                    else
                    {
                        //正常通信
                        printf("client say: %s\n" buf);
                        write(events[i].data.fd, buf, strlen(buf)+1);
                    }
                }
            }
        }
    }
    close(listenfd);
    return 0;
}
