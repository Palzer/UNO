//clientdata.h
#pragma once
#include "deck.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>

void join(int socket, char* name);
char** parse_socket(int socket, char* line);
void server_message(int socket, char* buf,myhand* currenthand, char* name,bool automatic);
void read_message(int socket, char* buf,myhand* currenthand, char* name,bool automatic);
void player_command(int numchar, int socket, char* buf, myhand* currenthand);
