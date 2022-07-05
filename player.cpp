#include "player.hpp"

using namespace std;
const int board_size = 8;

struct Player::Impl {
    int player_nr;
    struct Cell {
        int info[board_size][board_size];
        Cell *next;
    };
    typedef Cell* Pcell;
    void destroy(Pcell pc);

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
    pimpl->player_nr = player_nr;
    pimpl->head = nullptr;
}

Player::~Player() {
    pimpl->destroy(pimpl->head);
    delete pimpl;
}


void Player::Impl::destroy(Pcell pc) {
    if (pc!=nullptr) {
        destroy(pc->next);
        delete pc;
    }
}


void Player::move() {

}


void Player::load_board(const string &filename) {

}

void Player::store_board(const string &filename, int history_offset) const {

}

void Player::init_board(const string &filename) const {

}

bool Player::valid_move() const {

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