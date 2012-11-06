

/* server.c - code for example server program that uses TCP */
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
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include "clientdata.h"
#define PROTOPORT       36729            /* default protocol port number */
#define QLEN            6               /* size of request queue        */
#define TRUE             1
#define FALSE            0
int     visits      =   0;              /* counts client connections    */

using namespace std;

/*------------------------------------------------------------------------
 * Program:   server
 *
 * Purpose:   allocate a socket and then repeatedly execute the following:
 *              (1) wait for the next connection from a client
 *              (2) send a short message to the client
 *              (3) close the connection
 *              (4) go back to step (1)
 *
 * Syntax:    server [ port ]
 *
 *               port  - protocol port number to use
 *
 * Note:      The port argument is optional.  If no port is specified,
 *            the server uses the default given by PROTOPORT.
 *
 *------------------------------------------------------------------------
 */
 
void arg_parse(char* line, char* command, char* args, int len);
void player_commands(vector <client_data>* client_vec, int socket, char* command, char* args, int max_players);
int main(int argc, char *argv[])
{
        struct  	hostent  *ptrh;  /* pointer to a host table entry       */
        struct  	protoent *ptrp;  /* pointer to a protocol table entry   */
        struct  	sockaddr_in sad; /* structure to hold server's address  */
        struct  	sockaddr_in cad; /* structure to hold client's address  */
        int     	server_sd, client_sd, new_sd;         /* socket descriptors                  */
        int			OPTVAL = 1;
        int			flags = 1;
        int     	port;            /* protocol port number                */
        char 		string[8];
        vector		<client_data> client_vec;
        int 		n,x,k, rc, max_sd, desc_ready, close_conn,len, lobbytimer;
        int 		end_server = false;
        socklen_t   alen;            /* length of address                   */
        char		msg[100] = "Accepts message length of <= 1000\n";
        char  		args[1000];
        int			min_players = 2;
        int 		max_players = 10;
        char 		command[100];
        char    	buf[1000];       /* buffer for string the server sends  */
        timeval 	timeout;
        fd_set		master_fd_read;
        fd_set		master_fd_write;
        fd_set		working_fd_read;
        fd_set		working_fd_write;
#ifdef WIN32
        WSADATA wsaData;
        WSAStartup(0x0101, &wsaData);
#endif
        memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
        sad.sin_family = AF_INET;         /* set family to Internet     */
        sad.sin_addr.s_addr = INADDR_ANY; /* set the local IP address   */


        /* Check command-line argument for protocol port and extract    */
        /* port number if one is specified.  Otherwise, use the default */
        /* port value given by constant PROTOPORT                       */
        if (argc > 1) {                 /* if argument specified        */
                port = atoi(argv[1]);   /* convert argument to binary   */
        } else {
                port = PROTOPORT;       /* use default port number      */
        }
        if (port > 0)                   /* test for illegal value       */
                sad.sin_port = htons((u_short)port);
        else {                          /* print error message and exit */
                fprintf(stderr,"bad port number %s\n",argv[1]);
                exit(1);
        }
        /* Map TCP transport protocol name to protocol number */
        if ( (ptrp = getprotobyname("tcp")) == 0) {
                fprintf(stderr, "cannot map \"tcp\" to protocol number");
                exit(1);
        }
        /* Create a socket */
        server_sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
        setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, &OPTVAL, sizeof(OPTVAL));
        if (server_sd < 0) {
                fprintf(stderr, "socket creation failed\n");
                exit(1);
        }
        /*set socket to be nonblocking*/
        rc = ioctl(server_sd, FIONBIO, (char *)&flags);
        if (rc < 0)
   		{
      		perror("ioctl() failed");
      		close(server_sd);
      		exit(-1);
   		}
        /* Bind a local address to the socket */
        if (bind(server_sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
                fprintf(stderr,"bind failed\n");
                exit(1);
        }
        /* Specify size of request queue */
        if (listen(server_sd, QLEN) < 0) {
                fprintf(stderr,"listen failed\n");
                exit(1);
        }
        alen = sizeof(cad);
////////////////////////////////////////////////////////////////////
        /*Initialize the fd_sets*/
        FD_ZERO(&master_fd_read);
        //FD_ZERO(&master_fd_write);
        max_sd = server_sd;
        FD_SET(server_sd,&master_fd_read);
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
		/*initialize timeval struct to five seconds*/
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		lobbytimer = 30;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

        
        /* Main server loop - accept and handle requests */
        while (1) {
        		
        		//////////////////////////////////////////////////////////////////////////
        		/*copy master set over to the working set*/
        		memcpy(&working_fd_read,&master_fd_read, sizeof(master_fd_read));
        		//////////////////////////////////////////////////////////////////////////
        		
        		//////////////////////////////////////////////////////////////////////////
        		/*call select and wait for it to finish*/
        		fprintf(stdout,"\n");
        		fprintf(stdout,"Currently connected players are \n");
				for (int g=0; g<client_vec.size(); g++)
				{	
					client_vec[g].display();
				}
				fprintf(stdout,"\n");
				fprintf(stdout,"Time left until game starts is %d seconds\n",lobbytimer);
        		fprintf(stdout,"Waiting on select\n");
        		rc = select(max_sd + 1, &working_fd_read, NULL, NULL, &timeout);
        		if (client_vec.size() >= min_players)
        		{
        			fprintf(stdout,"decrementing lobby timer\n");
        			lobbytimer = lobbytimer - 5;
        			if (lobbytimer == 0)
        			{
        				fprintf(stdout,"************************************************************************************\n");
        				fprintf(stdout,"********************************* GAME HAS STARTED *********************************\n");
        				fprintf(stdout,"************************************************************************************\n");
        				break;
        			}
        		}
        		else
        		{
        			fprintf(stdout,"under minimum number of players. lobby timer not ticking\n");
        			lobbytimer = 30;
        		}
        		
        		timeout.tv_sec = 5;
				timeout.tv_usec = 0;
        		//////////////////////////////////////////////////////////////////////////
        		
        		//////////////////////////////////////////////////////////////////////////
        		/*check to see if select failed*/
        		if (rc < 0)
        		{
        			perror("select() failed");
        		}
        		//////////////////////////////////////////////////////////////////////////
        		
        		//////////////////////////////////////////////////////////////////////////
        		/*check to see if timeout expired*/
        		if (rc == 0)
        		{
        			fprintf(stdout,"select() timed out.\n");
        		}
        		//////////////////////////////////////////////////////////////////////////
        		
        		//////////////////////////////////////////////////////////////////////////
        		/*one or more descriptors are readable. loop through to find which ones*/
        		desc_ready = rc;
        		for (int i = 0; i <= max_sd and desc_ready > 0; i++)
        		{
        			if (FD_ISSET(i, &working_fd_read))
        			{
        				desc_ready = desc_ready - 1;	//found a readable sd so decrement descriptors ready
        				
        				if (i == server_sd)	//check to see if this is the server sd
        				{
        					fprintf(stdout,"server socket is readable\n");
        					//do
        					//{	/*accept each incoming connection before looping back to select again*/
        						fprintf(stdout,"gonna accept\n");
        						new_sd = accept(server_sd,(struct sockaddr *)&cad, &alen);
        						if (new_sd < 0)
        						{
        							if (errno != EWOULDBLOCK)
                     				{
                        				perror("  accept() failed");
                        				end_server = TRUE;
                     				}
                     				//break;
        						}
        						
        						/*add incoming connection to the fd_set*/
        						fprintf(stdout,"new incoming connection - %d\n",new_sd);
        						FD_SET(new_sd, &master_fd_read);
        						if (new_sd > max_sd) 
        						{
        							max_sd = new_sd;
        						}
        					//} while (new_sd != -1);
        				}
        				else //not the server socket so an existing connection must be readable
        				{
        					fprintf(stdout,"descriptor %d is readable\n", i);
        					close_conn = false;
        					//do
        					//{	/*receive data on this conn*/
        						fprintf(stdout,"waiting to read\n");
        						rc = read(i,buf,sizeof(buf));
        						fprintf(stdout,"just read\n");
        						if (rc < 0)
        						{
        							if (errno != EWOULDBLOCK)
                     				{
                        				perror("  recv() failed");
                        				close_conn = TRUE;
                     				}
                     				//break;
                     			}
                     			//check to see if connection has been closed by client
                     			if (rc == 0)
                     			{
                     				printf("  Connection closed\n");
                     				close_conn = TRUE;
                     				//break;
                  				}
                  				//data was received
                  				len = rc;
                  				fprintf(stdout,"%d bytes received\n",len);
                  				/*here is where i should be doing things with what a client sends me*/
                  				//echo data back to the client
                  				
                  				arg_parse(buf,command,args,len); //parse the line sent by the client
                  				fprintf(stdout,"command is \"%s\" and args are \"%s\"\n",command,args);
								if (not close_conn)
								{
									player_commands(&client_vec,i,command,args,max_players);
								}
                  				//rc = send(i, buf, len, 0);
                  				//if (rc < 0)
                  				//{
                  				//	perror(" send() failed");
                  				//	close_conn = TRUE;
                  				//	//break;
                  				//}
        					//} while (TRUE);
        					
        					//if close_conn is true then we need to clean up this closed connection.
        					//this includes removing the fd from the master set and determining the new
        					//max fd value based on the bits still turned on in the master read set
        					if (close_conn)
        					{
        						close(i);
        						FD_CLR(i,&master_fd_read);
        						for (int g = 0; g < client_vec.size(); g++)
        						{
        							if (client_vec[g].sd == i)
        							{	
        								fprintf(stdout,"Client name: %s Socket: %d (disconnected)\n",client_vec[g].name,client_vec[g].sd);
        								client_vec.erase(client_vec.begin() + g);
        							}
        						}
        						if (i == max_sd)
        						{
        							while (FD_ISSET(max_sd, &master_fd_read) == false) 
        							{
        								max_sd = max_sd - 1;
        							}        								
        						}
        					}
        				}
        			}
        		}
        }
}

void arg_parse(char* line, char* command, char* args, int len)
{
	int 	x = 0;
	int 	k = -1;
	bool	bar = false;
	strcpy(command,"");
	strcpy(args,"");
	for (int j = 1; j < len-2; j++){

		if (line[j] == '|'){
			bar = true;
		}
		else if (not bar){
			command[x] = line[j];
			x = x + 1;
		}
		else{
			k = k + 1;
			//fprintf(stdout,"moving %c to %i in command\n",line[j],k);
			args[k] = line[j];
		}
	}
	args[k] = 0;
	command[x] = 0;
    //fprintf(stdout,"command is %s and args are %s\n",command,args);
}

void player_commands(vector <client_data>* client_vec, int socket, char* command, char* args, int max_players)
{
	if (strcmp(command,"JOIN") == 0)
	{
		fprintf(stdout,"Player wants to join with name \"%s\"\n",args);
		if (client_vec->size() < max_players)
		{
			client_data client = client_data(socket,args);
			client_vec->push_back(client);
			client.display();
		}
		else
		{
			fprintf(stdout,"The lobby is full and player %s cannot join the game\n",args);
		}
		
	}
	else if (strcmp(command,"CHAT") == 0)
	{
		fprintf(stdout,"Player wants to chat with message \"%s\"\n",args);
	}
	else if (strcmp(command,"PLAY") == 0)
	{
		fprintf(stdout,"Player wants to play with card \"%s\"\n",args);
	}	
	else
	{
		fprintf(stdout,"STFU PLAYER!!!\n");
	}
}
