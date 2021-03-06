#include "ChessProg.h"

//TODO: change pre_turn_verify
//TODO: add support to reset in main
//TODO: change all prawn to mM

//prints board in the following form:
//8| R _ _ n _ K _ _
//7| MM_ _ _ _ _ N
//6| _ _ M_ _ _ MM
//5| _ _ _ _ _ _ _ _
//4| _ _ _ q _ _ _ _
//3| _ b _ _ _ _ _ m
//2| m _ m_ _ k m_
//1| _ _ _ _ r _ _ _
// |-----------------
//   A B C D E F G H

void print_board(char board[BOARD_SIZE][BOARD_SIZE]) {
	int i, j;
	for (j = BOARD_SIZE - 1; j >= 0; j--) {
		printf("%d|", j + 1);
		for (i = 0; i < BOARD_SIZE; i++) {
			printf(" %c", board[i][j]);
		}
		printf(" |\n");
	}
	printf("  -----------------\n");
	printf("  ");
	for (j = 0; j < BOARD_SIZE; j++) {
		printf(" %c", (char )('A' + j));
	}
	printf("\n");
}

void init_board(char board[BOARD_SIZE][BOARD_SIZE]) {
	printf("initboard");
	//initializes empty rows
	for (int i = 0; i < 8; i++) {
		for (int j = 2; j < 6; j++) {
			board[i][j] = '_';
		}
	}
	//initializes rows of pawns
	for (int i = 0; i < 8; i++) {
		board[i][1] = 'p';
		board[i][6] = 'P';
	}
	//initializes rest of pieces
	board[0][0] = 'r';
	board[0][7] = 'R';
	board[1][0] = 'n';
	board[1][7] = 'N';
	board[2][0] = 'b';
	board[2][7] = 'B';
	board[3][0] = 'q';
	board[3][7] = 'Q';
	board[4][0] = 'k';
	board[4][7] = 'K';
	board[5][0] = 'b';
	board[5][7] = 'B';
	board[6][0] = 'n';
	board[6][7] = 'N';
	board[7][0] = 'r';
	board[7][7] = 'R';
}

//game loader from xml in the following structure:
//<?xml version="1.0" encoding="UTF-8"?>
//<game>
//	<current_turn>current</current_turn>
//	<game_mode>mode</game_mode ><difficulty>difficulty</difficulty>
//	<user_color>color</user_color>
//		<board>
//			<row_8>8 charactersthat represent the row's content</row_8>
//			<row_7>8 charactersthat represent the row's content</row_7>
//			<row_6>8 charactersthat represent the row's content</row_6>
//			<row_5>8 charactersthat represent the row's content</row_5>
//			<row_4>8 charactersthat represent the row's content</row_4>
//			<row_3>8 charactersthat represent the row's content</row_3>
//			<row_2>8 charactersthat represent the row's content</row_2>
//			<row_1>8 charactersthat represent the row's content</row_1>
//		</board>
//</game>
int load_game(char * path, char board[BOARD_SIZE][BOARD_SIZE]) {
	FILE *fxml;
	fxml = fopen(path, "r");
	if (fxml == NULL ) return 0;
	char buffer[255];
	fgets(buffer, 255, (FILE*) fxml); // reads first header line
	fgets(buffer, 255, (FILE*) fxml); // reads game header
	fgets(buffer, 255, (FILE*) fxml); // reads start_color tag
	start_color = strstr(buffer, "<current_turn>black") ? BLACK : WHITE;
	fgets(buffer, 255, (FILE*) fxml);
	game_mode = strstr(buffer, "<game_mode>1") ? 1 : 2;
	fgets(buffer, 255, (FILE*) fxml);
	char * difficulty_tag = strstr(buffer, "<difficulty>") + strlen("<difficulty>");
	if (difficulty_tag[0] != '<') {
		minimax_depth = difficulty_tag[0] - '0';
	}
	fgets(buffer, 255, (FILE*) fxml);
	char * user_color_tag = strstr(buffer, "<user_color>") + strlen("<user_color>");
	if (user_color_tag[0] != '<') { // reads user_color tag if exists
		user_color = strstr(buffer, "black") ? BLACK : WHITE;
	}
	setvbuf(stdout, NULL, _IONBF, 0);
	fgets(buffer, 255, (FILE*) fxml);
	char * boardTag = strstr(buffer, "<board>");
	if (boardTag) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				board[i][j] = EMPTY;
			}
		}
	}
	for (int j = BOARD_SIZE - 1; j >= 0; j--) {
		fgets(buffer, 255, (FILE*) fxml);
		for (int i = strlen("<row_i>  "); i < strlen("<row_i>  ") + BOARD_SIZE; i++) {
			board[i - strlen("<row_i>  ")][j] = (buffer[i] == '_') ? EMPTY : buffer[i];
		}
	}

	fclose(fxml);
	return 1;
}

//saves game to xml in the a specific structure described in load_game
int save_game(char * path, char board[BOARD_SIZE][BOARD_SIZE]) {
	FILE *fxml;
	fxml = fopen(path, "w");
	if (fxml == NULL ) return 0;
	char * start_color_tag;
	char * game_mode_tag;
	char * user_color_tag;
	fprintf(fxml, "<?xml version=\"%.1f\" encoding=\"%s\"?>\n", 0.1, "UTF-8");
	fprintf(fxml, "<%s>\n", "game");
	start_color_tag = (start_color == BLACK) ? "black" : "white";
	fprintf(fxml, "\t<%s>%s</%s>\n", "current_turn", start_color_tag, "computer_turn");
	game_mode_tag = (game_mode == 1) ? "1" : "2";
	fprintf(fxml, "\t<%s>%s</%s>\n", "game_mode", game_mode_tag, "game_mode");
	if (game_mode == 1) {
		fprintf(fxml, "\t<%s>%d</%s>\n", "difficulty", minimax_depth, "difficulty");
		user_color_tag = (user_color == 0) ? "white" : "black";
		fprintf(fxml, "\t<%s>%s</%s>\n", "user_color", user_color_tag, "user_color");
	} else {
		fprintf(fxml, "\t<%s>%s</%s>\n", "difficulty", "", "difficulty");
		fprintf(fxml, "\t<%s>%s</%s>\n", "user_color", "", "user_color");
	}
	fprintf(fxml, "\t<%s>\n", "board");
	for (int j = BOARD_SIZE; j > 0; j--) {
		fprintf(fxml, "\t\t<row_%d>", j);
		for (int i = 0; i < BOARD_SIZE; i++)
			fprintf(fxml, "%c", board[i][j - 1]);
		fprintf(fxml, "</row_%d>\n", j);
	}
	fprintf(fxml, "\t</%s>\n", "board");
	fprintf(fxml, "</%s>\n", "game");
	fclose(fxml);
	return 1;
}

//gets users input, with max length set to 1024
char* input_to_str(FILE* pFile) {
	char * user_input = malloc(sizeof(char)*1024);
	fgets(user_input, 1024, pFile);
	user_input[strlen(user_input) - 1] = '\0';
	return user_input;
}

void print_settings_sole_player() {
	printf(SETTING_SOLE_PLAYER, game_mode, minimax_depth, user_color == WHITE ? "WHITE" : "BLACK");
}

void print_settings_two_players() {
	printf(SETTING_TWO_PLAYERS, game_mode);
}

//Sets the game according to the users input, in case the user doesn't starts the game immediately.
//supports the following commands:
//game_mode, difficulty x, user_color x, load x, default, print_setting, quit

void conosle_settings_mode(char* str, char board[BOARD_SIZE][BOARD_SIZE]) {
	char * command;
	command = strtok(str, " ");
	if (strcmp(command, "game_mode") == 0) {
		int x = atoi(strtok(NULL, " "));
		if (x == TWO_PLAYERS || x == PLAYER_VS_COMPUTER) {
			game_mode = x;
			printf("Game mode is set to %s\n", game_mode == 1 ? "1 player" : "2 players");
		} else {
			printf(WRONG_GAME_MODE);
		}
	} else if (strcmp(command, "user_color") == 0) {
		char * color = strtok(NULL, " ");
		printf("user_color");
		if (game_mode == 2)
			printf(ILLEGAL_COMMAND);
		else
			user_color = (strcmp(color, "black") == 0) ? BLACK : WHITE;
	} else if (strcmp(command, "difficulty") == 0) {
		int x = atoi(strtok(NULL, " "));
		if (game_mode == TWO_PLAYERS)
			printf(ILLEGAL_COMMAND);
		else {
			if (x <= 4 && x >= 1)
				minimax_depth = x;
			else if (x == 5) {
				printf(EXPERT_LEVEL_NOT_SUPPORTS);
				printf(", please choose a value between 1 to 4:\n");
			} else {
				printf(WRONG_MINIMAX_DEPTH);
			}
		}
	} else if (strcmp(command, "print_setting") == 0) {
		game_mode == 1 ? print_settings_sole_player() : print_settings_two_players();
	} else if (strcmp(command, "load") == 0) {
		char * path = strtok(NULL, " ");
		int load_success = load_game(path, board);
		if (!load_success) {
			printf(WRONG_FILE_NAME);
		}
		if (minimax_depth == 5) {
			printf(EXPERT_LEVEL_NOT_SUPPORTS);
			printf("\n");
		}
	} else if (strcmp(command, "default") == 0) {
		game_mode = PLAYER_VS_COMPUTER;
		minimax_depth = 2;
		user_color = WHITE;
	} else printf(ILLEGAL_COMMAND);
	return;
}

// verifies before every turn if it is a check pos, mate pose or tie pos.
int pre_turn_verify(char board[BOARD_SIZE][BOARD_SIZE], COLOR color) {
	get_all_moves(board, color);
	if (is_check(board, color) == 1 && moves_head == NULL ) return LOSE_POS;
	if (is_check(board, color) != 1 && moves_head == NULL ) return TIE_POS;
	if (is_check(board, color) == 1 && moves_head != NULL ) return CHECK_POS;
	return GAME_ON;
}

// manages the computer's turn
void computer_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color) {
	curr_player = color;
	if (moves_head != NULL ) {
		alpha_beta_minimax(board, color, 0, -500, 500);
		Move * move2do = best_move;
		exc_move(board, move2do);
		if (!gui_mode) {
			printf("Computer: move %s at ", get_piece_full_name_by_char(board[move2do->dest.col][move2do->dest.row]));
			print_move(move2do);
		}
		Move* copied_move = copy_move(move2do);
		copied_move->prev = curr_move;
		curr_move = copied_move;
	}
	clear_old_moves(moves_head);
}

// manages the users turn, game state user input loop
void user_turn(char board[BOARD_SIZE][BOARD_SIZE], COLOR color) {
	curr_player = color;
	char *command = NULL;
	char *command_type;
	Move* new_move = NULL;
	print_board(board);
	while (1) {
		printf(ENTER_YOUR_MOVE, color == WHITE ? "white" : "black");
		command = input_to_str(stdin);
		char delimiter[] = " <,>to";
		command_type = strtok(command, " ");
		if (strcmp(command_type, "quit") == 0 || strcmp(command_type, "reset") == 0) {
			game_on = 0;
			break;
		} else if (strcmp(command_type, "save") == 0) {
			char * file_name = strtok(NULL, " ");
			if (!save_game(file_name, board)) {
				printf(WRONG_FILE_NAME);
				continue;
			}
		} else if (strcmp(command_type, "undo") == 0) {
			if (game_mode == TWO_PLAYERS) {
				printf(WRONG_UNDO_GAME_MODE);
				continue;
			}
			if (curr_move == NULL || undo_count == 3) {
				printf(EMPTY_HISTORY);
				continue;
			} else {
				undo_move(board, get_opposite_color());
				undo_move(board, user_color);
				undo_count++;
				continue;
			}
		} else if (strcmp(command_type, "move") == 0) {
			if (new_move != NULL ) clear_old_moves(new_move);
			new_move = malloc(sizeof(Move));
			new_move->next = NULL;
			new_move->piece.row = atoi(strtok(NULL, delimiter)) - 1;
			new_move->piece.col = strtok(NULL, delimiter)[0] - 'A';
			if (!is_valid_pos(new_move->piece)) {
				printf(WRONG_POSITION);
				continue;
			}
			new_move->dest.row = atoi(strtok(NULL, delimiter)) - 1;
			new_move->dest.col = strtok(NULL, delimiter)[0] - 'A';
			if (!is_valid_pos(new_move->dest)) {
				printf(WRONG_POSITION);
				continue;
			}
			if (!is_valid_piece(board, new_move, color)) {
				printf(NO_PIECE, color == WHITE ? "white" : "black");
				continue;
			}
			Move * move2do = is_valid_move(moves_head, new_move);
			if (move2do == NULL ) {
				printf(ILLEGAL_MOVE);
				continue;
			}
			exc_move(board, move2do);
			Move* copied_move = copy_move(move2do);
			copied_move->prev = curr_move;
			curr_move = copied_move;
			undo_count = 0;
			break;
		} else printf(ILLEGAL_COMMAND);
	}
	free(command);
	clear_old_moves(new_move);
	clear_old_moves(moves_head);
}

//checks if there is a cheak,mate or tie and prints alerts to the console if needed
void console_alert(int alert) {
	if (alert == LOSE_POS || alert == TIE_POS) {
		if (alert == LOSE_POS)
			printf(curr_player == WHITE ? BLACK_WIN : WHITE_WIN);
		else printf(TIE);
		game_on = 0;
	} else if (alert == CHECK_POS) {
		printf(CHECK);
	}
}

//checks if there is a cheak,mate or tie to be shown on the game screen (check/mate/tie)
void gui_alert(int alert) {
	if (alert != GAME_ON) {
		COLOR alert_color = curr_player == WHITE ? BLACK : WHITE;
		if (alert != CHECK_POS) game_on = 0;
//		alert_state(alert, alert_color); // this func alert special states in chess ui
	}
}

int main(int argc, char * argv[]) {
	if (argc == 2) gui_mode = strcmp(argv[1], "gui") == 0 ? 1 : 0;
//todo change gui_mode to 1
	gui_mode = 1;
	char board[BOARD_SIZE][BOARD_SIZE];
	int start = 0;
//  next lines to check load_board and load_game
//	init_board(board);
//	load_game("chess1.xml", board);
//	print_board(board);
//	save_game("check_xml_save_game", board);
//setting state
	if (gui_mode) {
		//TODO: change back order
		init_board(board);
		print_board(board);
		duplicate_board(board,gui_board);
		print_board(gui_board);
		start = gui_setting_mode();
		printf("Finished Start cmd in if gui_mode");
	} //setting gui
	return 0;
}
//	else { //setting console
//		init_board(board);
//		printf(ENTER_SETTINGS);
//		char *command = input_to_str(stdin);
//		while (strcmp(command, "quit") != 0) {
//			if (strcmp(command, "start") == 0) {
//				if (is_valid_board(board)) {
//					start = 1;
//					break;
//				} else printf(WROND_BOARD_INITIALIZATION);
//			} else
//				conosle_settings_mode(command, board);
//			free(command);
//			printf(ENTER_SETTINGS);
//			command = input_to_str(stdin);
//		}
//		free(command);
//	}

//	if (start) {
//		while (1) {
//			if (game_mode == PLAYER_VS_COMPUTER) {
//				int turn;
//				if (user_color == start_color) {
//					//user starts
//					curr_player = user_color;
//					turn = pre_turn_verify(board, curr_player);
//					if (gui_mode) { //gui_mode
//						gui_alert(turn);
//						Move * new_move = NULL;
//						Move * move2do = NULL;
//						while (move2do == NULL ) {
//							new_move = gui_game_mode(board);
//							if (new_move != NULL ) move2do = is_valid_move(moves_head, new_move);
//							if (!game_on || new_move == NULL ) break;
//						}
//						if (game_on && move2do != NULL )
//							exc_move(board, move2do);
//						else if (game_on && move2do == NULL ) {
//							start = gui_setting_mode();
//							duplicate_board(gui_board, board);
//							continue;
//						} else
//							break;
//					} else { //console mode
//						console_alert(turn);
//						if (game_on)
//							user_turn(board, curr_player);
//						else
//							break;
//					}
//
//					curr_player = !user_color;
//					turn = pre_turn_verify(board, curr_player);
//					if (gui_mode) {
//						gui_alert(turn);
//						if (game_on)
//							computer_turn(board, curr_player);
//						else
//							break;
//					} else {
//						console_alert(turn);
//						if (game_on)
//							computer_turn(board, curr_player);
//						else
//							break;
//					}
//				}
//				//comp starts
//				else {
//					curr_player = !user_color;
//					turn = pre_turn_verify(board, curr_player);
//					if (gui_mode) {
//						gui_alert(turn);
//						if (game_on)
//							computer_turn(board, curr_player);
//						else
//							break;
//					} else {
//						console_alert(turn);
//						if (game_on)
//							computer_turn(board, curr_player);
//						else
//							break;
//					}
//
//					curr_player = user_color;
//					turn = pre_turn_verify(board, user_color);
//					if (gui_mode) {
//						gui_alert(turn);
//						Move * new_move = NULL;
//						Move * move2do = NULL;
//						while (move2do == NULL ) {
//							new_move = gui_game_mode(board);
//							if (new_move != NULL ) move2do = is_valid_move(moves_head, new_move);
//							if (!game_on || new_move == NULL ) break;
//						}
//						if (game_on && move2do != NULL )
//							exc_move(board, move2do);
//						else if (game_on && move2do == NULL ) {
//							start = gui_setting_mode();
//							duplicate_board(gui_board, board);
//							continue;
//						} else
//							break;
//					} else {
//						console_alert(turn);
//						if (game_on)
//							user_turn(board, curr_player);
//						else
//							break;
//					}
//				}
//			}
//			// 2 players
//			if (game_mode == 2) {
//				int turn;
//				curr_player = start_color;
//				turn = pre_turn_verify(board, curr_player);
//				if (gui_mode) {
//					gui_alert(turn);
//					Move * new_move = NULL;
//					Move * move2do = NULL;
//					while (move2do == NULL ) {
//						new_move = gui_game_mode(board);
//						if (new_move != NULL ) move2do = is_valid_move(moves_head, new_move);
//						if (!game_on || new_move == NULL ) break;
//					}
//					if (game_on && move2do != NULL )
//						exc_move(board, move2do);
//					else if (game_on && move2do == NULL ) {
//						start = gui_setting_mode();
//						duplicate_board(gui_board, board);
//						continue;
//					} else
//						break;
//				} else {
//					console_alert(turn);
//					if (game_on)
//						user_turn(board, curr_player);
//					else
//						break;
//				}
//
//				curr_player = !start_color;
//				turn = pre_turn_verify(board, curr_player);
//				if (gui_mode) {
//					gui_alert(turn);
//					Move * new_move = NULL;
//					Move * move2do = NULL;
//					while (move2do == NULL ) {
//						new_move = gui_game_mode(board);
//						if (new_move != NULL ) move2do = is_valid_move(moves_head, new_move);
//						if (!game_on || new_move == NULL ) break;
//					}
//					if (game_on && move2do != NULL )
//						exc_move(board, move2do);
//					else if (game_on && move2do == NULL ) {
//						start = gui_setting_mode();
//						duplicate_board(gui_board, board);
//						continue;
//					} else
//						break;
//				} else {
//					console_alert(turn);
//					if (game_on)
//						user_turn(board, curr_player);
//					else
//						break;
//				}
//
//			}
//		}
//	TODO verify the need for this last section
//		if (!gui_mode) {
//			char *command = input_to_str(stdin);
//			free(command);
//		}
//	}
//	printf(QUIT_MSG);
//	free(curr_move);
//	return 0;
//}

