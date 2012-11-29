//

/* client.c - code for example client program that uses TCP */
#ifndef unix
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "deck.h"
#include "clientdata.h"
#define PROTOPORT       36729            /* default protocol port number */
extern  int             errno;
char    localhost[] =   "localhost";    /* default host name            */

using namespace std;
/*------------------------------------------------------------------------
 * Program:   client
 *
 * Purpose:   allocate a socket, connect to a server, and print all output
 *
 * Syntax:    client [ host [port] ]
 *
 *               host  - name of a computer on which server is executing
 *               port  - protocol port number server is using
 *
 * Note:      Both arguments are optional.  If no host name is specified,
 *            the client uses "localhost"; if no protocol port is
 *            specified, the client uses the default given by PROTOPORT.
 *
 *------------------------------------------------------------------------
 */

int main(int argc, char *argv[])
{
        struct  hostent  *ptrh;  /* pointer to a host table entry       */
        struct  protoent *ptrp;  /* pointer to a protocol table entry   */
        struct  sockaddr_in sad; /* structure to hold an IP address     */
        int			flags = 1;
        int     sd, rc,desc_ready;              /* socket descriptor                   */
        int     port;            /* protocol port number                */
        char    *host;           /* pointer to host name                */
        int     n;               /* number of characters read           */
        timeval 	time;
        fd_set		master_fd_read;
        fd_set		working_fd_read;
        bool	close_conn;
        bool	playing	= true;
        char 	name[100];
        bool	automatic = false;
        char    buf[1000];       /* buffer for data from the server     */
        myhand 	currenthand;
#ifdef WIN32
        WSADATA wsaData;
        WSAStartup(0x0101, &wsaData);
#endif
        memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
        sad.sin_family = AF_INET;         /* set family to Internet     */
        /* Check command-line argument for protocol port and extract    */
        /* port number if one is specified.  Otherwise, use the default */

		
        /* port value given by constant PROTOPORT                       */
        if (argc > 4)
        {	
        	if (strcmp(argv[4],"a") == 0)
        	{
        		automatic = true;
        		fprintf(stderr,"Automatic Mode Set\n");
        	}
        	port = atoi(argv[3]);
        	if (strcmp(argv[2],"localhost")) host = localhost;
            else host = argv[2];
        	strcpy(name,argv[1]);
        }
        if (argc > 3) {
        	port = atoi(argv[3]);
        	if (strcmp(argv[2],"localhost")) host = localhost;
            else host = argv[2];
        	strcpy(name,argv[1]);
        }
        else if (argc > 2) {
        		if (strcmp(argv[2],"localhost")) host = localhost;
                else host = argv[2];
                strcpy(name,argv[1]);
                port = PROTOPORT;         /* if host argument specified   */
        }
        else if (argc > 1) {                 /* if protocol port specified   */
                strcpy(name,argv[1]);   /* convert to binary            */
                host = localhost;			
                port = PROTOPORT;
        } else {
        	fprintf(stderr,"Usage: ./client [name] [host] [port] [a]\n");
        	exit(0);
        	host = localhost;
                port = PROTOPORT;       /* use default port number      */
        }
        if (port > 0)                   /* test for legal value         */
                sad.sin_port = htons((u_short)port);
        else {                          /* print error message and exit */
                fprintf(stderr,"bad port number %s\n",argv[2]);
                exit(1);
        }
        /* Check host argument and assign host name. */
        if (argc > 2) {
                host = argv[2];         /* if host argument specified   */
        }
        /* Convert host name to equivalent IP address and copy to sad. */
        ptrh = gethostbyname(host);
        if ( ((char *)ptrh) == NULL ) {
                fprintf(stderr,"invalid host: %s\n", host);
                exit(1);
        }
        memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
        /* Map TCP transport protocol name to protocol number. */
        if ( (ptrp = getprotobyname("tcp")) == 0) {
                fprintf(stderr, "cannot map \"tcp\" to protocol number");
                exit(1);
        }
        /* Create a socket. */
        sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
        if (sd < 0) {
                fprintf(stderr, "socket creation failed\n");
                exit(1);
        }
        /* Connect the socket to the specified server. */
        if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
                fprintf(stderr,"connect failed\n");
                exit(1);
        }
        /* Send info to the server */
        //sprintf(buf,"GO CF367!!! This server has been contacted %d time%s\n",
        //                visits,visits==1?".":"s.");
        //        send(sd2,buf,strlen(buf),0);
        /* Repeatedly read data from socket and write to user's screen. */
		
		FD_ZERO(&master_fd_read);
        FD_SET(sd,&master_fd_read);
        FD_SET(0,&master_fd_read);
        time.tv_sec = 5;
		time.tv_usec = 0;
		
        join(sd,name);
        fprintf(stderr,"||Commands||\nTo chat, prefix your input with 'chat '\nTo play, prefix your choice with 'play '\nTo quit, enter 'quit'\n===================================================\n");
        while(playing){
        	
        	memcpy(&working_fd_read,&master_fd_read, sizeof(master_fd_read));
        	rc = select(sd + 1, &working_fd_read, NULL, NULL, &time);
        	if (rc < 0)
    		{
    			perror("select() failed");
    		}
    		
        	time.tv_sec = 5;
			desc_ready = rc;
			for (int i = 0; i <= sd and desc_ready > 0; i++)
    		{
    			if (FD_ISSET(i, &working_fd_read))
    			{
    				desc_ready = desc_ready - 1;
    				rc = read(i,buf,sizeof(buf));
    				if (rc == 0)
         			{
         				printf("  Server Connection Closed. Exiting\n");
         				exit(-1);
         				//break;
      				}
    				//while (n > 0) {
					if (i != 0)
					{	
						read_message(i,buf,&currenthand,name,automatic);
					}
					else
					{
						player_command(rc,sd,buf,&currenthand);
					}
					//	rc = read(sd,buf,sizeof(buf));
					//}
				}
			}		    
        }
        /* Close the socket. */
        closesocket(sd);
        /* Terminate the client program gracefully. */
        exit(0);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


