#include "clientdata.h"
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;

client_data::client_data()
{
		
			sd = 0;
			strcpy(name,"");

}
		
client_data::client_data(int sock, char* string)
{
		
			sd = sock;
			strcpy (name, string);
			
}

int client_data::display()
{

flush(cout);

cout << "Client Name: " << name << " Socket: " << sd << endl;

}
