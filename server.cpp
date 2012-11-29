

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
using namespace std;
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
#include <cstring>
#include "deck.h"
#include "serverdata.h"
#define PROTOPORT       36729            /* default protocol port number */
#define QLEN            6               /* size of request queue        */
#define TRUE             1
#define FALSE            0
int     visits      =   0;              /* counts client connections    */
fd_set		master_fd_read;
fd_set		working_fd_read;


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
 

int main(int argc, char *argv[])
{
        struct  	hostent  *ptrh;  /* pointer to a host table entry       */
        struct  	protoent *ptrp;  /* pointer to a protocol table entry   */
        struct  	sockaddr_in sad; /* structure to hold server's address  */
        struct  	sockaddr_in cad; /* structure to hold client's address  */
        int     	server_sd, client_sd, new_sd,g;         /* socket descriptors                  */
        int			OPTVAL = 1;
        int			flags = 1;
        int 		numplayers = 0;
        int     	port;            /* protocol port number                */
        char 		string[8];
        vector		<client_data> client_vec;
        int 		n,x,k, rc, max_sd, desc_ready, close_conn,len;
        int		lobbytimer;
        int 		end_server = false;
        socklen_t   alen;            /* length of address                   */
        char		msg[100] = "Accepts message length of <= 1000\n";
        char  		args[1000];
        char 		command[100];
        int			min_players = 2;
        int 		max_players = 10;
        bool		invalid = false;
        
        char    	buf[1000];       /* buffer for string the server sends  */
        timeval 	timeout;
        deck*		uno_deck = new deck;
#ifdef WIN32
        WSADATA wsaData;
        WSAStartup(0x0101, &wsaData);
#endif
        memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
        sad.sin_family = AF_INET;         /* set family to Internet     */
        sad.sin_addr.s_addr = INADDR_ANY; /* set the local IP address   */
		
		srand ( unsigned ( time (NULL) ) );
        /* Check command-line argument for protocol port and extract    */
        /* port number if one is specified.  Otherwise, use the default */
        /* port value given by constant PROTOPORT                       */
        if (argc > 3) {
        		if ((max_players = atoi(argv[2])) > 10)
        		{
        			fprintf(stdout,"That is more than the allowed number of players for uno. setting max to 10\n");
        			max_players = 10;
        		}
        		else if ((max_players = atoi(argv[2])) < 2)
        		{
        			fprintf(stdout,"That is less than the allowed number of players for uno. setting max to 2\n");
        			max_players = 2;
        		}
        		if ((min_players = atoi(argv[1])) < 2)
        		{
        			fprintf(stdout,"That is less than the minimum number of players for uno. setting min to 2\n");
        			min_players = 2;
        		}
        		else if ((min_players = atoi(argv[1])) > max_players)
        		{
        			fprintf(stdout,"That is more than the specified max number of players. setting min to max\n");
        			min_players = max_players;
        		}
        		
       			port = atoi(argv[3]);
       	} else if (argc > 2) {       			
       			if ((max_players = atoi(argv[2])) > 10)
        		{
        			fprintf(stdout,"That is more than the allowed number of players for uno. setting max to 10\n");
        			max_players = 10;
        		}
        		else if ((max_players = atoi(argv[2])) < 2)
        		{
        			fprintf(stdout,"That is less than the allowed number of players for uno. setting max to 2\n");
        			max_players = 2;
        		}
        		if ((min_players = atoi(argv[1])) < 2)
        		{
        			fprintf(stdout,"That is less than the minimum number of players for uno. setting min to 2\n");
        			min_players = 2;
        		}
        		else if ((min_players = atoi(argv[1])) > max_players)
        		{
        			fprintf(stdout,"That is more than the specified max number of players. setting min to max\n");
        			min_players = max_players;
        		}
        		fprintf(stdout,"No port number specified. Defaulting to port number %d\n",PROTOPORT);
       			port = PROTOPORT;      	
       	} else if (argc > 1) {                 /* if argument specified        */       			
       			fprintf(stdout,"No maximum number of players specified. Defaulting to %d player max\n",max_players);
       			if ((min_players = atoi(argv[1])) < 2)
        		{
        			fprintf(stdout,"That is less than the minimum number of players for uno. setting min to 2\n");
        			min_players = 2;
        		}
        		else if ((min_players = atoi(argv[1])) > max_players)
        		{
        			fprintf(stdout,"That is more than the specified max number of players. setting min to max\n");
        			min_players = max_players;
        		}
        		fprintf(stdout,"No port number specified. Defaulting to port number %d\n",PROTOPORT);
       			port = PROTOPORT;      	
        } else {
        		fprintf(stdout,"No maximum number of players specified. Defaulting to %d player max\n",max_players);
       			fprintf(stdout,"No minimum number of players specified. Defaulting to %d player min\n",min_players);
       			fprintf(stdout,"No port number specified. Defaulting to port number %d\n",PROTOPORT);
                port = PROTOPORT;       /* use default port number      */
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		lobbytimer = 10;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
        
        /* Main server loop - accept and handle requests */
        while (1) {
        		invalid = false;
        		//////////////////////////////////////////////////////////////////////////
        		/*copy master set over to the working set*/
        		memcpy(&working_fd_read,&master_fd_read, sizeof(master_fd_read));
        		//////////////////////////////////////////////////////////////////////////
        		
        		//////////////////////////////////////////////////////////////////////////
        		/*call select and wait for it to finish*/
        		//fprintf(stdout,"\n");
        		//fprintf(stdout,"Currently connected players are: \n");
        		//if (client_vec.size() == 0)
        		//{
        		//	fprintf(stdout,"None\n");
        		//}
				//for (int g=0; g<client_vec.size(); g++)
				//{	
				//	client_vec[g].display();
				//}
				//fprintf(stdout,"\n");
				//fprintf(stdout,"Time left until game starts is %d seconds\n",lobbytimer);
        		//fprintf(stdout,"Waiting on select\n");
        		rc = select(max_sd + 1, &working_fd_read, NULL, NULL, &timeout);
        		if (numplayers >= min_players and rc == 0)
        		{
        			timeout.tv_sec = 1;
					timeout.tv_usec = 0;
        			//fprintf(stdout,"timeout\n");
        			lobbytimer = lobbytimer - 1;
        			fprintf(stdout,"time left is %d\n",lobbytimer);
        			//if (numplayers == max_players)
        			//{
        			//	lobbytimer = 0;
        			//}
        			if (lobbytimer == 0)
        			{
        				start_game(&client_vec);
        				deal(&client_vec,uno_deck);			
        				playgame(&client_vec,&max_sd,server_sd,max_players,uno_deck,&numplayers);	////////////////									PLAYING THE GAME
        				fprintf(stdout,"************************************************************************************\n");
						fprintf(stdout,"********************************** GAME HAS ENDED **********************************\n");
						fprintf(stdout,"************************************************************************************\n");
						bool found = false; 
						client_data client;
						for (vector <client_data>::iterator itr = client_vec.begin(); itr != client_vec.end(); ++itr)
						{
							itr->turn = false;
							if (not found && not itr->spec)
							{
								found = true;
								client = *itr;
								client_vec.erase(itr);
								client_vec.push_back(client);
							}
						}
        				lobbytimer = 5;
        				g = 0;
        				while (g < client_vec.size() and numplayers < max_players)
        				{
        					if (client_vec[g].spec)
        					{
        						fprintf(stderr,"making %s a player\n",client_vec[g].name);
        						client_vec[g].spec = false;
        						fprintf(stderr,"Player: %s, Player #: %d, sd: %d, Turn?: %d, Spec?: %d\n",client_vec[g].name,g,client_vec[g].sd,client_vec[g].turn,client_vec[g].spec);   						
        						numplayers = numplayers + 1;
        					}
        					else
        					{
        						fprintf(stderr,"Player: %s, Player #: %d, sd: %d, Turn?: %d, Spec?: %d\n",client_vec[g].name,g,client_vec[g].sd,client_vec[g].turn,client_vec[g].spec); 
        					}
        					g = g + 1;
        				}
        			}
        		}
        		else if (rc == 0)
        		{
        			//fprintf(stdout,"under minimum number of players. lobby timer not ticking\n");
        			lobbytimer = 5;
        			timeout.tv_sec = 1;
					timeout.tv_usec = 0;
        		}
        		
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
        		//if (rc == 0)
        		//{
        		//	fprintf(stdout,"select() timed out.\n");
        		//}
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
        						fprintf(stdout,"added connection %d to fd_set\n",new_sd);
        						if (new_sd > max_sd) 
        						{
        							max_sd = new_sd;
        							fprintf(stdout,"max_sd is now %d\n",new_sd);
        						}
        					//} while (new_sd != -1);
        				}
        				else //not the server socket so an existing connection must be readable
        				{
        					fprintf(stdout,"descriptor %d is readable\n", i);
        					close_conn = false;
        					//do
        					//{	/*receive data on this conn*/
        						rc = read(i,buf,sizeof(buf));
        						if (rc < 0)
        						{
        							if (errno != EWOULDBLOCK)
                     				{
                        				perror("  recv() failed");
                        				close_conn = true;
                     				}
                     				//break;
                     			}
                     			//check to see if connection has been closed by client
                     			if (rc == 0)
                     			{
                     				printf("  Connection closed\n");
                     				close_conn = true;
                     				//break;
                  				}
                  				//data was received
                  				len = rc;
                  				//fprintf(stdout,"%d bytes received\n",len);
                  				//fprintf(stdout,"received \"%s\"\n",buf);
                  				/*here is where i should be doing things with what a client sends me*/
                  				//echo data back to the client
                  				if (not close_conn && not arg_parse(buf,command,args,len)) //parse the line sent by the client
                  				{
                  					rc = send(i,"[INVALID|Not a valid command format]\n",strlen("[INVALID|Not a valid command format]\n")+1,0);
                  					invalid = true;
                  				}
                  				if (strcmp(command,"JOIN") != 0 and not close_conn and not invalid){
		              				if (not player_is_in_lobby(&client_vec,i))
		              				{
		              					rc = send(i,"[INVALID|You cannot send commands unless you \"JOIN\" first]\n",strlen("[INVALID|You cannot send commands unless you \"JOIN\" first]\n")+1,0);
		              					invalid = true;
		              				}
		              			}
                  				//fprintf(stdout,"command is \"%s\" and args are \"%s\"\n",command,args);
								if (not close_conn and not invalid)
								{
									player_commands(&client_vec,i,command,args,max_players,false,uno_deck,&numplayers);
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
        						//fprintf(stdout,"gotta close this connection\n");
        						
        						close(i);
        						FD_CLR(i,&master_fd_read);
        						fprintf(stdout,"removed connection %d from fd_set\n",i);
        						for (int k = 0; k < client_vec.size(); k++)
        						{
        							if (client_vec[k].sd == i)
        							{	
        								fprintf(stdout,"Client name: %s Socket: %d (disconnected)\n",client_vec[k].name,client_vec[k].sd);
        								if (not client_vec[k].spec)
        								{
        									numplayers = numplayers - 1;
        									int g = 0;
        									while (g < client_vec.size() and numplayers < max_players)
											{
												if (client_vec[g].spec)
												{
													client_vec[g].spec = false;
													numplayers = numplayers + 1;
												}
												g = g + 1;
											}
        								}
        								client_vec.erase(client_vec.begin() + k);
        								break;
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
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
void playgame(vector <client_data>* client_vec,int* max_sd,int server_sd,int max_players,deck* uno_deck,int* numplayers)
{
	bool playing = true;
	int  rc,desc_ready,len,new_sd;
	card discard_card;
	int  end_server = false;
	bool close_conn = false;
	bool invalid = false;
	char buf[1000];
	char args[1000];
    char command[100];
	send_go(client_vec,uno_deck);
	struct  	sockaddr_in cad; /* structure to hold client's address  */
	 socklen_t   alen;            /* length of address                   */
	
	fprintf(stdout,"************************************************************************************\n");
	fprintf(stdout,"********************************* GAME HAS STARTED *********************************\n");
	fprintf(stdout,"************************************************************************************\n");
	while(playing){
		memcpy(&working_fd_read,&master_fd_read, sizeof(master_fd_read));
		rc = select(*max_sd + 1, &working_fd_read, NULL, NULL, NULL);
		desc_ready = rc;
		for (int i = 0; i <= *max_sd and desc_ready > 0; i++)
		{
			if (FD_ISSET(i, &working_fd_read))
			{
				desc_ready = desc_ready - 1;	//found a readable sd so decrement descriptors ready
				
				if (i == server_sd)	//check to see if this is the server sd
				{
					fprintf(stdout,"server socket is readable\n");
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
					fprintf(stdout,"added connection %d to fd_set\n",new_sd);
					if (new_sd > *max_sd) 
					{
						*max_sd = new_sd;
						fprintf(stdout,"max_sd is now %d\n",new_sd);
					}
				}
				else //not the server socket so an existing connection must be readable
				{
					fprintf(stdout,"descriptor %d is readable\n", i);
					close_conn = false;
					//do
					//{	/*receive data on this conn*/
						rc = read(i,buf,sizeof(buf));
						if (rc < 0)
						{
							if (errno != EWOULDBLOCK)
             				{
                				perror("  recv() failed");
                				close_conn = true;
             				}
             				//break;
             			}
             			//check to see if connection has been closed by client
             			if (rc == 0)
             			{
             				printf("  Connection closed\n");
             				close_conn = true;
             				//break;
          				}
          				//data was received
          				len = rc;
          				if (not close_conn && not arg_parse(buf,command,args,len)) //parse the line sent by the client
          				{
          					rc = send(i,"[INVALID|Not a valid command format]\n",strlen("[INVALID|Not a valid command format]\n")+1,0);
          					invalid = true;
          				}
          				//fprintf(stdout,"command is \"%s\" and args are \"%s\"\n",command,args);
						if (not close_conn and not invalid)
						{
							playing = player_commands(client_vec,i,command,args,max_players,true,uno_deck,numplayers);
							if (not playing)
							{
								for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
								{
									itr->clienthand.hand.erase(itr->clienthand.hand.begin(),itr->clienthand.hand.end());
								}
								delete uno_deck;
								uno_deck = new deck;
							}
						}
          				//fprintf(stdout,"%d bytes received\n",len);
          				//fprintf(stdout,"received \"%s\"\n",buf);
          				/*here is where i should be doing things with what a client sends me*/
					
					//if close_conn is true then we need to clean up this closed connection.
					//this includes removing the fd from the master set and determining the new
					//max fd value based on the bits still turned on in the master read set
					if (close_conn)
					{
						//fprintf(stdout,"gotta close this connection\n");
						for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
						{
							if (itr->sd == i && itr->turn)
							{
								set_turn(client_vec,uno_deck);
								*numplayers = *numplayers - 1;
								fprintf(stdout,"Client name: %s Socket: %d (disconnected)\n",itr->name,itr->sd);
								discard_card = uno_deck->show_top();
								uno_deck->discard_pile.erase(uno_deck->discard_pile.end()-1);
								for (vector <card>::iterator itp = itr->clienthand.hand.begin(); itp != itr->clienthand.hand.end(); ++itp)
								{
									uno_deck->discard(card(itp->type,itp->color));
								}
								fprintf(stderr,"\n");
								uno_deck->discard(discard_card);
								client_vec->erase(itr);								
								if (*numplayers == 1)
								{
									playing = false;
									strcpy(buf,"[GG|");
									for (vector <client_data>::iterator itp = client_vec->begin(); itp != client_vec->end(); ++itp)
									{
										if (not itp->spec)
										{
											strcat(buf,itp->name);
										}
									}
									strcat(buf,"]");
									send_to_players(client_vec,buf);
									for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
									{
										itr->clienthand.hand.erase(itr->clienthand.hand.begin(),itr->clienthand.hand.end());
									}
									delete uno_deck;
									uno_deck = new deck;
									break;
								}
								send_go(client_vec,uno_deck);
								break;
							}
							else if	(itr->sd == i)
							{
								fprintf(stdout,"Client name: %s Socket: %d (disconnected)\n",itr->name,itr->sd);
								discard_card = uno_deck->show_top();
								uno_deck->discard_pile.erase(uno_deck->discard_pile.end()-1);
								for (vector <card>::iterator itp = itr->clienthand.hand.begin(); itp != itr->clienthand.hand.end(); ++itp)
								{
									uno_deck->discard(card(itp->type,itp->color));
								}
								fprintf(stderr,"\n");
								uno_deck->discard(discard_card);
								if (not itr->spec)
								{
									*numplayers = *numplayers - 1;
								}
								client_vec->erase(itr);
								if (*numplayers == 1)
								{
									playing = false;
									strcpy(buf,"[GG|");
									for (vector <client_data>::iterator itp = client_vec->begin(); itp != client_vec->end(); ++itp)
									{
										if (not itp->spec)
										{
											strcat(buf,itp->name);
										}
									}
									strcat(buf,"]");
									send_to_players(client_vec,buf);
									for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
									{
										itr->clienthand.hand.erase(itr->clienthand.hand.begin(),itr->clienthand.hand.end());
									}
									delete uno_deck;
									uno_deck = new deck;
									break;
								}
								break;
							}	
						}
						close(i);
						FD_CLR(i,&master_fd_read);
						fprintf(stdout,"removed connection %d from fd_set\n",i);
						if (i == *max_sd)
						{
							while (FD_ISSET(*max_sd, &master_fd_read) == false) 
							{
								*max_sd = *max_sd - 1;
							}        	       								
						}
					}
				}
			}
		}
	}
}







