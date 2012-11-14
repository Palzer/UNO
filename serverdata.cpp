#include "serverdata.h"


using namespace std;

client_data::client_data()
{
		
			sd = 0;
			strcpy(name,"");
			spec = false;
			turn = false;

}
		
client_data::client_data(int sock, char* string, bool spectator)
{
		
			sd = sock;
			strcpy (name, string);
			spec = spectator;
			turn = false;
			
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
	for (int j = 0; j < len; j++){
		if (not fb)
		{
			if (line[j] == '[') 
			{	
				//fprintf(stderr,"found front bar\n");
				fb = true;
				//fprintf(stdout,"found front bracket\n");
			}
		}
		else if ((line[j] == '|') && (not bar)){
			//fprintf(stderr,"found bar\n");
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
				//fprintf(stderr,"found back bar\n");
				bb = true;
				//fprintf(stdout,"found back bracket\n");
			}
			
		}
	}
	args[k] = 0;
	command[x] = 0;
	//fprintf(stdout,"command is %s, arg is %s\n",command, args);
	if (not bar || not fb || not bb)
	{
		//fprintf(stderr,"didnt find proper bars and brackets\n");
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
bool player_commands(vector <client_data>* client_vec, int socket, char* command, char* args, int max_players,bool playing,deck* uno_deck,int* numplayers)
{
	int 	rc;
	char	ct [2];
	ct[1] = 0;
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
					send(itr->sd,"[INVALID|You are a spectator. You cannot use commands]\n",strlen("[INVALID|You are a spectator. You cannot use commands]\n")+1,0);
					return true;
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
				rc = send(socket, "[INVALID|You are already in a game]\n", strlen("[INVALID|You are already in a game]\n")+1, 0);
				already_in = true;
			}
			if (strcmp(itr->name,args) == 0)
			{
				samename = true;
			}
		}
		if (not already_in)
		{
			if (client_vec->size() < max_players and not playing)
			{	
				
				if (samename)
				{
					rc = send(socket, "[INVALID|Someone already has that name. Try another one]\n", strlen("[INVALID|Someone already has that name. Try another one]\n")+1, 0);
				}
				else
				{	
					strcat(sender,"ACCEPT|");
					*numplayers = *numplayers + 1;
					fprintf(stderr,"We now have %i player(s)\n",*numplayers);
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
				strcat(sender,"]");				
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
		if (playing == true)
		{
			for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
			{
				if (itr->sd == socket)
				{
					if (itr->turn)
					{
						fprintf(stdout,"%s wants to play with card \"%s\"\n",itr->name,args);
						card last_card = uno_deck->show_top();
						if (args[1] == 'N' && args[0] == 'N')
						{
							last_card = uno_deck->draw();
							sprintf(sender,"[DEAL|");
							ct[0] = last_card.color;
							strcat(sender,ct);
							ct[0] = last_card.type;
							strcat(sender,ct);
							itr->clienthand.addcard(last_card);						
							strcat(sender,"]");			
							send(itr->sd,sender,strlen(sender),0);
							send_go(client_vec,uno_deck);
						}
						else if (args[1] == last_card.type || args[0] == last_card.color || args[1] == 'F' || args[1] == 'W')
						{
							if (itr->clienthand.playcard(card(args[1],args[0])))
							{
								if (args[1] == 'W' || args[1] == 'F')
								{
									itr->clienthand.discard(card(args[1],'N'));
								}
								else
								{
									itr->clienthand.discard(card(args[1],args[0]));
								}
								if (args[0] == 'N')	uno_deck->discard(card(args[1],'R'));
								else	uno_deck->discard(card(args[1],args[0]));
								
								fprintf(stderr,"%s's hand is now: ",itr->name);
								itr->clienthand.display();
								strcpy(sender,"[PLAYED|");
								strcat(sender,itr->name);
								strcat(sender,",");
								strcat(sender,args);
								strcat(sender,"]");
								send_to_players(client_vec,sender);
								if (itr->clienthand.isempty())
								{
									strcpy(sender,"[GG|");
									strcat(sender,itr->name);
									strcat(sender,"]");
									send_to_players(client_vec,sender);
									return false;
								}
								else if (itr->clienthand.hand.size() == 1)
								{
									strcpy(sender,"[UNO|");
									strcat(sender,itr->name);
									strcat(sender,"]");
									send_to_players(client_vec,sender);
								}
								if (args[1] == 'R') uno_deck->forward = not uno_deck->forward;
								set_turn(client_vec,uno_deck);
								if (args[1] == 'D') draw_card(client_vec,uno_deck,2);
								if (args[1] == 'F') draw_card(client_vec,uno_deck,4);								
								if (args[1] == 'S') set_turn(client_vec,uno_deck);
								send_go(client_vec,uno_deck);
							}
							else
							{
								rc = send(socket,"[INVALID|You do not have that card in your hand]",strlen("[INVALID|You do not have that card in your hand]"),0);
							}
						}
						else
						{
							rc = send(socket,"[INVALID|That is not a valid card to play]",strlen("[INVALID|That is not a valid card to play]"),0);
						}
					}
					else
					{
						rc = send(socket,"[INVALID|It's not your turn]",strlen("[INVALID|It's not your turn]"),0);
					}
				}
			}				
		}
		else
		{
			rc = send(socket,"[INVALID|You cannot play. You are still in the lobby]\n", strlen("[INVALID|You cannot play. You are still in the lobby]\n"),0);
		}
		
	}	
	else
	{
		rc = send(socket,"[INVALID|That is not a valid command]\n", strlen("[INVALID|That is not a valid command]\n"),0);
		//fprintf(stdout,"That is not a valid command\n",);
	}
	return true;
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
	bool turnset = false;
	card card_for_player;
	uno_deck->shuffle();
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
	{	
		if (not itr->spec)
		{
			if (not turnset)
			{
				itr->turn = true;
				turnset = true; 
			}
			sprintf(buf,"[DEAL|");
			for (int i = 0; i < 7; i++)
			{
				if (i != 0)
				{
					strcat(buf,",");
				}
				card_for_player = uno_deck->draw();
				ct[0] = card_for_player.color;
				strcat(buf,ct);
				ct[0] = card_for_player.type;
				strcat(buf,ct);
				itr->clienthand.addcard(card_for_player);
			}
			strcat(buf,"]\n");			
			send(itr->sd,buf,strlen(buf),0);
		}
		fprintf(stderr,"%s's hand is: ",itr->name);
		itr->clienthand.display();
	}
	
	card_for_player = uno_deck->draw();
	//fprintf(stderr,"discarding top card and it is %c%c\n",card_for_player.color,card_for_player.type);
	if (card_for_player.color == 'N')
	{
		card_for_player.color = 'R'; //																					make this random
	}
	uno_deck->discard(card_for_player);
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void set_turn(vector <client_data>* client_vec,deck* uno_deck)
{
	bool found = false;
	bool set = false;
	
	if (uno_deck->forward == true)
	{
		for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
		{
			if (itr->spec == false && found)
			{
				fprintf(stderr,"now it is %s's turn\n",itr->name);
				itr->turn = true;
				set = true;
				found = false;
			}
			if (itr->turn == true && not set)
			{
				//fprintf(stderr,"it was just %s's turn\n",itr->name);
				found = true;
				itr->turn = false;
			}
		}
		if (not set)
		{
			for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
			{
				if (itr->spec == false && found)
				{
					fprintf(stderr,"now it is %s's turn\n",itr->name);
					itr->turn = true;
					set = true;
					found = false;
				}

			}
		}
	}
	else
	{
		
		for (vector <client_data>::reverse_iterator itr = client_vec->rbegin(); itr != client_vec->rend(); ++itr)
		{
			if (itr->spec == false && found)
			{
				fprintf(stderr,"now it is %s's turn\n",itr->name);
				itr->turn = true;
				set = true;
				found = false;
			}
			if (itr->turn == true && not set)
			{
				//fprintf(stderr,"it was just %s's turn\n",itr->name);
				found = true;
				itr->turn = false;
			}
		}
		if (not set)
		{
			for (vector <client_data>::reverse_iterator itr = client_vec->rbegin(); itr != client_vec->rend(); ++itr)
			{
				if (itr->spec == false && found)
				{
					fprintf(stderr,"now it is %s's turn\n",itr->name);
					
					itr->turn = true;
					set = true;
					found = false;
				}

			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void send_go(vector <client_data>* client_vec,deck* uno_deck)
{
	char ct [2];
	ct [1] = 0;
	char msg[300];
	int rc;
	card last_card = uno_deck->show_top();
	
	strcpy(msg,"[GO|");
	ct[0] = last_card.color;
	strcat(msg,ct);
	ct[0] = last_card.type;
	strcat(msg,ct);
	strcat(msg,"]");
	
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
		{
			if (itr->turn)
			{
				rc = send(itr->sd,msg,strlen(msg),0);
			}
		}
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void draw_card(std::vector <client_data>* client_vec,deck* uno_deck,int num_to_draw)
{
	char buf[300];
	char ct[2] = " ";
	card card_for_player;
	
	for (vector <client_data>::iterator itr = client_vec->begin(); itr != client_vec->end(); ++itr)
	{	
		if (not itr->spec && itr->turn)
		{
			sprintf(buf,"[DEAL|");
			for (int i = 0; i < num_to_draw; i++)
			{
				if (i != 0)
				{
					strcat(buf,",");
				}
				card_for_player = uno_deck->draw();
				ct[0] = card_for_player.color;
				strcat(buf,ct);
				ct[0] = card_for_player.type;
				strcat(buf,ct);
				itr->clienthand.addcard(card_for_player);
			}
			strcat(buf,"]");			
			send(itr->sd,buf,strlen(buf),0);
		}
	}
	
	set_turn(client_vec,uno_deck);
}
