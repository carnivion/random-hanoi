#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "solver.h"

#define MAX_LEVEL 12
#define SPEED 100
#define SAVE_HEADER "carnivion.labs.hanoi.v1_1"

uint8_t level = 5;
uint8_t disks[3][MAX_LEVEL];
uint8_t inHand = 0;
uint8_t hand = 0;
int key;
const uint8_t colStops[3] = {12, 38, 65};
uint16_t moves = 0;
bool win = false;
bool randomize = false;
char info[80];
bool autoSolving = false;

static void checkWin() {
	for (int i = 0; i < level; ++i) {
		if (disks[2][i] != i + 1) {
			win = false;
			return;
		}
	}
	win = true;
	strcpy(info, "WIN!");
	timeout(-1);
	autoSolving = false;
}

static void mvadddisk(int row, int colstop, int disk_width) {
	if (disk_width == 1)
		mvprintw(row, colStops[colstop] - 1, "<||>");
	else if (disk_width > 1) {
		int s = disk_width > 9 ? 2 : 1;
		mvaddch(row, colStops[colstop] - disk_width, '<');
		printw("%d", disk_width);
		for (int j = 0; j < disk_width - 1 - s; ++j)
			addch('=');
		addstr("||");
		for (int j = 0; j < disk_width - 1 - s; ++j)
			addch('=');
		printw("%d", disk_width);
		addch('>');
	}
}

static void draw() {
	erase();

	mvprintw(0, 0, "Level: %d", level);

	if (inHand == 0)
		mvaddstr(2, colStops[hand], "vv");
	else
		mvadddisk(2, hand, inHand);
	for (int t = 0; t < 3; ++t)
		mvaddstr(4, colStops[t], "||");
	for (int i = 0; i < level; ++i) {
		for (int t = 0; t < 3; ++t) {
			if (disks[t][i] == 0)
				mvaddstr(5 + i, colStops[t], "||");
			else if (disks[t][i] > 0)
				mvadddisk(5 + i, t, disks[t][i]);
		}
	}

	if (strlen(info))
		mvprintw(LINES - 3, 0, "%s", info);
	mvprintw(LINES - 2, 0, "Moves: %d", moves);
	if (win)
		mvaddstr(LINES - 1, 0, "[q]uit");
	else if (autoSolving)
		mvaddstr(LINES - 1, 0, "[a] = stop, [q]uit");
	else
		mvaddstr(LINES - 1, 0, "[arrows] = move, [s]ave, [l]oad, [h]int, [a]uto, [q]uit");

	refresh();
}

static void toSave() {
	char fn[21];
	FILE *file;

	erase();
	printw("File to save:");
	echo();
	getnstr(fn, 20);
	noecho();
	file = fopen(fn, "wb");
	if (file != NULL) {
		fwrite(SAVE_HEADER, 1, sizeof(SAVE_HEADER), file);
		fwrite(&level, sizeof(level), 1, file);
		fwrite(disks, sizeof(disks[0]), sizeof(disks) / sizeof(disks[0]), file);
		fwrite(&inHand, sizeof(inHand), 1, file);
		fwrite(&hand, sizeof(hand), 1, file);
		fwrite(&moves, sizeof(moves), 1, file);
		fwrite(&randomize, sizeof(randomize), 1, file);
		fclose(file);
		printw("Progress is saved to %s, Press any key to continue.\n", fn);
	} else
		printw("Error saving the file.");
	getch();
}

static void toLoad() {
	char fn[21];
	FILE *file;

	erase();
	printw("File to load:");
	echo();
	getnstr(fn, 20);
	noecho();
	file = fopen(fn, "rb");
	if (file != NULL) {
		fseek(file, sizeof(SAVE_HEADER), SEEK_SET);
		fread(&level, sizeof(level), 1, file);
		fread(disks, sizeof(disks[0]), sizeof(disks) / sizeof(disks[0]), file);
		fread(&inHand, sizeof(inHand), 1, file);
		fread(&hand, sizeof(hand), 1, file);
		fread(&moves, sizeof(moves), 1, file);
		fread(&randomize, sizeof(randomize), 1, file);
		fclose(file);
		printw("Progress is loaded from %s, Press any key to continue.\n", fn);
		strcpy(info, "");
	} else
		printw("Error loading the file.\n");
	getch();
}

int main(int argc, char *argv[]) {
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);

	if (argc > 1) {
		int num = 0;
		char ch = '\0';
		bool prompt = false;
		for (int i = 0; argv[1][i] != '\0'; ++i) {
			if (isdigit(argv[1][i]))
				num = num * 10 + (argv[1][i] - '0');
			else {
				ch = argv[1][i];
				break;
			}
		}
		if (num >= 3 && num <= MAX_LEVEL)
			level = num;
		else {
			prompt = true;
			printw("%d is not a valid level, use the default level.\n", num);
		}
		if (ch == 'r')
			randomize = true;
		else if (ch != '\0') {
			prompt = true;
			printw("%c is not a valid option and is ignored.\n", ch);
		}
		if (prompt) {
			printw("Press any key to continue. Press [q] to quit.\n");
			refresh();
			key = getch();
			if (key == 'q')
				goto end;
		}
	}

	if (randomize) {
		srand(time(NULL));
		for (int i = level; i > 0; --i) {
			int t;
			do
				t = rand() % 3;
			while (i == level && t == 2);

			for (int j = level - 1; j >= 0; --j) {
				if (disks[t][j] == 0) {
					disks[t][j] = i;
					break;
				}
			}
		}
	} else {
		for (int i = 0; i < level; ++i) {
			disks[0][i] = i + 1;
			disks[1][i] = 0;
			disks[2][i] = 0;
		}
	}

	while (1) {
		if (!win) {
			if (autoSolving) {
				strcpy(info, "Auto-solving...");
				moveOne();
				++moves;
				napms(SPEED);
				if (key == 'a' || key == 'A') {
					timeout(-1);
					autoSolving = false;
					strcpy(info, "");
				}
			} else {
				strcpy(info, "");
				switch (key) {
					case KEY_UP: {
						if (inHand) break;
						for (int i = 0; i < level; ++i) {
							if (disks[hand][i]) {
								inHand = disks[hand][i];
								disks[hand][i] = 0;
								break;
							}
						}
						break;
					}
					case KEY_DOWN: {
						if (!inHand) break;
						for (int i = level - 1; i >= 0; --i) {
							if (!disks[hand][i]) {
								if ((i != level - 1) && (inHand > disks[hand][i + 1])) break;
								disks[hand][i] = inHand;
								inHand = 0;
								++moves;
								break;
							}
						}
						break;
					}
					case KEY_LEFT: {
						--hand;
						if (hand == (uint8_t)-1) hand = 2;
						break;
					}
					case KEY_RIGHT: {
						++hand;
						if (hand > 2) hand = 0;
						break;
					}
					case 'S':
					case 's': {
						toSave();
						break;
					}
					case 'L':
					case 'l': {
						toLoad();
						break;
					}
					case 'h':
					case 'H': {
						if (inHand)
							strcpy(info, "Please put the plate down and let me examine it.");
						else {
							Movement m = hint();
							sprintf(
								info,
								"Move <%d> from %s to %s",
								m.disk,
								m.from == 1 ? "MIDDLE" : m.from == 0 ? "LEFT" : "RIGHT",
								m.to == 1 ? "MIDDLE" : m.to == 0 ? "LEFT" : "RIGHT"
							);
						}
						break;
					}
					case 'a':
					case 'A': {
						if (inHand)
							strcpy(info, "Please put the plate down and let me take care of it.");
						else {
							timeout(0);
							autoSolving = true;
						}
						break;
					}
					default:
						break;
				}  // switch (key)
			}  // if (!autoSolving)
			checkWin();
			draw();
		}  // if (!win)
		key = getch();
		if (key == 'q' || key == 'Q') break;
	}  // while (1)

end:
	endwin();
	return 0;
}
