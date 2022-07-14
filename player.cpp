#include "player.hpp"

using namespace std;
const int board_size = 8;
const short max_pieces_cnt = 2 * (4 * 3);
const int first_board[board_size][board_size] = {{3, 0, 3, 0, 3, 0, 3, 0},
                                                 {0, 3, 0, 3, 0, 3, 0, 3},
                                                 {3, 0, 3, 0, 3, 0, 3, 0},
                                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                                 {0, 0, 0, 0, 0, 0, 0, 0},
                                                 {0, 1, 0, 1, 0, 1, 0, 1},
                                                 {1, 0, 1, 0, 1, 0, 1, 0},
                                                 {0, 1, 0, 1, 0, 1, 0, 1}};


struct Player::Impl {
    int player_nr;
    struct Cell {
        int info[board_size][board_size];
        Cell *next;
    };
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


Player &Player::operator=(const Player &old) {
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
            pezzo = e;
            break;
        case 1:
            pezzo = x;
            break;
        case 2:
            pezzo = X;
            break;
        case 3:
            pezzo = o;
            break;
        case 4:
            pezzo = O;
            break;
        default:
            throw player_exception{player_exception::index_out_of_bounds, "tipo del pezzo non valido"};

    }
    return pezzo;
}


void Player::move() {

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
    ifstream input(filename, ios::in);
    if (input.fail()) {
        throw player_exception{player_exception::missing_file, "nonexisting file: " + filename};
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
            int j = 0;
            int x = 0;
            for (auto t: s) {
                if (j % 2 == 0) {
                    pieces_cnt++;
                    if (t == 'x') {
                        tmp_board[i][x] = 1;
                    } else if (t == 'X') {
                        tmp_board[i][x] = 2;
                    } else if (t == 'o') {
                        tmp_board[i][x] = 3;
                    } else if (t == 'O') {
                        tmp_board[i][x] = 4;
                    } else if (t == ' ') {
                        pieces_cnt--; //non avevo voglia di fare il ++ ovunque
                        tmp_board[i][x] = 0;
                    } else {
                        throw player_exception{player_exception::invalid_board, "not a valid char " + filename};
                    }
                    x++;
                } else if (t != ' ') {
                    throw player_exception{player_exception::invalid_board,
                                           "trying to insert a piece in a invalid cell"};
                }
                j++;
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
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            string s;
            switch (board[i][j]) {
                case 0:
                    s = " ";
                    break;
                case 1:
                    s = "x";
                    break;
                case 2:
                    s = "X";
                    break;
                case 3:
                    s = "o";
                    break;
                case 4:
                    s = "O";
                    break;
                default:
                    throw player_exception{player_exception::index_out_of_bounds,
                                           "invalid char for the board"};
            }
            out << s;
            if (j < board_size - 1) {
                out << " ";
            }
        }
        if (i < board_size - 1) {
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