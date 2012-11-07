#include <stdlib.h>
#include "deck.h"
#include <stdio.h>
#include <iostream>
#include <string.h>



card::card()
{
	color = 'B';
	type = '0';
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
card::card(char t, char c)
{	
	if ((t>='0' && t <= '9') || t == 'D' || t == 'S' || t == 'R' || t == 'W' || t == 'F')
	{
		type = t;
	}
	else
	{
		fprintf(stdout,"that is not a valid card type\n");
	}
	color = c;
	
	if (c == 'R' || c == 'G' || c == 'B' || c == 'Y' || c == 'N')
	{
		color = c;
	}
	else
	{
		fprintf(stdout,"that is not a valid card color\n");
	}
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
deck::deck()
{	
	card new_card;
	char color;
	for (int i = 1; i <= 4; i++)
	{	
		if (i == 1) color = 'B';
		else if (i == 2) color = 'R';
		else if (i == 3) color = 'Y';
		else if (i == 4) color = 'G';
		draw_pile.push_back(card('0',color));
		for (int j = 1; j <= 9; j++)
		{
			draw_pile.push_back(card(itoa(j),color));
			draw_pile.push_back(card(itoa(j),color));
		}
		draw_pile.push_back(card('D',color));
		draw_pile.push_back(card('D',color));
		draw_pile.push_back(card('S',color));
		draw_pile.push_back(card('S',color));
		draw_pile.push_back(card('R',color));
		draw_pile.push_back(card('R',color));
		draw_pile.push_back(card('W','N'));
		draw_pile.push_back(card('F','N'));
	}
}
