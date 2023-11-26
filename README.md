# Solver of Random Hanoi

This is my first project in C. A terminal-based game and solver that can randomly initializes puzzles, and lets you or your computer solve it. Win the game by moving all the disks to the rightmost column.

### Updates - v1.1

- Added hints and optimized the behavior of auto-solving.
- Optimized code and display.
- Please note, this version is NOT compatible with the progress saved in previous versions.

### Key features

- Randomly generated puzzles with different difficulty levels (3~12).
- Save and load the game progress.
- Watch how the computer solves optimally.

### Installation

This project depends on `ncurses`, please install it before compiling.

```apt install libncurses-dev```

Navigate to the project directory and run `make` command to build the program. The executable `hanoi` will appear in the same directory.

### Command line options

- If no options are supplied, it will start a game of level 5, and all disks are placed on the leftmost column as in classic.
- If a number between 3 and 12 is supplied, it will start a game with the corresponding level, and all disks are placed on the leftmost column as in classic.
- If a letter `r` is supplied, it will start a game of the default level, and the disks are randomly placed on all three columns, but follow the rules of the game.
- There should be no spaces between the number and the `r` option, and all other unknown options are ignored.

For example, run `./hanoi 8r` will start a game of level 8 with random initial configuration.

### Play and control

- Use the left and right arrow keys to move your hand over the different columns.
- Use the up arrow key to pick up the topmost disk of that column, if your hand is empty.
- Use the down arrow key to put down the disk on top of that column, if the rules allow it.
- Press [S] to save the game progress to a file. You will be prompted to enter the file name.
- Press [L] to load the game progress from a file. You will be prompted to enter the file name.
- Press [H] while no disk is in hand, it will tell you the next best movement.
- Press [A] while no disk is in hand, it will continuously move all the disks according to the optimal solution until the puzzle is solved.
- Press [A] again to stop the auto-solving.
- Press [Q] to quit the game. All unsaved progress will be lost.

### License

This project is licensed under the GPLv3. See the `LICENSE` for more details.
