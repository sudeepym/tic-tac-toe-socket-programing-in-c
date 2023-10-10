//server
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERV_PORT 5576

char t[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
int k = 19;
int id[2];
int flag=0;

int isdone() {
    // Check if the game is over (win or draw)
    for (int i = 0; i < 3; i++) {
        if (t[i] == t[i + 3] && t[i + 3] == t[i + 6]) return 0; // Vertical win
        if (t[i * 3] == t[i * 3 + 1] && t[i * 3 + 1] == t[i * 3 + 2]) return 0; // Horizontal win
    }
    if (t[0] == t[4] && t[4] == t[8]) return 0; // Diagonal win
    if (t[2] == t[4] && t[4] == t[6]) return 0; // Diagonal win
    for (int i = 0; i < 9; i++) {
        if (t[i] != 'X' && t[i] != 'O') return 1; // Game still ongoing
    }
    return 0; // Draw
}


void *handle_client(void *arg) {
    int connfd = *((int *)arg);
    int i;
    char s[80],f[10];

    f[0]=flag;
    write(connfd, f, sizeof(f));
    flag++;

   if(flag==2)
   {
	char mes[]="Start";
	for (i = 0; i < 2; i++)
            write(id[i], mes, sizeof(mes));
   }
    while (isdone())
    {
        if(read(connfd, s, sizeof(s))==0)
	{
		close(connfd);
    		return NULL;
	}
        int index = s[0] - '0';
        if (k % 2)
            t[index - 1] = 'X';
        else
            t[index - 1] = 'O';
        k--;
        for (i = 0; i < 2; i++)
            write(id[i], t, sizeof(t));
    }

    close(connfd);
    return NULL;
}

int main() {
    int i;
    pthread_t tid[2];
    struct sockaddr_in servaddr, cliaddr;
    int listenfd, connfd;
    socklen_t clilen;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 2);

    clilen = sizeof(cliaddr);

    printf("Waiting for players\n");

    for (i = 0; i < 2; i++) {
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("Client %d connected\n",i+1);
	id[i]=connfd;
	
        if (pthread_create(&tid[i], NULL, handle_client, &connfd) != 0) {
            perror("pthread_create");
            close(connfd);
        }
    }

    for (i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }
    
    printf("Game over\n");

    close(listenfd);
    return 0;
}