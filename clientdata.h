//clientdata.h

class client_data{

	public:
		
		client_data();
		
		client_data(int sock, char* string);
		
		int display();
		
		int 	sd;
		char 	name[9];
		//vector <card> hand;
		
		
	private: //can change public to private if i add appropriate functions
	
};
