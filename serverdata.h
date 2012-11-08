//clientdata.h
#pragma once
#include "deck.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>

class client_data{

	public:
		
		client_data();
		
		client_data(int sock, char* string, bool spec);
		
		int display();
		
		int 	sd;
		char 	name[9];
		bool 	spec;
		//vector <card> hand;
		
		
	private: //can change public to private if i add appropriate functions
	
};

class hand{

	public:
	
	
	private:
	
	
};

bool arg_parse(char* line, char* command, char* args, int len);
void player_commands(std::vector <client_data>* client_vec, int socket, char* command, char* args, int max_players);
void display_players(std::vector <client_data>* client_vec);
void send_to_players(std::vector <client_data>* client_vec, char* buf);
bool player_is_in_lobby(std::vector <client_data>* client_vec, int socket);
void start_game(std::vector <client_data>* client_vec);
void deal(std::vector <client_data>* client_vec, deck* uno_deck);
