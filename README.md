# Dhruv HTTP Server 💻

Built an HTTP/1.1 static file server in **C** using raw Linux sockets. Implemented TCP connection handling, HTTP request parsing, and static content serving to gain hands-on understanding of networking and systems programming fundamentals.

## ⚠️ Disclaimer: Educational and Experimental Project

**Dhruv is an educational project and is currently a work in progress.**

  * **NOT for Production Use:** This server is **not secure** and is **not intended for use in a production environment**. It lacks many critical security features, error handling, and performance optimizations found in production-grade servers.
  * **Vulnerabilities:** Due to its low-level nature and focus on core mechanics, this server **may contain security vulnerabilities** (such as buffer overflows, improper input validation, etc.). Use and testing should be confined to controlled, non-critical environments.

## ✨ Features

  * Handles basic **HTTP/1.1** requests.
  * Serves static content from the `public/` directory.
  * Built purely with C and raw Linux TCP sockets.

## 🛠️ Project Structure

The repository is organized as follows:

```
.
├── include/
│   ├── network.h         # Header for network-related functions and structures
│   ├── handle_client.h   # Header for handle client functions
│   └── util.h            # Header for util functions
│
├── public/               # The document root for serving static files
│   ├── index.html        # Main landing page (e.g., /)
│   ├── main.css          # Stylesheet
│   └── index.js          # Client-side JavaScript
│
├── src/
│   ├── main.c            # Entry point of the server
│   ├── handle_client.c   # Implementation of handle client functions
│   ├── network.c         # Implementation of network functions (socket creation, listening, etc.)
│   └── util.c            # Utility functions (e.g., string manipulation, logging)
│
├── .gitignore            # Files/directories to ignore in Git
├── Makefile              # Build instructions
└── client.c              # Optional: A simple C client to test the server
```
## 🚀 Getting Started

### Prerequisites

  * A Linux environment (the server relies on specific Linux socket APIs).
  * **GCC** (GNU Compiler Collection).
  * **Make**.

### Building the Server

1.  Clone the repository:
    ```bash
    git clone https://github.com/Dhiraj275/dhruv
    cd dhruv
    ```
2.  Compile the project using the provided `Makefile`:
    ```bash
    make server
    ```
    This will generate an executable file, named `server`

### Running the Server

1.  Execute the compiled server program.
    ```bash
    ./server
    ```
2.  Open your web browser and navigate to:
    ```
    http://localhost:8080
    ```

## 💡 Learning Goals

This project is a journey through:

  * **Socket Programming:** Understanding `socket()`, `bind()`, `listen()`, `accept()`, `send()`, and `recv()`.
  * **TCP/IP:** Managing raw TCP connections and data streams.
  * **HTTP Protocol:** Parsing HTTP request lines and headers, and correctly formatting HTTP responses.
  * **C Programming:** Gaining proficiency in memory management, pointer manipulation, and working with complex data structures in a systems programming context.

## 🤝 Contributing

While this is primarily a personal learning project, suggestions and bug reports focusing on the **educational goals and low-level C implementation** are welcome\!


