//deck.h
#pragma once
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <functional>
#include <ctime>
#include <cstdlib>

class card{
	
	public:
		
		card();
		card(char t, char c);
		
		
		char 	type;
		char 	color;
		
	private:
	
	//char 	type;
	//char 	color;
};

class deck{

	public:
	
	//may want functions for draw, discard, whats on top of each, shuffle...
	deck();
	void display();
	void shuffle();
	void discard(card card);
	card draw();
	bool isempty();
	
	private:
	
	//may want a vector of cards here for the draw and discard piles
	std::vector<card> draw_pile;
	std::vector<card> discard_pile;

};
