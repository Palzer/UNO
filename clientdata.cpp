#include "clientdata.h"

using namespace std;

void join(int socket, char* name)
{
	bool ingame = false;
	char buf[300];
	int rc;
	
	while (not ingame)
	{	
		strcpy(buf,"[JOIN|");
		strcat(buf,name);
		strcat(buf,"]");
		rc = send(socket,buf,strlen(buf),0);
		rc = read(socket,buf,sizeof(buf));
		if (strncmp(buf,"[ACCEPT",7) == 0)
		{
			fprintf(stderr,"Accepted: You were accepted into the game with the name \"%s\"\n",name);
			ingame = true;
		}
		else if(strncmp(buf,"[WAIT",5) == 0)
		{
			fprintf(stderr,"Wait: There is a game in progress. You are now a spectator.\n");
			ingame = true;
		}
		else
		{
			fprintf(stderr,"%s",buf);
			rc = read(0,name,sizeof(name));
			rc = strlen(name);
			if (name[rc-1] == '\n')
	    		name[rc-1] = 0;
		}
	}
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
char** parse_socket(int socket, char* line)
{
	int count = 0;
	int place = 0;
	bool bar = false;
	
	//fprintf(stderr,"message is %s\n",line);
	
		while (line[place] != 0){
			if (line[place] == ',')
			{
	 			count = count + 1;
			}
			else if ((line[place] == '|') && not bar){
				count = count + 1;
			}
			place = place + 1;
		}
		//fprintf(stderr,"the number of arguments is %d\n",count);
		count = count + 1;
		char** argv = (char**)malloc(count*sizeof(char*));
		count = 0;
		bar = false;
		place = 0;
		
		while (line[place] != 0){
			if (line[place] == ']')
			{
				line[place] = 0;
			}
			else if (line[place] == ',')
			{	
					line[place] = 0;
					argv[count] = &line[place+1];				//point to the different args
					//fprintf(stderr,"making arg[%i] %s\n",count,argv[count]);
					//fprintf(stderr,"arg %d is %s\n",count,argv[count]);
					count = count + 1;
			}
			else if ((line[place] == '|') && not bar)
			{
					argv[count] = &line[place+1];
					line[place] = 0;
					//fprintf(stderr,"making arg[%i] %s\n",count,argv[count]);
					count = count + 1;
					bar = true;
			}
			place = place + 1;
		}
		
		argv[count] = 0;
		count = 0;
		/*fprintf(stderr,"Arguments are:\n");
		while (argv[count] != 0)
		{
			fprintf(stderr,"%s\n",argv[count]);
			count = count + 1;
		}*/

		return argv;
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void server_message(int socket, char* buf,myhand* currenthand, char* name,bool automatic)
{
	int rc,i = 0;
	char sender[300];
	bool played = false;
	char msg[2] = " ";
	//fprintf(stderr,"Full buffer is %s\n",buf);
	char** argv = parse_socket(socket,buf);
	//fprintf(stderr,"parsed line and command is %s\n",buf);
	
	if (strcmp(buf,"[CHAT") == 0)
	{	
		fprintf(stderr,"%s: %s\n",argv[0],argv[1]);
	}
	if (strcmp(buf,"[DEAL") == 0)
	{
		fprintf(stderr,"You have received cards:\n");
		while (argv[i] != 0)
		{
			fprintf(stderr,"%c%c ",argv[i][0],argv[i][1]);
			i = i + 1;
		}
		fprintf(stderr,"\n");
		i = 0;
		while (argv[i] != 0)
		{
			currenthand->addcard(card(argv[i][1],argv[i][0]));
			i = i + 1;
		}
		
	}
	else if (strncmp(buf,"[UNO",4) == 0)
	{
		fprintf(stderr,"%s is on their last card!!\n",argv[0]);
	}
	else if (strncmp(buf,"[PLAYED",7) == 0)
	{
		if (strcmp(argv[0],name) == 0)
		{
			if (currenthand->last_played.color == argv[1][0] && currenthand->last_played.type == argv[1][1])
			{	
				if (argv[1][1] == 'W' || argv[1][1] == 'F')
				{
					currenthand->discard(card(argv[1][1],'N'));
				}
				else
				{
					currenthand->discard(card(argv[1][1],argv[1][0]));
				}
			}
		}
		else
		{
			fprintf(stderr,"%s played a %s\n",argv[0],argv[1]);
		}
	}
	else if (strncmp(buf,"[GO",3) == 0)
	{
		fprintf(stderr,"Your turn to play. The top card is a %s\n",argv[0]);
		if (automatic)
		{
			for (vector <card>::iterator itr = currenthand->hand.begin(); itr != currenthand->hand.end(); ++itr)
			{
				if (itr->color == argv[0][0] || itr->type == argv[0][1] || itr->type == 'F' || itr->type == 'W')
				{
					strcpy(sender,"[PLAY|");
					if (itr->color == 'N')
					{
						msg[0] = 'R';
						strcat(sender,msg);								
					}
					else
					{						
						msg[0] = itr->color;
						strcat(sender,msg);									
					}
					currenthand->last_played = card(itr->type,msg[0]);
					msg[0] = itr->type;
					strcat(sender,msg);	
					strcat(sender,"]");				
					rc = send(socket,sender,strlen(sender),0);
					fprintf(stderr,"sent message \'%s\' to the server\n",sender);
					played = true;
					break;
				} 
			}
			if(not played)
			{
				rc = send(socket,"[PLAY|NN]",strlen("[PLAY|NN]"),0);
			}
		}
	}
	else if (strncmp(buf,"[INVALID",8) == 0)
	{
		fprintf(stderr,"Invalid: \"%s\"\n",argv[0]);
	}
	else if (strncmp(buf,"[PLAYERS",8) == 0)
	{	
		fprintf(stderr,"Current players are:\n");
		while (argv[i] != 0)
		{
			fprintf(stderr,"%s\n",argv[i]);
			i = i + 1;
		}
		fprintf(stderr,"\n");
	}
	else if (strncmp(buf,"[ACCEPT",7) == 0)
	{
		fprintf(stderr,"matches accept\n");
	}
	else if (strncmp(buf,"[GG",3) == 0)
	{
		fprintf(stderr,"*************");
		for (i = 0; i < strlen(argv[0]); i++)
		{
			fprintf(stderr,"*");
		}
		fprintf(stderr,"********\n");
		fprintf(stderr,"GAME OVER!!! %s WINS!!!\n",argv[0]);
		fprintf(stderr,"*************");
		for (i = 0; i < strlen(argv[0]); i++)
		{
			fprintf(stderr,"*");
		}
		fprintf(stderr,"********\n");
		currenthand->hand.erase(currenthand->hand.begin(),currenthand->hand.end());
	}
	else if (strncmp(buf,"[WAIT",5) == 0)
	{
		fprintf(stderr,"Wait: %s\n",argv[0]);
	}
	free(argv);
	
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void read_message(int socket, char* buf,myhand* currenthand, char* name,bool automatic)
{
	char* begin_message = buf;
	int pos = 0;
	bool sent;

		while (buf[pos] != 0)
		{
			if (buf[pos] == '\n')
			{
				pos = pos + 1;
			}
			begin_message = &buf[pos];
			sent = false;																
			while ((buf[pos] != 0) && not sent)	
			{
				if (buf[pos] == ']')
				{
					buf[pos] = 0;
					server_message(socket,begin_message,currenthand,name,automatic);
					sent = true;
				}
				pos = pos + 1;
			}
		}
}
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void player_command(int numchar, int socket, char* buf,myhand* currenthand)
{
	char sender[300];
	int rc;
	char* message;
	if (buf[numchar-1] == '\n'){
	    buf[numchar-1] = 0;}
	    
	if (strncmp(buf,"chat ",5) == 0)
	{
		//fprintf(stderr,"You wanna chat '%s'\n",buf);
		message = &buf[5];
		strcpy(sender,"[CHAT|");
		strcat(sender,message);
		strcat(sender,"]");
		send(socket,sender,strlen(sender),0);
	}
	else if (strncmp(buf,"play ",5) == 0)
	{	
		message = &buf[5];
		message[2] = 0;
		//fprintf(stderr,"You wanna play card '%s'\n",message);
		if (currenthand->playcard(card(message[1],message[0])))
		{			
			
			if (message[0] == 'N')
			{
				fprintf(stderr,"What color would you like that to be? ");
				message[0] = getchar();
			}
			currenthand->last_played = card(message[1],message[0]);
			strcpy(sender,"[PLAY|");
			strcat(sender,message);
			strcat(sender,"]");
			rc = send(socket,sender,strlen(sender),0);
			//send card message
		}
		else if (message[0] == 'N' and message [1] == 'N')
		{
			strcpy(sender,"[PLAY|");
			strcat(sender,message);
			strcat(sender,"]");
			rc = send(socket,sender,strlen(sender),0);
			fprintf(stderr,"sent message \'%s\' to the server\n",sender);
		}
		else
		{
			fprintf(stderr,"You dont have that card in your hand\n");
		}
		
	}
	else if (strncmp(buf,"show hand",9) == 0)
	{
		currenthand->display();
	}
	else if (strncmp(buf,"quit",4) == 0)
	{
		exit(0);
	}
	else
	{
		fprintf(stderr,"Your options are: 'play [card]', 'chat [msg]', 'show hand', or 'quit'\n");
	}
}
