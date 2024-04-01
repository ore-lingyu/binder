#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <utils/Log.h>
#include <sys/epoll.h>
#define MAXFILE 65535 // 最大的文件描述符
char* result;
long lenght = 8192;
char *socket_path = "server-socket";
void executeCMD(const char *cmd) {
    ALOGE(" executeCMD  \n");
    char buf_ps[8192];
    char ps[8192] = { 0 };
    int i = 1;
    char *result2 = NULL;
    FILE *ptr = NULL;
    strcpy(ps, cmd);
    if ((ptr = popen(ps, "r")) != NULL) {
        result = (char *) malloc(lenght * sizeof(char));
        char *result2 = (char *) malloc(lenght * sizeof(char));

        while (fgets(buf_ps, 8192, ptr) != NULL) {
            result = (char *) malloc(lenght * i * sizeof(char));
            if (result2 != NULL)
                strcpy(result, result2);
            strcat(result, buf_ps);
            i++;
            result2 = (char *) malloc(lenght * (i - 1) * sizeof(char));
            strcpy(result2, result);
    	    ALOGE(" executeCMD  result = %s\n",result);
        }
        pclose(ptr);
        ptr = NULL;
    } else {
        printf("popen %s error\n", ps);
    }

}

int main() {

    printf("main rootServer running \n");
     struct sockaddr_un serun, cliun;
       socklen_t cliun_len;
       int listenfd, connfd, size;
       char buf[8192];
       int i, n;

       if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
           perror("socket error");
           exit(1);
       }

       memset(&serun, 0, sizeof(serun));
       serun.sun_family = AF_UNIX;
       serun.sun_path[0] = 0;
       strcpy(serun.sun_path+1,socket_path);
       socklen_t addrlen_ = sizeof(serun.sun_family) + strlen(socket_path) + 1;
	//unlink(socket_path);
       if (bind(listenfd, (struct sockaddr *)&serun, addrlen_) < 0) {
           perror("bind error");
           exit(1);
       }

       if (listen(listenfd, 20) < 0) {
           perror("listen error");
           exit(1);
       }


    while (1) // 守护进程实现的服务
    {
        ALOGE("wait connect...\n");
         printf("wait connect...\n");
        socklen_t l =   sizeof(struct sockaddr_un);
        if ((connfd = accept(listenfd,(struct sockaddr *) &cliun,  &l)) < 0) {
            perror("accept");
            //return 1;
        }
        int len =0;
        if ((len = recv(connfd, buf, 8192, 0)) > 0) {
            ALOGE("connect  recv\n");
            buf[len] = '\0';
            printf("%s\n", buf);
            executeCMD(buf);
            if (strlen(result) == 0) {
                strcpy(result, "Returing is null!");
            }
            if (send(connfd, result, strlen(result), 0) < 0) {
                perror("write");
                //return 1;
            }
        }
        close(connfd);
    }

    close(listenfd);
    return 0;
}
