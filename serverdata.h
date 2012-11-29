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
		bool	turn;
		myhand clienthand;
		
		
	private: //can change public to private if i add appropriate functions
	
};

bool arg_parse(char* line, char* command, char* args, int len);
bool player_commands(std::vector <client_data>* client_vec, int socket, char* command, char* args, int max_players,bool playing,deck* uno_deck,int* numplayers);
void display_players(std::vector <client_data>* client_vec);
void send_to_players(std::vector <client_data>* client_vec, char* buf);
bool player_is_in_lobby(std::vector <client_data>* client_vec, int socket);
void start_game(std::vector <client_data>* client_vec);
void deal(std::vector <client_data>* client_vec, deck* uno_deck);
void playgame(std::vector <client_data>* client_vec,int* max_sd,int server_sd,int max_players,deck* uno_deck,int* numplayers);
void set_turn(std::vector <client_data>* client_vec,deck* uno_deck);
void send_go(std::vector <client_data>* client_vec,deck* uno_deck);
void draw_card(std::vector <client_data>* client_vec,deck* uno_deck,int num_to_draw);
