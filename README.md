# Echo Mita
[![Banner](./resources/pictures/header.jpg)](https://store.steampowered.com/app/2527500/MiSide/)

![Static Badge](https://img.shields.io/badge/language-blue?style=for-the-badge&logo=C)
![Static Badge](https://img.shields.io/badge/chatGPT-green?style=for-the-badge&logo=openai&logoColor=white)

The Echo MIta is an unofficial voice assistant for PC written in C.


[DEMO]()

## About

**Echo Mita** is an open-source voice assistant inspired by the project [Jarvis](https://github.com/Priler/jarvis) and the game [MiSide](https://store.steampowered.com/app/2527500/MiSide/).

It is built with a **fully modular architecture**, allowing developers to customize nearly every aspect of the assistant — including behavior, voice interaction, and internal components. This flexibility makes Echo Mita suitable both for experimentation and for building personalized voice-driven systems.

The project focuses on **extensibility, simplicity, and control**, enabling users to adapt Mita to different environments and use cases without being limited by a fixed design.

## Quick start
We are using the `nob` build system. Before using nob you need to bootstrap it. Just compile it with the avalible C compiler.
```bash
cc -o nob nob.c # ONLY ONCE!
./nob
./build/Mita
# or
./nob -no-build -run
```
The nob has own flags. To check it, just run `./nob -help`.

## Project Wiki

## Commands

## Contributing
Feel free to contribute the project. Write issues, bug reports, ideas, etc. Follow the convention to contribute the project, [CONTRIBUTING](CONTRIBUTING.md). 

## Support the author

## License
The Echo Mita is free and open-source software licensed under the GNU General Public License v3.0. 
