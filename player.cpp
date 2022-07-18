#include "player.hpp"

using namespace std;
const int board_size = 8;
const short max_pieces_cnt = 2 * (4 * 3);
const int first_board[board_size][board_size] = {
        {0, 4, 0, 4, 0, 4, 0, 4},
        {4, 0, 4, 0, 4, 0, 4, 0},
        {0, 4, 0, 4, 0, 4, 0, 4},
        {4, 4, 4, 4, 4, 4, 4, 4},
        {4, 4, 4, 4, 4, 4, 4, 4},
        {4, 1, 4, 1, 4, 1, 4, 1},
        {1, 4, 1, 4, 1, 4, 1, 4},
        {4, 1, 4, 1, 4, 1, 4, 1},
};


struct Player::Impl {
    int player_nr;

    struct Cell {
        int info[board_size][board_size];
        Cell *next;

    };

    void print_board(const int(&board)[board_size][board_size]) const;

    typedef Cell *Pcell;

    void destroy(Pcell pc);

    void prepend(const int(&board)[board_size][board_size]);

    Pcell copy(Pcell source) const;

    Pcell at(int pos);


    void write_on_file(const int(&board)[board_size][board_size], const string &filename) const;


    Pcell head;
};


/**
 *
 * start player with empty history. player_nr can be either 1 or 2.
 * Player 1 is the one starting in the low row values (rows 0,1,2),
 * player 2 starts in the high row values (rows 5,6,7).
* @param player_nr
 */
Player::Player(int player_nr) {

    if (player_nr != 1 && player_nr != 2) {
        throw player_exception{player_exception::index_out_of_bounds, "Il numero del player deve essere 1 o 2"};
    }
    this->pimpl = new Impl;
    pimpl->player_nr = player_nr;
    pimpl->head = nullptr;
}

Player::~Player() {
    pimpl->destroy(pimpl->head);
    delete pimpl;
}

void Player::Impl::destroy(Pcell pc) {
    if (pc != nullptr) {
        destroy(pc->next);
        delete pc;
    }
}


Player::Player(const Player &old) {
    this->pimpl = new Impl;
    pimpl->player_nr = old.pimpl->player_nr;

    pimpl->destroy(pimpl->head);

    pimpl->head = pimpl->copy(old.pimpl->head);

}

Player::Impl::Pcell Player::Impl::copy(Pcell source) const {
    if (source == nullptr)
        return nullptr;
    else {
        auto dest = new Cell;
        for (int i = 0; i < board_size; i++) {
            for (int j = 0; j < board_size; j++) {
                dest->info[i][j] = source->info[i][j];
            }
        }
        dest->next = copy(source->next);
        return dest;
    }
}


void Player::Impl::print_board(const int(&board)[board_size][board_size]) const {
    cout << "  j: ";
    for (int i = board_size - 1; i >= 0; --i) {
        cout << i << ' ';
    }
    cout << endl;
    for (int i = board_size - 1; i >= 0; --i) {
        cout << "i: " << i << ' ';
        for (int j = board_size - 1; j >= 0; --j) {
            char pezzo;

            switch (board[i][j]) {
                case 0:
                    pezzo = 'x';
                    break;
                case 1:
                    pezzo = 'o';
                    break;
                case 2:
                    pezzo = 'X';
                    break;
                case 3:
                    pezzo = 'O';
                    break;
                case 4:
                    pezzo = ' ';
                    break;
            }
            cout << pezzo << ' ';
        }
        cout << endl;
    }
    cout << "  j: ";
    for (int i = board_size - 1; i >= 0; --i) {
        cout << i << ' ';
    }
};

Player &Player::operator=(const Player &old) {
    this->pimpl = new Impl;
    pimpl->player_nr = old.pimpl->player_nr;

    pimpl->destroy(pimpl->head);
    delete this;
    pimpl->player_nr = old.pimpl->player_nr;
    pimpl->head = pimpl->copy(old.pimpl->head);
    return *this;
}


Player::piece Player::operator()(int r, int c, int history_offset) const {
    if (r < 0 || r >= 8 || c < 0 || c >= 8) {
        throw player_exception{player_exception::index_out_of_bounds, "Non  valore di r e/o c non accettati"};
    }
    Player::Impl::Pcell tmp = pimpl->at(history_offset);
    Player::piece pezzo;
    switch (tmp->info[r][c]) {
        case 0:
            pezzo = x;
            break;
        case 1:
            pezzo = o;
            break;
        case 2:
            pezzo = X;
            break;
        case 3:
            pezzo = O;
            break;
        case 4:
            pezzo = e;
            break;
        default:
            throw player_exception{player_exception::index_out_of_bounds, "tipo del pezzo non valido"};

    }
    return pezzo;
}


void Player::move() {


    int new_table[board_size][board_size] = {};
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            new_table[i][j] = this->pimpl->head->info[i][j];
        }
    }
    pimpl->print_board(new_table);
    bool done = false;
    if (pimpl->player_nr == 1) {
        for (int i = 0; i < board_size && !done; ++i) {
            for (int j = 0; j < board_size && !done; ++j) {
                if (new_table[i][j] == 2) {
                    if (i + 2 < board_size && j + 2 < board_size && new_table[i + 2][j + 2] == 4 &&
                        (new_table[i + 1][j + 1] == 1 || new_table[i + 1][j + 1] == 3)) {
                        new_table[i + 2][j + 2] = 2;
                        new_table[i][j] = 4;
                        new_table[i + 1][j + 1] = 4;
                        done = true;
                    } else if (i + 2 < board_size && j - 2 >= 0 && new_table[i + 2][j - 2] == 4 &&
                               (new_table[i + 1][j - 1] == 1 || new_table[i + 1][j + 1] == 3)) {
                        new_table[i + 2][j - 2] = 2;
                        new_table[i][j] = 4;
                        new_table[i + 1][j - 1] = 4;
                        done = true;
                    } else if (i - 2 >= 0 && j - 2 >= 0 && new_table[i - 2][j - 2] == 4 &&
                               (new_table[i - 1][j - 1] == 1 || new_table[i - 1][j + 1] == 3)) {
                        new_table[i - 2][j - 2] = 2;
                        new_table[i][j] = 4;
                        new_table[i - 1][j - 1] = 4;
                        done = true;
                    } else if (i - 2 >= 0 && j + 2 < board_size && new_table[i - 2][j + 2] == 4 &&
                               (new_table[i - 1][j + 1] == 1 || new_table[i - 1][j + 1] == 3)) {
                        new_table[i - 2][j + 2] = 2;
                        new_table[i][j] = 4;
                        new_table[i - 1][j + 1] = 4;
                        done = true;
                    } else if (i - 1 >= 0 && j - 1 >= 0 && new_table[i - 1][j - 1] == 4) {
                        new_table[i - 1][j - 1] = 2;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i - 1 >= 0 && j + 1 < board_size && new_table[i - 1][j + 1] == 4) {
                        new_table[i - 1][j + 1] = 2;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i + 1 < board_size && j + 1 < board_size && new_table[i + 1][j + 1] == 4) {
                        new_table[i + 1][j + 1] = 2;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i + 1 < board_size && j - 1 >= 0 && new_table[i + 1][j - 1] == 4) {
                        new_table[i + 1][j - 1] = 2;
                        new_table[i][j] = 4;
                        done = true;
                    }

                } else if (new_table[i][j] == 0) {

                    if (i + 2 < board_size && j + 2 < board_size && new_table[i + 2][j + 2] == 4 &&
                        new_table[i + 1][j + 1] == 1) {
                        new_table[i + 2][j + 2] = 0;
                        new_table[i][j] = 4;
                        new_table[i + 1][j + 1] = 4;
                        done = true;
                    } else if (i + 2 < board_size && j - 2 >= 0 && new_table[i + 2][j - 2] == 4 &&
                               new_table[i + 1][j - 1] == 1) {
                        new_table[i + 2][j - 2] = 0;
                        new_table[i][j] = 4;
                        new_table[i + 1][j - 1] = 4;
                        done = true;
                    } else if (i + 1 < board_size && j + 1 < board_size && new_table[i + 1][j + 1] == 4) {
                        new_table[i + 1][j + 1] = 0;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i + 1 < board_size && j - 1 >= 0 && new_table[i + 1][j - 1] == 4) {
                        new_table[i + 1][j - 1] = 0;
                        new_table[i][j] = 4;
                        done = true;
                    }
                }
            }
        }
    } else if (pimpl->player_nr == 2) {
        for (int i = board_size - 1; i >= 0 && !done; --i) {
            for (int j = board_size - 1; j >= 0 && !done; --j) {
                if (new_table[i][j] == 3) {
                    if (i - 2 >= 0 && j - 2 >= 0 && new_table[i - 2][j - 2] == 4 &&
                        (new_table[i - 1][j - 1] == 0 || new_table[i - 1][j - 1] == 2)) {
                        new_table[i - 2][j - 2] = 3;
                        new_table[i][j] = 4;
                        new_table[i - 1][j - 1] = 4;
                        done = true;
                    } else if (i - 2 >= 0 && j + 2 < board_size && new_table[i - 2][j + 2] == 4 &&
                               (new_table[i - 1][j + 1] == 0 || new_table[i - 1][j + 1] == 2)) {
                        new_table[i - 2][j + 2] = 3;
                        new_table[i][j] = 4;
                        new_table[i - 1][j + 1] = 4;
                        done = true;
                    } else if (i + 2 < board_size && j + 2 < board_size && new_table[i + 2][j + 2] == 4 &&
                               (new_table[i + 1][j + 1] == 0 || new_table[i + 1][j + 1] == 2)) {
                        new_table[i + 2][j + 2] = 3;
                        new_table[i][j] = 4;
                        new_table[i + 1][j + 1] = 4;
                        done = true;
                    } else if (i + 2 > board_size && j - 2 >= 0 && new_table[i + 2][j - 2] == 4 &&
                               (new_table[i + 1][j - 1] == 0 || new_table[i + 1][j - 1] == 2)) {
                        new_table[i + 2][j - 2] = 3;
                        new_table[i][j] = 4;
                        new_table[i + 1][j + 1] = 4;
                        done = true;
                    } else if (i - 1 >= 0 && j - 1 >= 0 && new_table[i - 1][j - 1] == 4) {
                        new_table[i - 1][j - 1] = 3;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i - 1 >= 0 && j + 1 < board_size && new_table[i - 1][j + 1] == 4) {
                        new_table[i - 1][j + 1] = 3;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i + 1 < board_size && j + 1 < board_size && new_table[i + 1][j + 1] == 4) {
                        new_table[i + 1][j + 1] = 3;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i + 1 < board_size && j - 1 >= 0 && new_table[i + 1][j - 1] == 4) {
                        new_table[i + 1][j - 1] = 3;
                        new_table[i][j] = 4;
                        done = true;
                    }

                }
                if (new_table[i][j] == 1) {
                    if (i - 2 >= 0 && j - 2 >= 0 && new_table[i - 2][j - 2] == 4 && new_table[i - 1][j - 1] == 0) {
                        new_table[i - 2][j - 2] = 1;
                        new_table[i][j] = 4;
                        new_table[i - 1][j - 1] = 4;
                        done = true;
                    } else if (i - 2 >= 0 && j + 2 < board_size && new_table[i - 2][j + 2] == 4 &&
                               new_table[i - 1][j + 1] == 0) {
                        new_table[i - 2][j + 2] = 1;
                        new_table[i][j] = 4;
                        new_table[i - 1][j + 1] = 4;
                        done = true;
                    } else if (i - 1 >= 0 && j - 1 >= 0 && new_table[i - 1][j - 1] == 4) {
                        new_table[i - 1][j - 1] = 1;
                        new_table[i][j] = 4;
                        done = true;
                    } else if (i - 1 >= 0 && j + 1 < board_size && new_table[i - 1][j + 1] == 4) {
                        new_table[i - 1][j + 1] = 1;
                        new_table[i][j] = 4;
                        done = true;
                    }
                }
            }
        }
    } else {
        throw player_exception{player_exception::index_out_of_bounds, "Il numero del player deve essere 1 o 2"};
    }

    for (int j = 0; j < board_size; j++) {
        if (new_table[board_size - 1][j] == 0) {
            new_table[board_size - 1][j] = 2;
        }
        if (new_table[0][j] == 1) {
            new_table[0][j] = 3;
        }
    }
    pimpl->prepend(new_table);
}


Player::Impl::Pcell Player::Impl::at(int pos) {
    Pcell pc = head;
    while (pc != nullptr && pos > 0) {
        pc = pc->next;
        pos--;
    }
    if (pc)
        return pc;
    else
        throw player_exception{player_exception::index_out_of_bounds,
                               "Non  esiste la cella della lista alla posizione richietsa"};
}

void Player::load_board(const string &filename) {
    ifstream input(filename, fstream::in);
    if (input.fail()) {
        throw player_exception{player_exception::missing_file,
                               "not existing file: " + filename + ' ' + strerror(errno)};
    }
    if (filename.substr(filename.find_last_of('.') + 1) != "txt") {
        throw player_exception{player_exception::missing_file, "not a txt file file: " + filename};
    }
    int tmp_board[board_size][board_size];
    string line;
    short pieces_cnt = 0;
    int i = board_size;
    while (input) {
        istringstream line_str{line};
        while (!line_str.eof()) {
            string s;
            getline(line_str, s);
            int j = (board_size - 1) * 2;
            int x = board_size - 1;
            for (auto t: s) {
                if (j % 2 == 0) {
                    pieces_cnt++;
                    if (t == 'x') {
                        tmp_board[i][x] = 0;
                    } else if (t == 'X') {
                        tmp_board[i][x] = 2;
                    } else if (t == 'o') {
                        tmp_board[i][x] = 1;
                    } else if (t == 'O') {
                        tmp_board[i][x] = 3;
                    } else if (t == ' ') {
                        pieces_cnt--; //non avevo voglia di fare il ++ ovunque
                        tmp_board[i][x] = 4;
                    } else {
                        throw player_exception{player_exception::invalid_board, "not a valid char " + filename};
                    }
                    x--;
                } else if (t != ' ') {
                    throw player_exception{player_exception::invalid_board,
                                           "trying to insert a piece in a invalid cell"};
                }
                j--;
            }
        }
        i--;
        getline(input, line);
    }
    if (pieces_cnt > max_pieces_cnt) {
        throw player_exception{player_exception::invalid_board, "trying to insert more pieces than allowed"};
    }
    pimpl->prepend(tmp_board);
}

void Player::Impl::prepend(const int(&board)[board_size][board_size]) {
    Pcell newone = new Cell;
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            newone->info[i][j] = board[i][j];
        }
    }
    newone->next = head;
    head = newone;
}

void Player::store_board(const string &filename, int history_offset) const {
    pimpl->write_on_file(pimpl->at(history_offset)->info, filename);
}


void Player::Impl::write_on_file(const int (&board)[board_size][board_size], const string &filename) const {
    ofstream out(filename, ios::out);
    if (filename.substr(filename.find_last_of('.') + 1) != "txt") {
        throw player_exception{player_exception::missing_file, "not a txt file for output file: " + filename};
    }
    for (int i = board_size - 1; i >= 0; i--) {
        for (int j = board_size - 1; j >= 0; j--) {
            string s;
            switch (board[i][j]) {
                case 0:
                    s = "x";
                    break;
                case 1:
                    s = "o";
                    break;
                case 2:
                    s = "X";
                    break;
                case 3:
                    s = "O";
                    break;
                case 4:
                    s = " ";
                    break;
                default:
                    throw player_exception{player_exception::index_out_of_bounds,
                                           "invalid char for the board"};
            }
            out << s;
            if (j > 0) {
                out << " ";
            }
        }
        if (i > 0) {
            out << '\n';
        }

    }
    out.close();
}

void Player::init_board(const string &filename) const {
    pimpl->write_on_file(first_board, filename);

}

bool Player::valid_move() const {


    return false;
}

void Player::pop() {

}

bool Player::wins(int player_nr) const {
    return false;
}

bool Player::wins() const {
    return false;
}

bool Player::loses(int player_nr) const {
    return false;
}

bool Player::loses() const {
    return false;
}

int Player::recurrence() const {
    return 0;
}