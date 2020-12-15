#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <cstdio>
#include <algorithm>

using namespace std;

typedef vector<int> Board;

#define CHECK_SAME_COLOR(c1, c2)  (((c1)  < 0 && (c2) < 0) || ((c1)  > 0 && (c2) > 0 && (c1) != BEDROCK && (c2) != BEDROCK))
#define MOVE_SCORE(move, board) (piece_values[move.value] - piece_values[board[move.start]] - piece_values[board[move.end]])


const int BOARD_SIZE = 12;
int MAX_DEPTH = 3;

const int EMPTY = 0;
const int KING = 1;
const int QUEEN = 2;
const int ROOK = 3;
const int BISHOP = 4;
const int KNIGHT = 5;
const int PAWN = 6;
const int BEDROCK = 7;

const int WHITE = 1;
const int BLACK = -1;

struct One_move {
	int start;
	int end;
	int value;
};



vector<int> non_bedrock_cells;
vector<int> white_promote{ QUEEN, ROOK, BISHOP, KNIGHT };
vector<int> black_promote{ -QUEEN, -ROOK, -BISHOP, -KNIGHT };

vector<int> position_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 5, 5, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 5, 5, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

vector<int> white_pawn_pos_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 15, 15, 15, 15, 15, 15, 15, 15, 0, 0,
	0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0,
	0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
	0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

vector<int> black_pawn_pos_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, -2, -2, -2, -2, -2, -2, -2, -2, 0, 0,
	0, 0, -4, -4, -4, -4, -4, -4, -4, -4, 0, 0,
	0, 0, -6, -6, -6, -6, -6, -6, -6, -6, 0, 0,
	0, 0, -8, -8, -8, -8, -8, -8, -8, -8, 0, 0,
	0, 0, -15, -15, -15, -15, -15, -15, -15, -15, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



vector<int> knigh_bishop_pos_score{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 4, 4, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 3, 3, 3, 3, 2, 1, 0, 0,
	0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


bool operator ==(const One_move& om1, const One_move& om2) {
	return (om1.start == om2.start && om1.end == om2.end && om1.value == om2.value);
}

unordered_map<char, int> promote{
	{'Q', QUEEN},
	{'R', ROOK},
	{'B', BISHOP},
	{'N', KNIGHT}
};

unordered_map<int, vector<int>> tmp_move{
	{KING, {-1, 1, BOARD_SIZE, -BOARD_SIZE, BOARD_SIZE - 1, BOARD_SIZE + 1, -BOARD_SIZE - 1, -BOARD_SIZE + 1}},
	{ROOK, {-1, 1, BOARD_SIZE, -BOARD_SIZE}},
	{BISHOP, {-BOARD_SIZE - 1, -BOARD_SIZE + 1, BOARD_SIZE + 1, BOARD_SIZE - 1}},
	{KNIGHT, {-2 * BOARD_SIZE + 1, -2 * BOARD_SIZE - 1, -BOARD_SIZE - 2, -BOARD_SIZE + 2, 2 * BOARD_SIZE + 1, 2 * BOARD_SIZE - 1, BOARD_SIZE - 2, BOARD_SIZE + 2}}
};
unordered_map<int, vector<int>> piece_move{
	{KING, tmp_move[KING]},
	{QUEEN, tmp_move[KING]},
	{ROOK, tmp_move[ROOK]},
	{BISHOP, tmp_move[BISHOP]},
	{KNIGHT, tmp_move[KNIGHT]},
	{PAWN, {-BOARD_SIZE }},
	{-KING, tmp_move[KING]},
	{-QUEEN, tmp_move[KING]},
	{-ROOK, tmp_move[ROOK]},
	{-BISHOP, tmp_move[BISHOP]},
	{-KNIGHT, tmp_move[KNIGHT]},
	{-PAWN, { BOARD_SIZE }},
	{EMPTY, {}},
	{BEDROCK, {}}
};

unordered_map<int, int> piece_values{
	{EMPTY, 0},
	{BEDROCK, 0},
	{KING, 1000},
	{QUEEN, 9},
	{KNIGHT, 3},
	{BISHOP, 3},
	{PAWN, 1},
	{ROOK, 5},
	{-KING, -1000},
	{-QUEEN, -9},
	{-KNIGHT, -3},
	{-BISHOP, -3},
	{-PAWN, -1},
	{-ROOK, -5},
};
unordered_map<int, string> piece_to_print{
	{KING, "wK"},
	{QUEEN, "wQ"},
	{ROOK, "wR"},
	{BISHOP, "wB"},
	{KNIGHT, "wN"},
	{PAWN, "wP"},
	{-KING, "bK"},
	{-QUEEN, "bQ"},
	{-ROOK, "bR"},
	{-BISHOP, "bB"},
	{-KNIGHT, "bN"},
	{-PAWN, "bP"},
	{EMPTY, "  "}
};
unordered_map<int, bool> repeated_piece{
	{KING, false},
	{QUEEN, true},
	{ROOK, true},
	{BISHOP, true},
	{KNIGHT, false},
	{-KING, false},
	{-QUEEN, true},
	{-ROOK, true},
	{-BISHOP, true},
	{-KNIGHT, false}
};

unordered_map<int, int> position_piece_score{
	{ROOK, 1},
	{BISHOP, 2},
	{KNIGHT, 2},
	{PAWN, 4},
	{QUEEN, 0},
	{KING, -1},
	{-ROOK, -1},
	{-BISHOP, -2},
	{-KNIGHT, -2},
	{-PAWN, -4},
	{-QUEEN, 0},
	{-KING, 1},
	{EMPTY, 0}
};


Board create_initial_board() {
	return {
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,-ROOK, -KNIGHT, -BISHOP, -QUEEN, -KING, -BISHOP, -KNIGHT, -ROOK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,-PAWN, -PAWN,-PAWN,-PAWN,-PAWN,-PAWN,-PAWN,-PAWN,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,PAWN, PAWN,PAWN,PAWN,PAWN,PAWN,PAWN,PAWN,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK
	};
}


Board create_initial_board_test() {
	cout << "get here test\n";
	return {
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,-QUEEN,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,-KING,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,-PAWN,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,QUEEN,EMPTY,PAWN,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,KING,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
	};
}


Board create_initial_board_test_1() {
	cout << "get here test\n";
	return {
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,-KING,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,-PAWN,EMPTY,BEDROCK,BEDROCK,		
		BEDROCK, BEDROCK,-PAWN, EMPTY,EMPTY,EMPTY,-KNIGHT,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,KNIGHT,-KNIGHT, EMPTY,-BISHOP,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, EMPTY,EMPTY,-PAWN,QUEEN,EMPTY,-BISHOP,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,PAWN, EMPTY,EMPTY,PAWN,PAWN,-QUEEN,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,EMPTY, PAWN,PAWN,EMPTY,EMPTY,PAWN,EMPTY,PAWN,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,ROOK, EMPTY,EMPTY,EMPTY,KING,EMPTY,EMPTY,EMPTY,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
		BEDROCK, BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,BEDROCK,
	};
}


int find_best_score_for_black(Board& board, int depth, int best_white_score, int best_black_score);
int find_best_score_for_white(Board& board, int depth, int best_white_score, int best_black_score);
vector<One_move> get_all_moves(const Board& board, int color, vector<One_move>& output);
vector<One_move> get_all_moves(const Board& board, int color, vector<One_move>& output, bool isCapture);

class int32_vector_hasher {
public:
	std::size_t operator()(std::vector<int> const& vec) const {
		std::size_t seed = vec.size();
		for (auto& i : vec) {
			seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};


std::size_t get_hash_value(std::vector<int> const& vec) {
	std::size_t seed = vec.size();
	for (auto& i : vec) {
		seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

unordered_map<size_t, int>  score_boards;

int ncount = 0;

int attack(const Board& board, vector<One_move>& moves) {
	int score = 0;
	for (One_move move : moves) {
		if (board[move.end] == EMPTY) {
			continue;
		}
		if (CHECK_SAME_COLOR(board[move.end], board[move.start])) {
			continue;
		}

		int piece_start = piece_values[abs(board[move.start])];
		int piece_end = piece_values[abs(board[move.end])];
		score += (piece_start < piece_end) ? piece_end - piece_start : 1;
	}
	return score;
}


int score_is_procted(const Board& board, int color, vector<One_move>& moves) {
	unordered_map<int, bool> protected_positions;
	for (One_move move : moves) {
		protected_positions[move.end] = true;
	}
	int score = 0;
	for (int pos : non_bedrock_cells) {
		if (CHECK_SAME_COLOR(board[pos], color)
			&& protected_positions.find(pos) == protected_positions.end()) {
			score -= piece_values [ abs(board[pos])];
		}
	}
	return score;
}

int protect(const Board& board, vector<One_move>& moves) {
	int score = 0;
	for (One_move move : moves) {
		if (board[move.end] == EMPTY) {
			continue;
		}
		if (CHECK_SAME_COLOR(board[move.end], board[move.start])) {
			int piece_start = piece_values[abs(board[move.start])];
			int piece_end = piece_values[abs(board[move.end])];
			score += (piece_start < piece_end) ? piece_end - piece_start : 1;

		}
		return score;
	}
	return score;
}


int get_space_score(const Board& board, vector<One_move>& moves) {
	int score = 0;
	for (One_move move : moves) {
		if (abs(board[move.start]) != QUEEN  && board[move.end] == EMPTY) {
			score++;
		}
	}
	return score; 
}


bool isKingCaptured(const Board &board, int color) {
	for (int pos : non_bedrock_cells) {
		if (abs(board[pos]) == KING && CHECK_SAME_COLOR(color, board[pos])) {
			return false;
		}
	}
	return true;
}


int get_score(const Board& board) {
	ncount++;

	if (isKingCaptured(board, WHITE)) {
		return std::numeric_limits<int>::min() / 2;
	}
	

	if (isKingCaptured(board, BLACK)) {
		return std::numeric_limits<int>::max()/2 ;
	}



	size_t hash_value = get_hash_value(board);

	if (score_boards.find(hash_value) != score_boards.end()) {
		return score_boards[hash_value];
	}

	int piece_score = 0;
	for (int pos : non_bedrock_cells) {
		piece_score += piece_values[board[pos]];
	}

	int board_position_score = 0;

	for (int pos : non_bedrock_cells) {
		if (board[pos] != BEDROCK && board[pos] != EMPTY) {
			board_position_score += (board[pos] > 0) ? position_score[pos] : -position_score[pos];
		}
	}

	for (int pos : non_bedrock_cells) {
		if (board[pos] == PAWN) {
			board_position_score += white_pawn_pos_score[pos];
		}
	}

	for (int pos : non_bedrock_cells) {
		if (board[pos] == -PAWN) {
			board_position_score += black_pawn_pos_score[pos];
		}
	}

	for (int pos : non_bedrock_cells) {
		if (board[pos] == KNIGHT || board[pos] == BISHOP || board[pos] == -KNIGHT || board[pos] == -BISHOP) {
			board_position_score += (board[pos] > 0) ? knigh_bishop_pos_score[pos] : -knigh_bishop_pos_score[pos];
		}
	}

	vector<One_move> black_moves;
	vector<One_move> white_moves;
	get_all_moves(board, BLACK, black_moves, false);
	get_all_moves(board, WHITE, white_moves, false);

	int score = piece_score * 10000 + board_position_score
		+ get_space_score(board, white_moves) - get_space_score(board, black_moves)
		+ attack(board, white_moves) - attack(board, black_moves)
		+ protect(board, white_moves) - protect(board, black_moves)
		+ score_is_procted(board, WHITE, white_moves) - score_is_procted(board, BLACK, black_moves);		

	score_boards[hash_value] = score;
	return score;
}


bool finish(Board& board) {
	int count = 0;
	for (int pos : non_bedrock_cells) {
		if (board[pos] == KING || board[pos] == -KING) {
			count++;
		}
	}
	return count != 2;
}


vector<int> move_all_but_pawn(int pos, const Board& board) {
	vector<int> output;
	int piece = board[pos];
	for (int move : piece_move[piece]) {
		int new_pos = pos;
		do {
			new_pos += move;
			if (board[new_pos] == BEDROCK) {
				break;
			}
			if (board[new_pos] == EMPTY) {
				output.push_back(new_pos);
			}
			else {
				output.push_back(new_pos);
				break;
			}
		} while (repeated_piece[board[pos]]);

	}
	return output;
}

vector<int> move_pawn(int pos, const Board& board) {
	vector<int> output;
	int new_pos = pos;
	int direction = piece_move[board[pos]][0];
	new_pos += direction;
	if (board[new_pos] == EMPTY) {
		output.push_back(new_pos);
		new_pos += direction;
		int row = pos / BOARD_SIZE;
		bool double_move = (board[pos] < 0) ? row == 3 : row == 8;

		if (board[new_pos] == EMPTY && double_move) {
			output.push_back(new_pos);

		}
	}
	new_pos = pos + direction;
	for (int diag : {-1, 2}) {
		new_pos += diag;
		if (board[new_pos] == EMPTY || board[new_pos] == BEDROCK) {
			continue;
		}
		output.push_back(new_pos);

	}
	return output;

}

vector<One_move> get_all_moves(const Board& board, int color, vector<One_move>& output, bool isCapture) {
	output.clear();
	for (int pos : non_bedrock_cells) {
		int value = board[pos];
		if (value == BEDROCK || value == EMPTY) {
			continue;
		}
		if (!(CHECK_SAME_COLOR(value, color))) {
			continue;
		}
		vector<int> tmp = (abs(value) == PAWN) ? move_pawn(pos, board) : move_all_but_pawn(pos, board);
		for (int new_pos : tmp) {
			if (isCapture) {
				if (CHECK_SAME_COLOR(color, board[new_pos])) {
					continue;
				}
			}
			if (abs(value) == PAWN) {
				int row = new_pos / BOARD_SIZE;
				if (row == 2 || row == 9) {
					vector<int> t = (color < 0) ? black_promote : white_promote;
					for (int promoting_value : t) {
						output.push_back({ pos, new_pos, promoting_value });
					}
					continue;
				}
			}
			output.push_back({ pos, new_pos, board[pos] });
		}
	}

	sort(output.begin(), output.end(),
		[&board, &color](const One_move& m1, const One_move& m2) {
			int move_one_value = MOVE_SCORE(m1, board);
			int move_two_value = MOVE_SCORE(m2, board);
			return (color == WHITE) ? (move_one_value > move_two_value) : (move_one_value < move_two_value);
		});
	return output;
}

vector<One_move> get_all_moves(const Board& board, int color, vector<One_move>& output) {
	return get_all_moves(board, color, output, true);
}


string location_to_notation(int pos) {
	char row = 8 - (pos / BOARD_SIZE - 2) + '0';
	char col = pos % BOARD_SIZE - 2 + 'a';
	string output;

	output += col;
	output += row;
	return output;
}


int to_loc(char file, char rank) {
	int row = (7 - (rank - '1')) + 2;
	int col = (file - 'a') + 2;
	return row * BOARD_SIZE + col;
}


One_move notation_to_location(string s, const Board& board, int color) {
	return {
		to_loc(s[0], s[1]),
		to_loc(s[2], s[3]),
		(s.size() > 4) ? color * promote[s[5]] : board[to_loc(s[0], s[1])]
	};
}


One_move get_users_notation(const Board& board, int color) {
	vector<One_move> tmp;
	get_all_moves(board, color, tmp);
	cout << "\n";
	while (true) {
		string s;
		cout << "Please Move!\n";
		cin >> s;

		One_move humans_move = notation_to_location(s, board, color);
		if (find(tmp.begin(), tmp.end(), humans_move) != tmp.end()) {
			return humans_move;
		}
		cout << "INVALID MOVE \n";
	}
}

vector<int> find_non_bedrock() {
	vector<int> output;
	Board tmp = create_initial_board();
	for (int i = 0; i < tmp.size(); i++) {
		if (tmp[i] != BEDROCK) {
			output.push_back(i);
		}
	}
	return output;
}

void print(const Board& board) {
	cout << "\n\n";
	int count = 8;
	for (int row = 2; row <= 9; row++) {
		cout << "  +--+--+--+--+--+--+--+--+\n";
		cout << count-- << " ";
		for (int col = 2; col <= 9; col++) {
			cout << "|" << piece_to_print[board[row * BOARD_SIZE + col]];
		}
		cout << "|\n";
	}
	cout << "  +--+--+--+--+--+--+--+--+\n   ";
	for (char ch = 'a'; ch < 'h' + 1; ch++) {
		cout << ch << "  ";
	}
	cout << "\n";
	cout << "score = " << get_score(board) << "\n\n";

}

One_move find_best_move_for_white(Board& board) {
	vector<One_move> moves;
	One_move best_move = { -1,-1,1 };
	int best_score = std::numeric_limits<int>::min();
	//get all possible move for WHITE
	get_all_moves(board, WHITE, moves);

	 
	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];
		//make the move
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		int score = find_best_score_for_black(board, MAX_DEPTH, best_score, std::numeric_limits<int>::max());		
		
			cout << "move: " << location_to_notation(move.start) << location_to_notation(move.end) << ": score = " << score << endl;

		board[move.start] = start_value;
		board[move.end] = end_value;

		// update the best score
		if (score > best_score || (score == best_score && rand() % 2 == 0)) {
			best_move = move;
			best_score = score;
		}
	}
	return best_move;
}





int find_best_score_for_white(Board& board, int depth, int best_white_score, int best_black_score) {
	vector<One_move> moves;
	int best_score = std::numeric_limits<int>::min();
	//get all possible move for WHITE
	get_all_moves(board, WHITE, moves);

	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];
		//make the move
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		int score =
			(depth == 0)
			? get_score(board)
			: find_best_score_for_black(board, depth - 1, max(best_white_score, best_score), best_black_score);
		board[move.start] = start_value;
		board[move.end] = end_value;

		if (score > best_black_score) {
			return std::numeric_limits<int>::max();
		}

		// update the best score
		if (score > best_score) {
			best_score = score;
		}

	}
	return best_score;
}



int find_best_score_for_black(Board& board, int depth, int best_white_score, int best_black_score) {
	vector<One_move> moves;
	int best_score = std::numeric_limits<int>::max();
	//get all possible move for WHITE
	get_all_moves(board, BLACK, moves);

	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];
		//make the move
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		int score =
			(depth == 0)
			? get_score(board)
			: find_best_score_for_white(board, depth - 1, best_white_score, min(best_black_score, best_score));

		board[move.start] = start_value;
		board[move.end] = end_value;
		if (score < best_white_score) {
			return std::numeric_limits<int>::min();
		}

		// update the best score
		if (score < best_score) {
			best_score = score;
		}
	}
	return best_score;
}



One_move find_best_move_for_black(Board& board) {
	vector<One_move> moves;

	int best_score = std::numeric_limits<int>::max();
	//get all possible move for WHITE
	get_all_moves(board, BLACK, moves);

	One_move best_move{ -1,-1,-1 };
	for (const One_move& move : moves) {
		int start_value = board[move.start];
		int end_value = board[move.end];
		//make the move
		board[move.start] = EMPTY;
		board[move.end] = move.value;

		int score = find_best_score_for_white(board, MAX_DEPTH, std::numeric_limits<int>::min(), best_score);
		board[move.start] = start_value;
		board[move.end] = end_value;

		// update the best score
		if (score < best_score || (score == best_score && rand() % 2 == 0)) {
			best_move = move;
			best_score = score;
		}
	}
	return best_move;
}


void play_game_simple_for_white() {
	Board board = create_initial_board_test_1();
	//Board board = create_initial_board();
	print(board);
	while (true) {
		ncount = 0;
		One_move best_move = find_best_move_for_white(board);
		board[best_move.start] = EMPTY;
		board[best_move.end] = best_move.value;
		cout << "move: " << location_to_notation(best_move.start) << location_to_notation(best_move.end) << endl;
		print(board);
		cout << "\nNumber of possibilities = " << ncount << "\n";
		One_move human_move = get_users_notation(board, BLACK);
		board[human_move.start] = EMPTY;
		board[human_move.end] = human_move.value;
		print(board);
		cout << "\n";
	}
}

void play_game_simple_for_black() {
	return;
	Board tmp = create_initial_board_test();
	while (true) {
		One_move best_move = find_best_move_for_black(tmp);
		tmp[best_move.start] = EMPTY;
		tmp[best_move.end] = best_move.value;
		cout << "Score = " << get_score(tmp) << "\n";
		print(tmp);
		cout << "\n";
		One_move human_move = get_users_notation(tmp, WHITE);
		tmp[human_move.start] = EMPTY;
		tmp[human_move.end] = human_move.value;
		cout << "Score = " << get_score(tmp) << "\n";
		print(tmp);
		cout << "\n";
	}
}

void play_game_simple() {
	Board board = create_initial_board();
	One_move best_move;
	int old_score = -1;
	while (!finish(board)) {
		best_move = find_best_move_for_white(board);
		board[best_move.start] = EMPTY;
		board[best_move.end] = best_move.value;
		print(board);
		if (finish(board)) {
			break;
		}
		best_move = find_best_move_for_black(board);
		board[best_move.start] = EMPTY;
		board[best_move.end] = best_move.value;
		print(board);
	}
	print(board);
}


int main() {
	srand(time(NULL));
	non_bedrock_cells = find_non_bedrock();
	play_game_simple_for_white();
}

/*


  +--+--+--+--+--+--+--+--+
8 |  |  |  |  |  |  |bK|  |
  +--+--+--+--+--+--+--+--+
7 |  |  |  |  |  |  |bP|  |
  +--+--+--+--+--+--+--+--+
6 |bP|  |  |  |bN|  |  |  |
  +--+--+--+--+--+--+--+--+
5 |  |  |  |wN|bN|  |bB|  |
  +--+--+--+--+--+--+--+--+
4 |  |  |  |bP|wQ|  |bB|  |
  +--+--+--+--+--+--+--+--+
3 |wP|  |  |wP|wP|bQ|  |  |
  +--+--+--+--+--+--+--+--+
2 |  |wP|wP|  |  |wP|  |wP|
  +--+--+--+--+--+--+--+--+
1 |wR|  |  |  |wK|  |  |  |
  +--+--+--+--+--+--+--+--+
   a  b  c  d  e  f  g  h

*/