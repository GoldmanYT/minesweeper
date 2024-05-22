#include <iostream>
#include <ctime>
#include <vector>
#include <string>

using namespace std;


class Minesweeper
{
private:
    enum node_type {empty, bomb = 9};
    enum node_state {unopened, opened, with_flag};
    enum game_states {run, won, lost};
    const char node_chars[12] = " .12345678B";
    const char flag_char = 'F';

    struct node
    {
        size_t x;
        size_t y;
    };

    size_t width;
    size_t height;
    size_t bomb_count;
    size_t game_state;
    vector<vector<short>> type_field;
    vector<vector<short>> state_field;

    bool on_field(size_t x, size_t y)
    {
        return x < width && y < height;
    }

    void open_all_nodes()
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                state_field[y][x] = opened;
            }
        }
    }

public:
    Minesweeper(size_t field_width, size_t field_height, size_t field_bomb_count)
    {
        width = field_width;
        height = field_height;
        bomb_count = field_bomb_count;

        type_field.resize(height);
        state_field.resize(height);

        for (size_t y = 0; y < height; y++)
        {
            type_field[y].resize(width);
            state_field[y].resize(width);
        }

        new_game();
    }

    void print()
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                cout << "+---";
            }
            cout << "+\n";
            for (size_t x = 0; x < width; x++)
            {
                cout << "| ";
                short current_node_type = type_field[y][x];
                switch (state_field[y][x])
                {
                case unopened:
                    cout << node_chars[empty];
                    break;

                case opened:
                    cout << node_chars[current_node_type + 1];
                    break;

                case with_flag:
                    cout << flag_char;
                    break;
                }
                cout << " ";
            }
            cout << "|\n";
        }
        for (size_t x = 0; x < width; x++)
        {
            cout << "+---";
        }
        cout << "+\n";
    }

    void open_node(size_t cx, size_t cy)
    {
        if (!on_field(cx, cy))
        {
            return;
        }

        state_field[cy][cx] = opened;
        short opened_node_type = type_field[cy][cx];

        if (opened_node_type == bomb)
        {
            game_state = lost;
            open_all_nodes();
            return;
        }

        if (opened_node_type == empty)
        {
            node first{cx, cy};
            vector<node> stack;
            stack.push_back(first);

            while (!stack.empty())
            {
                node current = stack.back();
                stack.pop_back();

                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        size_t x = current.x + dx;
                        size_t y = current.y + dy;
                        if (on_field(x, y) && (dx != 0 || dy != 0))
                        {
                            short current_node_type = type_field[y][x];
                            if (current_node_type == empty && state_field[y][x] == unopened)
                            {
                                node next_node{x, y};
                                stack.push_back(next_node);
                            }
                            if (state_field[y][x] == unopened)
                            {
                                state_field[y][x] = opened;
                            }
                        }
                    }
                }
            }
            return;
        }
    }

    void place_flag(size_t x, size_t y)
    {
        if (on_field(x, y))
        {
            state_field[y][x] = with_flag;
        }
    }

    void new_game()
    {
        game_state = run;
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                type_field[y][x] = empty;
                state_field[y][x] = unopened;
            }
        }

        srand(time(0));
        size_t put_bomb_count = 0;
        while (put_bomb_count < bomb_count)
        {
            size_t x = rand() % width;
            size_t y = rand() % height;
            if (type_field[y][x] != bomb)
            {
                put_bomb_count++;
                type_field[y][x] = bomb;
            }
        }

        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                if (type_field[y][x] != bomb)
                {
                    short around_bomb_count = 0;
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        for (int dx = -1; dx <= 1; dx++)
                        {
                            if (on_field(x + dx, y + dy) && (dx != 0 || dy != 0) && type_field[y + dy][x + dx] == bomb)
                            {
                                around_bomb_count++;
                            }
                        }
                    }
                    type_field[y][x] = around_bomb_count;
                }
            }
        }
    }

    bool game_ended()
    {
        if (game_state == lost)
        {
            return true;
        }
        
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                if (type_field[y][x] != bomb && state_field[y][x] != opened)
                {
                    return false;
                }
            }
        }
        game_state = won;
        open_all_nodes();
        return true;
    }

    bool get_game_state()
    {
        return game_state;
    }
};


int main()
{
    Minesweeper game(5, 5, 3);
    while (!game.game_ended())
    {
        game.print();
        string s;
        size_t x, y;
        cout << "Enter node \"<x> <y>\" to open or \"F <x> <y>\" to place flag: ";
        cin >> s;
        if (s == "F")
        {
            cin >> x >> y;
            game.place_flag(x - 1, y - 1);
        }
        else
        {
            x = stoull(s);
            cin >> y;
            game.open_node(x - 1, y - 1);
        }
    }
    game.print();
    int game_state = game.get_game_state();
    switch (game_state)
    {
    case 1:
        cout << "You won!";
        break;
    
    default:
        cout << "You lost!";
        break;
    }
    return 0;
}