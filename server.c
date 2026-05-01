
//
//  basic-server.c
//  
//
//  Created by Pragati on 25/03/26.
//
// Simple TCP server
//#include "basic-server.h"
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
struct msg{
    int sender_id;
    int receiver_id;
    int type;
    char text[256];
};

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int client_sockets[128];//to store the socketfd of clinets
int client_ids[128];//to store the corresponding user_id of clinets
int client_count=0;//no of active clients

pthread_mutex_t lock;

void* mang_client(void*arg){
    int sockfd=*(int*)arg;

    struct msg m;
    while(1){
        ssize_t n=read(sockfd,&m,sizeof(m));

        if(n==0){
            printf("client disconnected\n");
            close(sockfd);
            return NULL;
        }
        if(n<0){
            error("Error in reading socket\n");
            close(sockfd);
            return NULL;
        }

        //registering clinet for the fd
        if(m.type==0){
            pthread_mutex_lock(&lock);//mutex lock so no other thread changes the socket detayila and theier user ids when one therad is making changes
            client_ids[client_count]=m.sender_id;
            client_sockets[client_count]=sockfd;
            client_count++;
            pthread_mutex_unlock(&lock);
            printf("client %d connected\n",m.sender_id);
        }else if(m.type==1){//sending private messages
        
            pthread_mutex_lock(&lock);//to ensure no other thred writes into it...
           
            for(int i=0;i<client_count;i++){
                if(client_ids[i]==m.receiver_id){
                   
                    write(client_sockets[i],&m,sizeof(m));
                    break;
                }
            }
            
            pthread_mutex_unlock(&lock);
        }else if(m.type==2){//broadcasting that is to everyone online
            pthread_mutex_lock(&lock);
            for(int i=0;i<client_count;i++){
                if(client_sockets[i]!=sockfd){
                    write(client_sockets[i],&m,sizeof(m));
                
                }
            }
            pthread_mutex_unlock(&lock);

        }else if(m.type==3){//removing the user from active users in the server
            pthread_mutex_lock(&lock);
        
            for(int i=0;i<client_count;i++){
                if(client_sockets[i]==sockfd){
                    client_sockets[i]=client_sockets[client_count-1];
                    client_ids[i]=client_ids[client_count-1];
                    client_count--;
                
                    printf("client %d disconnected\n",m.sender_id);
                    break;
                }
            }
            pthread_mutex_unlock(&lock);
            close(sockfd);
            return NULL;
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0)
       error("ERROR opening socket");
    
    // create a sockt structure to store the IP address and port no. of the server
    struct sockaddr_in serv_addr;
    // fill the block of memory with zeros
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // port no is passed in the command line arguments
    int portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);
    // htons() is used to convert an unsigned short integer (usually a port number) from the host byte order to the network byte order.
    // network byte order is always big-endian (most significant byte first)
    
    // bind the socket with the local IP address and Port no.
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
             error("ERROR on binding");
    //On success, bind() returns 0. On error, it returns -1
    
    // put the server socket on passive state, waiting client to connect
    listen(sockfd,5);
    
    // when client send a connection request, accept connection create a new socket for communication with client and return its file discriptor
    struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);
    pthread_mutex_init(&lock,NULL);
    while(1){//creating as many client maangers as possible
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
         error("ERROR on accept");
    pthread_t server_thread;
    int *clientfd=malloc(sizeof(int));
    *clientfd=newsockfd;
    pthread_create(&server_thread,NULL,mang_client,clientfd);
    pthread_detach(server_thread);

    }
    
    
   
    return 0;

}
