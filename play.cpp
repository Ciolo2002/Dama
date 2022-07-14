#include "player.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

using std::cout;
using std::cin;
using std::endl;
using std::ifstream;
using std::string;

/*
int main(int argc, char **argv){

    int player_nr = 1;

    Player p(player_nr);

    cout << "Playing as player " << player_nr << endl;
    Player c(p);
    Player x = c;
    int round = player_nr; // 1 or 2

    while (true) {

        string board_name = "board_" + std::to_string(round) + ".txt";
        ifstream infile(board_name);

        if (infile.good()) {

            cout << "Reading board " << board_name << endl;
		
			infile.close();
			std::this_thread::sleep_for (std::chrono::milliseconds(100));
			
			p.load_board(board_name);
			p.move();
			board_name =  "board_" + std::to_string(++round) + ".txt";
			p.store_board(board_name);
			round++;
			
		}
		
	}
	
} */


int main() {
    Player p1(1);

    Player p2(2);
    int i = 1;
    p1.init_board("board_1.txt");
    std::cout << "PLAYER 1: " << std::endl;
    std::cout << "CARICATA BOARD:" << i << std::endl;
    p1.load_board("board_1.txt");
    p1.load_board("board_1.txt");
    p1.load_board("board_1.txt");
    p1.load_board("board_1.txt");

    cout << p1(0, 0, 0) << ' ';
    cout << p1(1, 0, 1) << ' ';
    cout << p1(7, 6, 2) << ' ';

    p1.store_board("board_2.txt", 1);
    p1.store_board("board_3.txt", 2);
    p1.load_board("board_2.txt");

    /* while (i < 10) {
         try {
             std::cout << "PLAYER 1: " << std::endl;
             std::cout << "CARICATA BOARD:" << i << std::endl;
             p1.load_board("board_" + std::to_string(i++) + ".txt");
             p1.move();
             std::cout << "SALVATA BOARD" << i << std::endl;
             p1.store_board("board_" + std::to_string(i) + ". txt");
             std::cout << "PLAYER 2:" << std::endl;
             std::cout << "CARICATA BOARD:" << i << std::endl;
             p2.load_board("board_" + std::to_string(i++) + ". txt");
             p2.move();
             std::cout << "SALVATA BOARD:" << i << std::endl;
             p2.store_board("board " + std::to_string(i) + ".txt");
         }
         catch (player_exception e ) {
             std::cerr << e.msg << std::endl;
         }
     } */
    return 0;
}