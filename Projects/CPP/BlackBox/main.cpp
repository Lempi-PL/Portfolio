
#include "GameSession.h"
#include "CrossFunctions.h"
#include <iostream>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif
void welcome_screen()
{
	short vertical = (cross_get_length() - 17) / 2;
	for (int i = 0; i < vertical; ++i)
		std::cout << "\n";

	std::string welcome_sc = R"(
                            ______ __              __     ______
                           |   __ \  |.---.-.----.|  |--.|   __ \.-----.--.--.
                           |   __ <  ||  _  |  __||    < |   __ <|  _  |_   _|
                           |______/__||___._|____||__|__||______/|_____|__.__|


                               _______         __   __
                              |   _   |.--.--.|  |_|  |--.-----.----.
                              |       ||  |  ||   _|     |  _  |   _|
                              |___|___||_____||____|__|__|_____|__|


                         _____                  __     __ _____                                    __
                         |     \.---.-.--.--.--.|__|.--|  |     |_.-----.--------.-----.---.-.----.|  |_
                         |  --  |  _  |  |  |  ||  ||  _  |       |  -__|        |  _  |  _  |   _||   _|
                         |_____/|___._|________||__||_____|_______|_____|__|__|__|   __|___._|__|  |____|
                                                                                 |__|
)";
	fwrite(welcome_sc.data(), 1, welcome_sc.size(), stdout);
	std::this_thread::sleep_for(std::chrono::seconds(3)); //wait 3 sec.


}
void menu_screen()
{

	std::cout << "\033[H\033[2J\033[3J"; //clear screen

	short horizontal_padding = (cross_get_width() - 17) / 2;
	short vertical_padding = (cross_get_length() - 5) / 2;
	for (int i = 0; i < vertical_padding; ++i)
		std::cout << "\n";
	std::string s;
	s.reserve(3 * (5 * (horizontal_padding + 29)));
	s.append(horizontal_padding, ' ');
	s += u8"Select a game mode:\n";
	s.append(horizontal_padding, ' ');
	s += u8"1 - 5 x 5 board and 3 atoms\n";
	s.append(horizontal_padding, ' ');
	s += u8"2 - 8 x 8 board and 5 atoms\n";
	s.append(horizontal_padding, ' ');
	s += u8"3 - 10 x 10 board and 8 atoms\n";
	s.append(horizontal_padding, ' ');
	s += u8"q - quit the game\n";

	fwrite(s.c_str(), sizeof(char), s.size(), stdout);
}
void goodbye_screen()
{
	short padding = (cross_get_length() - 4) / 2;

	std::cout << "\033[H\033[2J\033[3J"; //clear screen
	for (int i = 0; i < padding; ++i)
		std::cout << "\n";
	std::string goodbye_sc = R"(
                       _______ __                 __           ___              _______
                      |_     _|  |--.---.-.-----.|  |--.-----.'  _|.-----.----.|     __|.---.-.--------.-----.
                        |   | |     |  _  |     ||    <|__ --|   _||  _  |   _||    |  ||  _  |        |  -__|
                        |___| |__|__|___._|__|__||__|__|_____|__|  |_____|__|  |_______||___._|__|__|__|_____|)";
	size_t written = fwrite(goodbye_sc.c_str(), 1, goodbye_sc.size(), stdout);
	for (int i = 0; i < padding; ++i)
		std::cout << "\n";
	std::this_thread::sleep_for(std::chrono::seconds(3)); //wait 3 sec.
}

void gameplay(GameSession& session)
{
	bool quit = false;
	while (!quit)
	{
		unsigned char option = static_cast<unsigned char>(cross_getch());
		option = std::tolower(option);
		switch (option)
		{
		case 'a':
			session.move_left();
			break;
		case 'w':
			session.move_up();
			break;
		case 's':
			session.move_down();
			break;
		case 'd':
			session.move_right();
			break;
		case ' ':
			session.shoot();
			break;
		case 'e':
			session.mark_atom();
			break;
		case 'z':
			session.unmark_atom();
			break;
		case 'x':
			session.undo_shot();
			break;
		case 'q':
			quit = true;
			break;
		default:
			session.error_message();
			break;
		}
	}
}

int main()
{

#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8); // setting utf-8 encoding
#endif // win_32

	bool exit = false;
	char choice = '\0';
	welcome_screen();

	while (!exit)
	{
		//start screen
		menu_screen();

		choice = cross_getch();

		switch (choice)
		{
		case '1':
		{
			GameSession session(7, 3);
			gameplay(session);
			break;
		}

		case '2':
		{
			GameSession session(10, 5);
			gameplay(session);
			break;
		}

		case '3':
		{
			GameSession session(12, 8);
			gameplay(session);
			break;
		}

		case 'q':
		{
			exit = true;
			break;
		}

		case 'Q':
		{
			exit = true;
			break;
		}

		default:
		{
			std::cout << "Bad input data" << std::endl;
			break;
		}
		}
	}

	goodbye_screen();

	return 0;
}