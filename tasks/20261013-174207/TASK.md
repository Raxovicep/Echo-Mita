# Building sources and extarnals

- STATUS: CLOSED 
- PRIORITY: 100
- TAGS: build

Here's how I want to build my project

- Create a `build` folder if it's not exist
- Version of project via git, compiler version and when it built like [tatr](https://github.com/tsoding/tatr)
- If a file edit, nob rebuilds that file and link it executable
- If a library edit, nob links that lib to executable
- Options for nob
  - -dev - build project in dev mode, that enables fsanitize
  - -no-build - don't build project
  - -run - run an executable
    - After -run user should provide some arguments for executable
      - `./nob -no-build -run resources/Voice.wav`
  - flag `-force` to force rebuild
