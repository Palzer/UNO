//deck.h

class card{
	
	public:
		
		card();
		card(char t, char c);
		
	private:
	
	char 	type;
	char 	color;
};

class deck{

	public:
	
	//may want functions for draw, discard, whats on top of each, shuffle...
	deck();
	
	private:
	
	//may want a vector of cards here for the draw and discard piles
	vector<card> draw_pile;
	vector<card> discard_pile;

};
