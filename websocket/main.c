#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <libwebsockets.h>

#define BUFSIZE 1024

/*
 *  * error - wrapper for perror
 *   */
void error(char *msg) {
    perror(msg);
    exit(1);
}

int sockfd;                     /* socket */
char udp_buf[BUFSIZE];          /* message buf */
int receive_flag = 0;
char tmp[BUFSIZE];              /* tmp buf */

void * recv_data_thread(void *arg) {
    int clientlen;                  /* byte size of client's address */
    struct sockaddr_in clientaddr;  /* client addr */
    struct hostent *hostp;          /* client host info */
    char *hostaddrp;                /* dotted decimal host addr string */
    int optval;                     /* flag value for setsockopt */
    int n;                          /* message byte size */

    while (1) {

        /*
         * recvfrom: receive a UDP datagram from a client
         */
        bzero(udp_buf, BUFSIZE);
        n = recvfrom(sockfd, udp_buf, BUFSIZE, 0,
            (struct sockaddr *) &clientaddr, &clientlen);
        if (n < 0)
            error("ERROR in recvfrom");

        strcpy(tmp, udp_buf);

        /* 
         * gethostbyaddr: determine who sent the datagram
         */
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL)
            error("ERROR on inet_ntoa\n");
        printf("server received datagram from %s\n", hostaddrp);
        printf("server received %d/%d bytes: %s\n", (int)strlen(udp_buf), n, udp_buf);
      
        sendto(sockfd, "ok", strlen("ok"), 0, (struct sockaddr *) &clientaddr, clientlen);

        receive_flag = 1;
    }
}

static int callback_http(struct lws *wsi,
                         enum lws_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    return 0;
}



static int callback_dumb_increment(struct lws *wsi,
                                   enum lws_callback_reasons reason,
                                   void *user, void *in, size_t len)
{

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
            printf("received data: %s\n", (char *) in);

            while(1) {

                if (receive_flag == 0 ) {
                    usleep(1000 * 10);
                    continue;
                }

                int back_len = strlen(tmp);
                char *ws_buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + back_len +
                                                             LWS_SEND_BUFFER_POST_PADDING);
                
                strcpy(&ws_buf[LWS_SEND_BUFFER_PRE_PADDING], tmp);
                        
                lws_write(wsi, &ws_buf[LWS_SEND_BUFFER_PRE_PADDING], back_len, LWS_WRITE_TEXT);

                free(ws_buf);

                receive_flag = 0;
            }
            break;
        }
        default:
            break;
    }
    
    return 0;
}



static struct lws_protocols protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "http-only",   // name
        callback_http, // callback
        0              // per_session_data_size
    },
    {
        "dumb-increment-protocol", // protocol name - very important!
        callback_dumb_increment,   // callback
        0                          // we don't use any per session data
    },
    {
        NULL, NULL, 0   /* End of list */
    }
};


int main(void) {
    // server url will be http://localhost:9000
    int port = 9000;
    struct lws_context *context;
    struct lws_context_creation_info context_info =
    {
        .port = port, .iface = NULL, .protocols = protocols, .extensions = NULL,
        .ssl_cert_filepath = NULL, .ssl_private_key_filepath = NULL, .ssl_ca_filepath = NULL,
        .gid = -1, .uid = -1, .options = 0, NULL, .ka_time = 0, .ka_probes = 0, .ka_interval = 0,
    };
    // create lws context representing this server
    context = lws_create_context(&context_info);

    if (context == NULL) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }

    
    printf("starting ws server...\n");


    int portno; /* port to listen on */
    int clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char udp_buf[BUFSIZE]; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    pthread_t recv_thread;

    portno = 9001;

    /* 
     * socket: create the parent socket 
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
      error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets 
     * us rerun the server immediately after we kill it; 
     * otherwise we have to wait about 20 secs. 
     * Eliminates "ERROR on binding: Address already in use" error. 
     */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
           (const void *)&optval , sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    /* 
     * bind: associate the parent socket with a port 
     */
    if (bind(sockfd, (struct sockaddr *) &serveraddr, 
         sizeof(serveraddr)) < 0) 
      error("ERROR on binding");

    /* 
     * main loop: wait for a datagram, then echo it
     */
    clientlen = sizeof(clientaddr);

    pthread_create(&recv_thread, NULL, recv_data_thread, NULL);
    
    // infinite loop, to end this server send SIGTERM. (CTRL+C)
    while (1) {
        lws_service(context, 50);
        // lws_service will process all waiting events with their
        // callback functions and then wait 50 ms.
        // (this is a single threaded webserver and this will keep our server
        // from generating load while there are not requests to process)
    }
    
    lws_context_destroy(context);
    
    return 0;
}
