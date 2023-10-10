//client
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERV_PORT 5576
char t[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
int flag;
int start=0;

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
    return -1; // Draw
}

void display_board() {
    // Display the current state of the Tic-Tac-Toe board
    printf("\n");
    for (int i = 0; i < 9; i++) {
        printf(" %c ", t[i]);
        if (i % 3 != 2) printf("|");
        if (i == 2 || i == 5) printf("\n---+---+---\n");
        if (i % 3 == 2 && i != 8) printf("\n");
    }
    printf("\n");
}

int validate_move(char move[]) {

    if (move[0] < '1' || move[0] > '9' || t[move[0]-49] == 'X' || t[move[0]-49] == 'O') {
        return 1; // Invalid move
    }
    return 0; // Valid move
}

void *sendi(void *arg) {
    int sockfd = *((int *)arg);
    char s[80];

    while (isdone()==1)
    {
	if((flag==0)&&(start))
	{
		do{
		printf("Enter position(1-9) : ");
        	scanf("%s", s);
		}while(validate_move(s));
        	write(sockfd, s, sizeof(s));
		flag=2;
	}
    } 

    pthread_exit(NULL); // Exit the thread when done
}

void *recvi(void *arg) {
    int sockfd = *((int *)arg);
 
    while ((isdone()==1))
    {
	if(!start)
	{
		char mes[10];
		read(sockfd, mes,sizeof(mes));
		start=1;
		printf("Game has started\n");
	}
        read(sockfd, t, sizeof(t));
        display_board();
	flag--;
    }
    close(sockfd);

    pthread_exit(NULL); // Exit the thread when done
}

int main() {
    int i, j, ans;
    ssize_t n;
    pthread_t send_id, recv_id;
    struct sockaddr_in servaddr;
    int sockfd;
    char f[10];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))==0)
    {
	printf("Connected to server\n");
	read(sockfd, f, sizeof(f));
    	flag=f[0];

    	pthread_create(&send_id, NULL, sendi, &sockfd);
    	pthread_create(&recv_id, NULL, recvi, &sockfd);

    	pthread_join(recv_id, NULL); 
    	pthread_cancel(send_id);     

    	if((isdone()==0)&&(flag==1))
	printf("Win\n");
    	if((isdone()==0)&&(flag==0))
	printf("Loss\n");
    	if(isdone()==-1)
	printf("Tie\n");
	close(sockfd);
        return 0;
    }
    printf("Server not running \n");
    close(sockfd);
    return 0;
}
