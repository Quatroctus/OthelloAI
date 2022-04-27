# Othello AI
> [Othello(Reversi)](https://en.wikipedia.org/wiki/Reversi) is a board game where players try to dominate the board with their pieces. I used the MiniMax algorithm to implement an AI which can play against the player or another minimax agent. 

This project was implemented from scratch with a GUI implemented with OpenGL. As such this program cannot run on the OSU Engr servers as they do not have a graphics device. 

## Source Compiliation
This repository has compiliation support for Windows and some Linux distros.

### Windows
First step after cloning or downloading the source code is to run the following command.
`cmake CMakeLists.txt`
After running that command a Visual Studio solution(.sln) file will have been generated. Open that and you can easily build the solution file.

### Linux
First step after cloning or downloading the source code is to run the Makefile using make.
`make`
If the compilation fails it's likely due to the library files being compiled for my local WSL2 system. In which case you need to obtain the freetype and glfw3 library files.

---

## Running Othello

### Windows
Running the Othello AI from inside Visual Studio will require you to go to the 
`Solution Explorer -> Right Click Othello -> Properties -> Debugging`
Then in *Command Arguments* add human or minimax for the first player and second player. 
**IE:** `human minimax` **to run human as player 1 and minimax agent as player 2.**

### Linux
Running the Othello AI from terminal on linux is as simple as being in the root directory after compiling and running the following command
`./othello <player_type> <player_type>`
where `<player_type>` is either 'human' or 'minimax'.
