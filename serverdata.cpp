#include "serverdata.h"


using namespace std;

client_data::client_data()
{
		
			sd = 0;
			strcpy(name,"");
			spec = false;

}
		
client_data::client_data(int sock, char* string, bool spectator)
{
		
			sd = sock;
			strcpy (name, string);
			spec = spectator;
			
}

int client_data::display()
{

flush(cout);

cout << "Client Name: " << name << " Socket: " << sd << endl;

}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
bool arg_parse(char* line, char* command, char* args, int len)
{
	int 	x = 0;
	int 	k = -1;
	bool	bar = false;
	bool	fb = false;
	bool	bb = false;
	bool	valid = true;
	strcpy(command,"");
	strcpy(args,"");
	for (int j = 0; j < len-1; j++){
		if (not fb)
		{
			if (line[j] == '[') 
			{
				fb = true;
				//fprintf(stdout,"found front bracket\n");
			}
		}
		else if ((line[j] == '|') && (not bar)){
			bar = true;
		}
		else if (not bar){
			command[x] = line[j];
			x = x + 1;
		}
		else if (not bb){
			
			//fprintf(stdout,"moving %c to %i in command\n",line[j],k);
			k = k + 1;
			args[k] = line[j];
			if (line[j] == ']') 
			{
				bb = true;
				//fprintf(stdout,"found back bracket\n");
			}
			
		}
	}
	args[k] = 0;
	command[x] = 0;
	fprintf(stdout,"command is %s, arg is %s\n",command, args);
	if (not bar || not fb || not bb)
	{
		return false;
	}
	else
	{
		return valid;
	}
    //fprintf(stdout,"command is %s and args are %s\n",command,args);
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void player_commands(vector <client_data>* client_vec, int socket, char* command, char* args, int max_players)
{
	int 	rc;
	char 	buf[200];
	char 	sender[300] = "[";
	bool	already_in = false;
	bool	samename = false;
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
		{
			if (itr->sd == socket)
			{
				if (itr->spec)
				{
					send(itr->sd,"[INVALID|You are a spectator. You cannot use commands]\n",strlen("[INVALID|You are a spectator. You cannot use commands]\n"),0);
					return;
				}
			}
		}
	if (strcmp(command,"JOIN") == 0)
	{
		fprintf(stdout,"Player wants to join with name \"%s\"\n",args);
		
		for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
		{
			if (itr->sd == socket)
			{
				rc = send(socket, "[INVALID|You are already in a game]\n", strlen("[INVALID|You are already in a game]\n"), 0);
				already_in = true;
			}
			if (strcmp(itr->name,args) == 0)
			{
				samename = true;
			}
		}
		if (not already_in)
		{
			if (client_vec->size() < max_players)
			{	
				
				if (samename)
				{
					rc = send(socket, "[INVALID|Someone already has that name. Try another one]\n", strlen("[INVALID|Someone already has that name. Try another one]\n"), 0);
				}
				else
				{	
					strcat(sender,"ACCEPT|");
					client_data client = client_data(socket,args,false);
					strcat(sender,args);
					client_vec->push_back(client);
					client.display();
					strcat(sender,"]\n");				
					rc = send(socket,sender,strlen(sender),0);
					display_players(client_vec);
				}				
			}
			else
			{	
				strcat(sender,"WAIT|");
				client_data client = client_data(socket,args,true);
				strcat(sender,args);
				client_vec->push_back(client);
				client.display();
				strcat(sender,"]\n");				
				rc = send(socket,sender,strlen(sender),0);
			}
		}
	}
	else if (strcmp(command,"CHAT") == 0)
	{	
		strcat(sender,"CHAT|");
		for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
		{
			if (itr->sd == socket)
			{
				strcat(sender,itr->name);
			}
		}
		strncpy(buf,args,200);
		//fprintf(stdout,"Player wants to chat with message \"%s\"\n",buf);
		//fprintf(stdout,"buf is %d chars long\n",strlen(buf));
		buf[strlen(buf)] = 0;
		//fprintf(stdout,"Player wants to chat with message \"%s\"\n",buf);
		//fprintf(stdout,"buf is %d chars long\n",strlen(buf));
		strcat(sender,",");
		strncat(sender,buf,strlen(buf)+1);
		strcat(sender,"]");
		strcat(sender,"\n");
		for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
		{
			if (itr->sd != socket)
			{
				rc = send(itr->sd, sender, strlen(sender), 0);
			}
			
		}
	}
	else if (strcmp(command,"PLAY") == 0)
	{
		fprintf(stdout,"Player wants to play with card \"%s\"\n",args);
	}	
	else
	{
		rc = send(socket,"[INVALID|That is not a valid command]\n", strlen("[INVALID|That is not a valid command]\n"),0);
		//fprintf(stdout,"That is not a valid command\n",);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void display_players(vector <client_data>* client_vec)
{
	char buf[200];
	int rc;
	
	sprintf(buf,"[PLAYERS|");
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
	{
		if (itr != client_vec->begin())
		{
			strcat(buf, ",");
		}
		if (itr->spec == false)
		{
			strcat(buf,itr->name);
		}
		
	}
	strcat(buf,"]\n");
	send_to_players(client_vec,buf);
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void send_to_players(vector <client_data>* client_vec, char* buf)
{
	int rc;
	
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
	{
		rc = send(itr->sd, buf, strlen(buf), 0);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
bool player_is_in_lobby(vector <client_data>* client_vec, int socket)
{
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
	{
		if (itr->sd == socket)
		{
			return true;
		}
	}
	
	return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void start_game(vector <client_data>* client_vec)
{
	char buf[300];
	int rc;
	
	sprintf(buf,"[STARTGAME|");
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
	{
		if (itr != client_vec->begin())
		{
			strcat(buf, ",");
		}
		if (not itr->spec)
		{
			strcat(buf,itr->name);
		}
	}
	strcat(buf,"]\n");
	send_to_players(client_vec,buf);

}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void deal(vector <client_data>* client_vec, deck* uno_deck)
{	
	char buf[300];
	char ct[2] = " ";
	card card_for_player;
	uno_deck->shuffle();
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
	{	
		if (not itr->spec)
		{
			sprintf(buf,"[DEAL|");
			for (int i = 0; i < 7; i++)
			{
				if (i != 0)
				{
					strcat(buf,", ");
				}
				card_for_player = uno_deck->draw();
				ct[0] = card_for_player.color;
				strcat(buf,ct);
				ct[0] = card_for_player.type;
				strcat(buf,ct);
			}
			strcat(buf,"]\n");
			send(itr->sd,buf,strlen(buf),0);
		}
	}
}
