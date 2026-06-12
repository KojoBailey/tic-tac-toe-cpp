# CLI Tic-Tac-Toe - C++
This is a command-line implementation of the game Tic-Tac-Toe (aka Noughts & Crosses) in modern C++.

It was inspired by [this Rust entry](https://rosettacode.org/wiki/Tic-tac-toe#Rust) on Rosetta Code, although doesn't aim to replicate it.

The computer is incredibly simple and is only capable of placing symbols in random cells. I might revisit this project in future to make it more artificially intelligent.

All the code is in one `main.cpp`, partially because I'm lazy, but also because it helps make it evident that the project is fairly simple.

I make use of modern C++ features - that is, the latest stuff from C++23/26 - where it makes sense and where my compiler (Clang) actually lets me. This would typically not be something worth mentioning, but C++ programmers have a strange habit of avoiding new things despite their advantages.

I also make use of this code:

```cpp
#define loop while(true)
```

I cannot justify this - I just wanted to try it out.

## How To Play
The rules follow that of classic Tic-Tac-Toe.

There are two players, each using either noughts (`O`s) or crosses (`X`s). The first player to get 3 of their symbol in a row, column, or diagonal wins!

> [!TIP]
> Tic-Tac-Toe is a [solved game](https://en.wikipedia.org/wiki/Solved_game), meaning the first player can always win if they play optimally. Assuming you don't already know, can you figure out how?

This version of the game supports both Player vs Player (PvP) and Player vs Computer (PvC), although the computer just plays randomly.

## Contributing
If you happen to find a bug while inspecting the source code or playing the game, please open an [issue](https://github.com/KojoBailey/tic-tac-toe-cpp/issues) on GitHub.

[Pull requests](https://github.com/KojoBailey/tic-tac-toe-cpp/pulls) are enabled, but since this is a random, fun project, it's unlikely I'll merge any directly. Instead, I'd take suggestions and use them to figure out how to make the changes myself, for learning purposes.
