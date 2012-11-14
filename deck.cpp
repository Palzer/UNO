#include "deck.h"

using namespace std;
card::card()
{
	color = 'B';
	type = '0';
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
card::card(char t, char c)
{	
	if ((t>='0' && t <= '9') || t == 'D' || t == 'S' || t == 'R' || t == 'W' || t == 'F' || t == 'N' || t == 'U')
	{
		type = t;
	}
	else
	{
		fprintf(stdout,"that is not a valid card type\n");
	}
	
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
	forward = true;
	for (int i = 1; i <= 4; i++)
	{	
		if (i == 1) color = 'B';
		else if (i == 2) color = 'R';
		else if (i == 3) color = 'Y';
		else if (i == 4) color = 'G';
		draw_pile.push_back(card('0',color));
		for (char j = '1'; j <= '9'; j++)
		{
			draw_pile.push_back(card(j,color));
			draw_pile.push_back(card(j,color));
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
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void deck::display()
{
	for (vector <card>::iterator itr = draw_pile.begin(); itr != draw_pile.end(); ++itr)
	{
		cout << itr->type << itr->color << " ";
	}
	cout << endl;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void deck::shuffle()
{		
	for (vector <card>::iterator itr = discard_pile.begin(); itr != discard_pile.end(); ++itr)
	{
		if(itr->type == 'F' || itr->type == 'W')
		{
			itr->color = 'N';
		}
	}
	if (discard_pile.size() > 0)
	{
		draw_pile.insert(draw_pile.end(), discard_pile.begin(), discard_pile.end()-1);
		discard_pile.erase(discard_pile.begin(),discard_pile.end()-1);
	}
	random_shuffle (draw_pile.begin(), draw_pile.end());
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void deck::discard(card card)
{
	fprintf(stderr,"discarded %c%c\n",card.color,card.type);
	discard_pile.push_back(card);
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
card deck::draw()
{
		card thiscard = draw_pile[0];
		draw_pile.erase(draw_pile.begin());
		if (draw_pile.size() == 0)
		{
			shuffle();
		}
		return thiscard;	
}	
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
bool deck::isempty()
{
	if (draw_pile.size() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////	
card deck::show_top()
{
	fprintf(stderr,"top card is %c%c\n",discard_pile.back().color,discard_pile.back().type);
	return card(discard_pile.back().type,discard_pile.back().color);
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////	
myhand::myhand(){}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
bool myhand::isempty()
{
	if (hand.size() < 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
bool myhand::playcard(card thiscard)
{
	if (not isempty())
	{
		for (vector <card>::iterator itr = hand.begin(); itr != hand.end(); ++itr)
		{
			if ((itr->color == thiscard.color || itr->color == 'N') && itr->type == thiscard.type)
			{
				return true;
			}
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void myhand::display()
{
	fprintf(stderr,"Your current hand is:\n");
	for (vector <card>::iterator itr = hand.begin(); itr != hand.end(); ++itr)
	{
		fprintf(stderr,"%c%c ",itr->color,itr->type);
	}
	fprintf(stderr,"\n");
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void myhand::addcard(card card)
{
	hand.push_back(card);
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void myhand::discard(card thiscard)
{	
	if (not isempty())
	{
		for (vector <card>::iterator itr = hand.begin(); itr != hand.end(); ++itr)
		{
			if (itr->color == thiscard.color && itr->type == thiscard.type)
			{
				hand.erase(itr);
				break;
			}
		}
	}
}
	
