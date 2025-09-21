#pragma once
#include <vector>
#include <string> 

//GameSession
short cross_get_width();

class GameSession
{

	//all functions for user
public:

	//construcotr
	GameSession(short size_board, short number_of_atoms);
	// - shuffle atoms
	// - create board (user and atoms)
	// - display game window


	// movement
	void move_up();
	void move_down();
	void move_left();
	void move_right();


	// shoot
	void shoot();

	// mark atoms
	void mark_atom();

	// unmark atoms
	void unmark_atom();

	// undo shots
	void undo_shot();

	//invalid input error message
	void error_message();

	// destructor:
	~GameSession();
	// - show number of hits

	// functions and variables hidden from a user
private:
	std::vector < std::vector<char> > board; // (board)
	std::vector < std::vector<char> > at_board; // atom board 
	std::vector < std::vector<short> > atoms; //coordinates of atoms (x1, y1 ... xn, yn)

	std::vector < std::vector<short> > marked_atoms; // coordinates to uncheck atoms
	std::vector < std::vector<short> > shots_memory; // coordinates to uncheck shots

	std::vector <short> nr_of_fields; // H,R-1, misses-2
	std::vector <short> cursor; // (x, y) [@]
	std::vector <short> prev_coord_char;

	std::string message;
	char prev_char;

	const short number_of_atoms;
	const short size_board;

	short counter_shot;
	short miss_counter;
	short windw_length;

	void shuffle_atoms();
	void create_board();
	void display_game_window(bool hide_cursor);

	void add_padding(short padding, std::string& game_window);
	void add_space(short padding, std::string& game_window);
	void add_board(short padding, std::string& game_window);
	void add_separator(short padding, std::string& game_window);
	void add_message(short padding, std::string& game_window);


	void up_bottom_check(short& y_laser, short& x_laser);
	void bottom_up_check(short& y_laser, short& x_laser);
	void left_right_check(short& y_laser, short& x_laser);
	void right_left_check(short& y_laser, short& x_laser);
};