#include "GameSession.h"
#include "CrossFunctions.h"
#include <iostream>
#include <random>
#include <cstdio>
#include <chrono>
#include <thread>

GameSession::GameSession(short size_board, short nr_of_atoms) : size_board(size_board), number_of_atoms(nr_of_atoms),
board(size_board, std::vector<char>(size_board, '/0')), at_board(size_board, std::vector<char>(size_board, '*')),
cursor({ 0,0 }), prev_coord_char({ 0,0 }), message("Message Window"), prev_char('+'), miss_counter(0), counter_shot(0)
{
    shuffle_atoms();
    create_board();
    display_game_window(false);
}

void GameSession::shuffle_atoms()
{
    short i = 0;

    std::random_device rd; // creating random device seed
    std::mt19937 eng(rd()); //initializing engine Mersenne Twister with random device seed
    std::uniform_int_distribution <short> disr(1, size_board - 2); //setting distributions range

    while (i < number_of_atoms)
    {
        short y = 0, x = 0;
        y = disr(eng);
        x = disr(eng);
        if (atoms.empty() == false)
        {
            bool repeat = false;
            for (int j = 0; j < atoms.size(); ++j)
            {
                if (atoms[j][0] == y && atoms[j][1] == x)
                {
                    repeat = true;
                    j = atoms.size();
                }
            }
            if (!repeat)
            {
                atoms.push_back({ y,x });
                ++i;
            }
        }
        else
        {
            atoms.push_back({ y, x });
            ++i;
        }
    }
}

void GameSession::create_board()
{
    for (int i = 1; i < size_board - 1; ++i)
    {
        board[i][0] = '#';
        board[0][i] = '#';
        board[i][size_board - 1] = '#';
        board[size_board - 1][i] = '#';
    }

    for (int i = 1; i < size_board - 1; ++i)
    {
        for (int j = 1; j < size_board - 1; ++j)
        {
            board[i][j] = '*';
        }
    }

    board[0][0] = board[0][size_board - 1] = board[size_board - 1][0] = board[size_board - 1][size_board - 1] = '+';


    for (int i = 0; i < atoms.size(); ++i)
    {
        at_board[atoms[i][0]][atoms[i][1]] = 'a';
    }

}


void GameSession::display_game_window(bool hide_cursor)
{
    // place old character replaced by a cursor to board
    board[prev_coord_char[0]][prev_coord_char[1]] = prev_char;

    //save replacing character
    prev_char = board[cursor[0]][cursor[1]];
    prev_coord_char[0] = cursor[0];
    prev_coord_char[1] = cursor[1];

    //set cursor
    if (!hide_cursor)
    {
        board[cursor[0]][cursor[1]] = '@';
    }

    //clear screen
    std::cout << "\033[H\033[2J\033[3J";


    short horizontalpadding = cross_get_width() / 2 - 25;
    short windw_length = 14 + size_board;
    short verticalpadding = (cross_get_length() - windw_length) / 2;

    std::string game_window;
    game_window.reserve((horizontalpadding + 150) * (windw_length + 2));

    for (int i = 0; i < verticalpadding - 1; ++i)
        std::cout << "\n";
    add_padding(horizontalpadding, game_window);
    game_window += u8"╔";
    for (int i = 0; i < 48; ++i)
    {
        game_window += u8"═";
    }
    game_window += u8"╗";

    add_space(horizontalpadding, game_window);
    add_board(horizontalpadding, game_window);
    add_space(horizontalpadding, game_window);
    add_separator(horizontalpadding, game_window);
    // counters
    add_padding(horizontalpadding, game_window);
    game_window += u8"║    atoms marked: ";
    game_window.push_back('0' + marked_atoms.size());
    game_window += R"( / )";
    game_window.push_back('0' + number_of_atoms);
    game_window += "    shot counter: ";
    if (counter_shot < 10)
    {
        game_window.push_back('0' + counter_shot);
        game_window += u8"      ║";
    }
    else
    {
        game_window.push_back('0' + counter_shot / 10);
        game_window.push_back('0' + counter_shot % 10);
        game_window += u8"     ║";
    }


    //message
    add_separator(horizontalpadding, game_window);
    add_message(horizontalpadding, game_window);
    add_separator(horizontalpadding, game_window);
    //interface
    add_padding(horizontalpadding, game_window);
    game_window += u8"║    a, w, s, d - movement      q - quit game    ║";
    add_padding(horizontalpadding, game_window);
    game_window += u8"║            e - mark atom      z - unmark atom  ║";
    add_padding(horizontalpadding, game_window);
    game_window += u8"║            space - shoot      x - undo shot    ║";
    add_padding(horizontalpadding, game_window);
    game_window += u8"╚";
    for (int i = 0; i < 48; ++i)
    {
        game_window += u8"═";
    }
    game_window += u8"╝";
    game_window.push_back('\n');
    fwrite(game_window.data(), 1, game_window.size(), stdout);
}

void GameSession::add_padding(short padding, std::string& game_window)
{
    game_window.push_back('\n');
    game_window.append(padding, ' ');
}

void GameSession::add_space(short padding, std::string& game_window)
{
    for (int i = 0; i < 2; ++i)
    {
        add_padding(padding, game_window);
        game_window += u8"║";
        game_window.append(48, ' ');
        game_window += u8"║";
    }

}

void GameSession::add_board(short padding, std::string& game_window)
{
    short left_distance = 24 - ((size_board + (size_board - 1) * 2) / 2);
    short right_distance = 0;

    if (size_board % 2 == 0)
    {
        right_distance = left_distance;
    }
    else
    {
        right_distance = left_distance - 1;
    }

    for (int i = 0; i < size_board; ++i)
    {
        add_padding(padding, game_window);
        game_window += u8"║";
        game_window.append(left_distance, ' ');
        for (int k = 0; k < size_board; ++k)
        {
            game_window.push_back(board[i][k]);
            game_window += "  ";
        }
        game_window.append(right_distance - 2, ' ');
        game_window += u8"║";
    }
}

void GameSession::add_separator(short padding, std::string& game_window)
{
    add_padding(padding, game_window);
    game_window += u8"╠";
    for (int i = 0; i < 48; ++i)
    {
        game_window += u8"═";
    }
    game_window += u8"╣";
}

void GameSession::add_message(short padding, std::string& game_window)
{
    short left_distance = (48 - message.length()) / 2;
    short right_distance = 0;
    if (message.length() % 2 == 0)
    {
        right_distance = left_distance;
    }
    else
    {
        right_distance = left_distance + 1;
    }
    add_padding(padding, game_window);
    game_window += u8"║";
    game_window.append(left_distance, ' ');
    game_window += message;
    game_window.append(right_distance, ' ');
    game_window += u8"║";
}

void GameSession::error_message()
{
    message = "invalid input data";
    display_game_window(false);
    message = "Message Window";
}

void GameSession::move_up()
{
    if (cursor[0] > 0)
    {
        --cursor[0];
    }
    else
    {
        cursor[0] = size_board - 1;
    }

    display_game_window(false);
}

void GameSession::move_down()
{
    if (cursor[0] < size_board - 1)
    {
        ++cursor[0];
    }
    else
    {
        cursor[0] = 0;
    }

    display_game_window(false);
}

void GameSession::move_left()
{
    if (cursor[1] > 0)
    {
        --cursor[1];
    }
    else
    {
        cursor[1] = size_board - 1;
    }

    display_game_window(false);
}

void GameSession::move_right()
{
    if (cursor[1] < size_board - 1)
    {
        ++cursor[1];
    }
    else
    {
        cursor[1] = 0;
    }

    display_game_window(false);
}

void GameSession::up_bottom_check(short& y_laser, short& x_laser)
{
    bool event = false;

    while (!event && y_laser < size_board - 2)
    {
        if (at_board[y_laser + 1][x_laser] == 'a')
        {
            prev_char = 'H';
            nr_of_fields.push_back(1);
            event = true;
        }

        else if (at_board[y_laser + 1][x_laser - 1] == 'a')
        {
            if (at_board[y_laser + 1][x_laser + 1] == 'a')
            {
                prev_char = 'R';
                nr_of_fields.push_back(1);
                event = true;
            }

            else
            {
                // left-right-check
                left_right_check(y_laser, x_laser);
                event = true;
            }
        }

        else if (at_board[y_laser + 1][x_laser + 1] == 'a')
        {
            // right-left-check
            right_left_check(y_laser, x_laser);
            event = true;
        }

        else
        {
            ++y_laser;
        }

    }

    if (!event)
    {
        y_laser += 1;

        shots_memory.push_back({ y_laser, x_laser });
        nr_of_fields.push_back(2);


        at_board[y_laser][x_laser] = 's';
        ++miss_counter;

        if (miss_counter < 10)
        {
            prev_char = '0' + miss_counter;
            board[y_laser][x_laser] = '0' + miss_counter;
        }
        else
        {
            prev_char = 90 + miss_counter;
            board[y_laser][x_laser] = 90 + miss_counter;
        }
    }
}

void GameSession::bottom_up_check(short& y_laser, short& x_laser)
{
    bool event = false;

    while (!event && y_laser > 1)
    {
        if (at_board[y_laser - 1][x_laser] == 'a')
        {
            prev_char = 'H';
            nr_of_fields.push_back(1);
            event = true;
        }

        else if (at_board[y_laser - 1][x_laser - 1] == 'a')
        {
            if (at_board[y_laser - 1][x_laser + 1] == 'a')
            {
                prev_char = 'R';
                nr_of_fields.push_back(1);
                event = true;
            }

            else
            {
                // left-right-check
                left_right_check(y_laser, x_laser);
                event = true;
            }
        }

        else if (at_board[y_laser - 1][x_laser + 1] == 'a')
        {
            // right-left-check
            right_left_check(y_laser, x_laser);
            event = true;
        }

        else
        {
            --y_laser;
        }

    }

    if (!event)
    {
        y_laser -= 1;

        shots_memory.push_back({ y_laser, x_laser });
        nr_of_fields.push_back(2);


        at_board[y_laser][x_laser] = 's';

        ++miss_counter;

        if (miss_counter < 10)
        {
            prev_char = '0' + miss_counter;
            board[y_laser][x_laser] = '0' + miss_counter;
        }
        else
        {
            prev_char = 90 + miss_counter;
            board[y_laser][x_laser] = 90 + miss_counter;
        }
    }
}

void GameSession::left_right_check(short& y_laser, short& x_laser)
{
    bool event = false;

    while (!event && x_laser < size_board - 2)
    {
        if (at_board[y_laser][x_laser + 1] == 'a')
        {
            prev_char = 'H';
            nr_of_fields.push_back(1);
            event = true;
        }

        else if (at_board[y_laser + 1][x_laser + 1] == 'a')
        {
            if (at_board[y_laser - 1][x_laser + 1] == 'a')
            {
                prev_char = 'R';
                nr_of_fields.push_back(1);
                event = true;
            }

            else
            {
                // bottom-up-check
                bottom_up_check(y_laser, x_laser);
                event = true;
            }
        }

        else if (at_board[y_laser - 1][x_laser + 1] == 'a')
        {
            // up-bottom-check
            up_bottom_check(y_laser, x_laser);
            event = true;
        }

        else
        {
            ++x_laser;
        }

    }

    if (!event)
    {
        x_laser += 1;

        shots_memory.push_back({ y_laser, x_laser });
        nr_of_fields.push_back(2);


        at_board[y_laser][x_laser] = 's';

        ++miss_counter;

        if (miss_counter < 10)
        {
            prev_char = '0' + miss_counter;
            board[y_laser][x_laser] = '0' + miss_counter;
        }
        else
        {
            prev_char = 90 + miss_counter;
            board[y_laser][x_laser] = 90 + miss_counter;
        }
    }
}

void GameSession::right_left_check(short& y_laser, short& x_laser)
{
    bool event = false;

    while (!event && x_laser > 1)
    {
        if (at_board[y_laser][x_laser - 1] == 'a')
        {
            prev_char = 'H';
            nr_of_fields.push_back(1);
            event = true;
        }

        else if (at_board[y_laser + 1][x_laser - 1] == 'a')
        {
            if (at_board[y_laser - 1][x_laser - 1] == 'a')
            {
                prev_char = 'R';
                nr_of_fields.push_back(1);
                event = true;
            }

            else
            {
                // bottom-up-check
                bottom_up_check(y_laser, x_laser);
                event = true;
            }
        }

        else if (at_board[y_laser - 1][x_laser - 1] == 'a')
        {
            // up-bottom-check
            up_bottom_check(y_laser, x_laser);
            event = true;
        }

        else
        {
            --x_laser;
        }

    }

    if (!event)
    {
        x_laser -= 1;

        shots_memory.push_back({ y_laser, x_laser });
        nr_of_fields.push_back(2);


        at_board[y_laser][x_laser] = 's';

        ++miss_counter;

        if (miss_counter < 10)
        {
            prev_char = '0' + miss_counter;
            board[y_laser][x_laser] = '0' + miss_counter;
        }
        else
        {
            prev_char = 90 + miss_counter;
            board[y_laser][x_laser] = 90 + miss_counter;
        }
    }
}

void GameSession::shoot()
{
    if ((cursor[0] > 0 && cursor[0] < size_board - 1 && (cursor[1] == 0 || cursor[1] == size_board - 1))
        || (cursor[1] > 0 && cursor[1] < size_board - 1 && (cursor[0] == 0 || cursor[0] == size_board - 1)))
    {
        if (at_board[cursor[0]][cursor[1]] != 's')
        {
            short y_laser = cursor[0];
            short x_laser = cursor[1];

            if (cursor[0] == 0)
            {
                // up
                if (at_board[y_laser + 1][x_laser] == 'a')
                {
                    prev_char = 'H';
                    nr_of_fields.push_back(1);

                }

                else if (at_board[y_laser + 1][x_laser - 1] == 'a' || at_board[y_laser + 1][x_laser + 1] == 'a')
                {
                    prev_char = 'R';
                    nr_of_fields.push_back(1);
                }

                else
                {
                    ++y_laser;
                    //chaeck up_down
                    up_bottom_check(y_laser, x_laser);
                }

            }

            else if (cursor[1] == 0)
            {
                // left

                if (at_board[y_laser][x_laser + 1] == 'a')
                {
                    prev_char = 'H';
                    nr_of_fields.push_back(1);
                }

                else if (at_board[y_laser - 1][x_laser + 1] == 'a' || at_board[y_laser + 1][x_laser + 1] == 'a')
                {
                    prev_char = 'R';
                    nr_of_fields.push_back(1);
                }

                else
                {
                    ++x_laser;
                    //check left-right
                    left_right_check(y_laser, x_laser);

                }
            }

            else if (cursor[0] == size_board - 1)
            {
                // bottom

                if (at_board[y_laser - 1][x_laser] == 'a')
                {
                    prev_char = 'H';
                    nr_of_fields.push_back(1);
                }

                else if (at_board[y_laser - 1][x_laser - 1] == 'a' || at_board[y_laser - 1][x_laser + 1] == 'a')
                {
                    prev_char = 'R';
                    nr_of_fields.push_back(1);
                }

                else
                {
                    --y_laser;
                    //check down-up
                    bottom_up_check(y_laser, x_laser);
                }
            }

            else if (cursor[1] == size_board - 1)
            {
                // right

                if (at_board[y_laser][x_laser - 1] == 'a')
                {
                    prev_char = 'H';
                    nr_of_fields.push_back(1);

                }

                else if (at_board[y_laser - 1][x_laser - 1] == 'a' || at_board[y_laser + 1][x_laser - 1] == 'a')
                {
                    prev_char = 'R';
                    nr_of_fields.push_back(1);

                }

                else
                {
                    --x_laser;
                    // right-left
                    right_left_check(y_laser, x_laser);
                }
            }
            ++counter_shot;
            shots_memory.push_back({ cursor[0], cursor[1] });
            at_board[cursor[0]][cursor[1]] = 's';
            display_game_window(true);

        }

        else
        {
            message = "Shot already marked";
            display_game_window(false);
            message = "Message Window";
        }
    }

    else
    {
        message = "Shot impossible";
        display_game_window(false);
        message = "Message Window";
    }
}

void GameSession::undo_shot()
{
    if (!nr_of_fields.empty())
    {
        for (int i = 0; i < nr_of_fields.back() && !shots_memory.empty(); ++i)
        {
            if (cursor[0] == shots_memory.back()[0] && cursor[1] == shots_memory.back().back())
            {
                prev_char = '#';
            }
            else
            {
                board[shots_memory.back()[0]][shots_memory.back()[1]] = '#';
            }

            at_board[shots_memory.back()[0]][shots_memory.back()[1]] = '*';
            shots_memory.pop_back();
        }

        if (nr_of_fields.back() == 2)
        {
            --miss_counter;
        }
        --counter_shot;
        nr_of_fields.pop_back();
        display_game_window(false);
    }
    else
    {
        message = "There is nothink to undo";
        display_game_window(false);
        message = "Message Window";
    }
}

void GameSession::mark_atom()
{
    if (cursor[0] > 0 && cursor[0] < size_board - 1 && cursor[1] > 0 && cursor[1] < size_board - 1)
    {
        if (marked_atoms.size() < number_of_atoms)
        {
            if (!marked_atoms.empty())
            {
                bool replay = false;

                for (int i = 0; i < marked_atoms.size(); ++i)
                {
                    if (cursor[0] == marked_atoms[i][0] && cursor[1] == marked_atoms[i][1])
                    {
                        replay = true;
                        i = marked_atoms.size();
                        message = "Cannot mark an atom twice";
                        display_game_window(true);
                        message = "Message Window";
                    }
                }

                if (!replay)
                {
                    marked_atoms.push_back({ cursor[0], cursor[1] });
                    prev_char = 'o';
                    display_game_window(true);

                }
            }
            else
            {
                marked_atoms.push_back({ cursor[0],cursor[1] });
                prev_char = 'o';
                display_game_window(true);
            }
        }
        else
        {
            message = "Limit of marked atoms reached";
            display_game_window(true);
            message = "Message Window";
        }
    }
    else
    {
        message = "Mark impossible";
        display_game_window(true);
        message = "Message Window";
    }


}

void GameSession::unmark_atom()
{
    if (!marked_atoms.empty())
    {
        if (marked_atoms.back()[0] == cursor[0] && marked_atoms.back()[1] == cursor[1])
        {
            prev_char = '*';
        }
        else
        {
            board[marked_atoms.back()[0]][marked_atoms.back().back()] = '*';
        }
        marked_atoms.pop_back();
        display_game_window(false);
    }

    else
    {
        message = "No atoms marked";
        display_game_window(false);
        message = "Message Window";
    }
}

GameSession::~GameSession()
{
    short hits = 0, horizontal_p = 0, vertical_p = 0;
    for (int i = 0; i < number_of_atoms; ++i)
    {
        board[atoms[i][0]][atoms[i][1]] = 'O';
        if (atoms[i][0] == prev_coord_char[0] && atoms[i][1] == prev_coord_char[1])
            prev_char = '0';

    }
    for (int i = 0; i < marked_atoms.size(); ++i)
    {
        bool miss = true;

        for (int j = 0; j < number_of_atoms; ++j)
        {
            if (marked_atoms[i][0] == atoms[j][0] && marked_atoms[i][1] == atoms[j][1])
            {
                miss = false;
                ++hits;
            }
        }

        if (miss == true)
        {
            board[marked_atoms[i][0]][marked_atoms[i][1]] = 'x';
            if (marked_atoms[i][0] == prev_coord_char[0] && marked_atoms[i][1] == prev_coord_char[1])
                prev_char = 'x';
        }

    }
    message.clear();
    message += "Your score is ";
    message += static_cast <char> ('0' + hits);
    message += R"( \ )";
    message += static_cast <char> ('0' + number_of_atoms);
    display_game_window(1);
    std::this_thread::sleep_for(std::chrono::seconds(5)); //wait 6 sec.
    bool exit = false;
    message = "Press any key to continue";
    display_game_window(true);
    while (!exit)
    {
        if (cross_getch())
            exit = true;
    }
}


