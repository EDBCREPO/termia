# TERMIA | A Gemini CLI Chat Client (using nodepp)

![ezgif-7b51292b9724d0](https://github.com/user-attachments/assets/58cc01ca-66dc-4180-b45a-19f0760f38ee)

This is a simple command-line interface (CLI) chat client built with the nodepp framework (a C++ analogue to Node.js) designed to interact with the Google Gemini API.

It allows you to type prompts directly into your terminal and see the AI's response streamed back, character by character.

## Getting Started

```txt
# Before running this client, you'll need:

- The nodepp environment and compiler (requires a C++ environment).
- A Google Gemini API Key.
```

```bash
g++ -o main main.cpp -I./include -lssl -lcrypto ; ./main
```

## Windows stream::pipe( fs::std_input() ) Bug

Okay, technically this is not a bug, but officially, Windows does not let us work with standard I/O files asynchronously (thanks, Windows!). This means the line stream::pipe( fin ); might not behave as expected for non-blocking input on Windows systems.

But this is not the end! Since nodepp supports workers, it is possible to create a dedicated thread to handle incoming std_input messages asynchronously in a different thread, allowing the main event loop to remain responsive:

```cpp
void worker_file_pipe( file_t file ) {
    generator::stream::pipe arg;
    worker::add( arg, file );
}
```
## How it works

[![IMAGE](https://miro.medium.com/v2/resize:fit:720/format:webp/1*7RczdtorVGJNy5aWHmJXHw.jpeg)](https://medium.com/p/21b1b3709cf2)

[Click Here](https://medium.com/p/21b1b3709cf2)
