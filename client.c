//
//  basic-client.c
//  
//
//  Created by Pragati on 25/03/26.
//


//TCP client
#include <stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<pthread.h>
#include<string.h>

//data structure used... easy to understand the purpose
struct msg{
    int sender_id;
    int receiver_id;
    int type;//0 to register the client in server, 1 for private, 2 for private ,3 for discoonecting from server
    char text[256];
};

void error(char *msg)
{
    perror(msg);
    exit(0);
}


// Thread for receiving messages
void* rec_msg(void *arg){
    int sockfd=*(int*)arg;
    struct msg m;
    while(1){
        ssize_t c=read(sockfd,&m,sizeof(m));
        if(c==0){
            printf("\t\t\tserver disconnected\n");
            break;
        }
    if(c<0)error("\t\t\tError in reading msg");
    
    printf("\t\t\tFrom %d : %s\n",m.sender_id,m.text);//clent gets the sender id and message
    }   
    return NULL;
    
    
}
int main(int argc, char *argv[])
{
    int sockfd, portno;
    ssize_t n;

    struct sockaddr_in serv_addr;
    struct hostent *server;  //hostent structure is defined in <netdb.h>
 /*
  struct hostent  has variables:
      char *h_name;         /* official name of the host
      int h_addrtype;       /* host address type (e.g., AF_INET)
      int h_length;         /* length of the address in bytes
..... */
    
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"\t\t\tusage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("\t\t\tERROR opening socket");
    //function in C is used to resolve a hostname (like "www.google.com") to an IP address
    // it returns a pointer to a struct hostent, which contains the host information. On error, it returns NULL.
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"\t\t\tERROR, no such host\n");
        exit(0);
    }
    // initialize structure serv_addr.
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    // copy a specified number of bytes from a source memory area to a destination
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    
    serv_addr.sin_port = htons(portno);
    
    // connect send a connection request to the server
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("\t\t\tERROR connecting");


        // taking the id of client id is like username
        int my_id;
        printf("\t\t\tenter you id in integer only:\n");
        scanf("%d",&my_id);
        getchar();
        struct msg m;
        m.sender_id=my_id;
        m.type=0;//0 for telling the server to fix my id for this connection
         n=write(sockfd,&m,sizeof(m));
            if(n<0)error("\t\t\tERROR in writing to socket");


    pthread_t rec_thread;
    pthread_create(&rec_thread,NULL,rec_msg,(void*)&sockfd);////////creating thread to read msgs
    pthread_detach(rec_thread);

    // asking the choic of the user what to do
    printf("\t\t\tEnter:\n 1:private message\n 2:everyone\n 3:Go offline\n");
    int choice;
    scanf("%d",&choice);
    getchar();
    while(choice==1||choice==2){
        int other_clientid=-1;
        if(choice==1){
            printf("\t\t\tenter receiver client id\n");
            scanf("%d",&other_clientid);
            getchar();
            while(other_clientid<0){
                printf("\t\t\tthe client id should be greater than 0...enter id again\n");
                scanf("%d",&other_clientid);
                getchar();
            }
        }
        printf("\t\t\t when you are done with private messaging enter 'exit' to end the chat\n");
        while(1){
            fgets(buffer,256,stdin);
            buffer[strcspn(buffer,"\n")]=0;
            if(strcmp(buffer,"exit")==0)break;

            //filling the details into message data structure
            struct msg m;
            m.sender_id=my_id;
            m.receiver_id=other_clientid;
            m.type=choice;
            strcpy(m.text,buffer);
            n=write(sockfd,&m,sizeof(m));
            if(n<0)error("\t\t\tERROR in writing to socket");
            bzero(buffer,256);
        }
        printf("\t\t\tEnter:\n 1:private message\n 2:everyone\n 3:Go offline\n");
        scanf("%d",&choice);
        getchar();
        bzero(buffer,256);
        
    }
    printf("\t\t\tGoing offline.....\n");
    
    m.sender_id=my_id ;
    m.type=3;
    write(sockfd,&m,sizeof(m));
   close(sockfd);
    exit(0);
    // create and intilaze to zeros, a buffer to store the message
    
}

