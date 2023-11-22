#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "solver.h"

#define MAX_LEVEL 12

uint8_t level = 5;
uint8_t disks[3][MAX_LEVEL];
uint8_t inHand = 0;
uint8_t hand = 0;
int key;
const uint8_t colStops[3] = {12, 38, 65};
uint32_t moves = 0;
bool win = false;
bool randomize = false;
char info[80];
char save_header[] = "carnivion.labs.hanoi";

static void checkWin() {
	for (int i = 0; i < level; ++i) {
		if (disks[2][i] != i + 1) {
			win = false;
			return;
		}
	}
	win = true;
}

static void mvadddisk(int row, int colstop, int disk_width) {
	if (disk_width == 1)
		mvprintw(row, colStops[colstop] - 1, "<++>");
	else if (disk_width > 1) {
		int s = disk_width > 9 ? 2 : 1;
		mvaddch(row, colStops[colstop] - disk_width, '<');
		printw("%d", disk_width);
		for (int j = 0; j < disk_width - 1 - s; ++j)
			addch('-');
		addstr("++");
		for (int j = 0; j < disk_width - 1 - s; ++j)
			addch('-');
		printw("%d", disk_width);
		addch('>');
	}
}

void draw() {
	erase();

	mvprintw(0, 0, "Level: %d\n\n", level);

	if (inHand == 0)
		mvaddstr(2, colStops[hand], "vv");
	else
		mvadddisk(2, hand, inHand);
	for (int t = 0; t < 3; ++t)
		mvaddstr(4, colStops[t], "++");
	for (int i = 0; i < level; ++i) {
		for (int t = 0; t < 3; ++t) {
			if (disks[t][i] == 0)
				mvaddstr(5 + i, colStops[t], "++");
			else if (disks[t][i] > 0)
				mvadddisk(5 + i, t, disks[t][i]);
		}
	}

	if (strlen(info))
		mvprintw(LINES - 3, 0, "%s", info);
	mvprintw(LINES - 2, 0, "Moves: %d", moves);
	mvaddstr(LINES - 1, 0, "Press [q] to quit.");

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
		fwrite(save_header, sizeof(char), sizeof(save_header) / sizeof(char), file);
		fwrite(&level, sizeof(uint8_t), 1, file);
		fwrite(disks, sizeof(uint8_t), sizeof(disks) / sizeof(uint8_t), file);
		fwrite(&inHand, sizeof(uint8_t), 1, file);
		fwrite(&hand, sizeof(uint8_t), 1, file);
		fwrite(&moves, sizeof(uint32_t), 1, file);
		fwrite(&randomize, sizeof(bool), 1, file);
		fclose(file);
		printw("Progress is saved to %s\n", fn);
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
		fread(save_header, sizeof(char), sizeof(save_header) / sizeof(char), file);
		fread(&level, sizeof(uint8_t), 1, file);
		fread(disks, sizeof(uint8_t), sizeof(disks) / sizeof(uint8_t), file);
		fread(&inHand, sizeof(uint8_t), 1, file);
		fread(&hand, sizeof(uint8_t), 1, file);
		fread(&moves, sizeof(uint32_t), 1, file);
		fread(&randomize, sizeof(bool), 1, file);
		fclose(file);
		printw("Progress is loaded from %s\n", fn);
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

	do {
		if (win)
			continue;
		strcpy(info, "");

		switch (key) {
			case KEY_UP: {
				if (inHand)
					break;
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
				if (!inHand)
					break;
				for (int i = level - 1; i >= 0; --i) {
					if (!disks[hand][i]) {
						if ((i != level - 1) && (inHand > disks[hand][i + 1]))
							break;
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
				if (hand == (uint8_t)-1)
					hand = 2;
				break;
			}
			case KEY_RIGHT: {
				++hand;
				if (hand > 2)
					hand = 0;
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
			case 'a':
			case 'A': {
				if (inHand)
					strcpy(info, "Please put the plate down and let me take care of it.");
				else {
					hint();
					++moves;
				}
				break;
			}
			default:
				break;
		} // switch (key)

		checkWin();
		if (win)
			strcpy(info, "WIN!");
		draw();
	} while ((key = getch()) != 'q');

end:
	endwin();
	return 0;
}
