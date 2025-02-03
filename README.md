# ft_irc - Internet Relay Chat Server

## Introduction
This project consists of implementing an IRC (Internet Relay Chat) server in C++98. The server will handle multiple clients simultaneously and support basic IRC functionalities, allowing users to communicate in real-time through channels and private messages.

## Features
- Authentication with a password
- Nickname and username management
- Support for multiple clients
- Channel management
  - Create and join channels
  - Send and receive messages
  - Channel operator commands:
    - `KICK` - Remove a user from a channel
    - `INVITE` - Invite a user to a channel
    - `TOPIC` - Change or view the channel topic
    - `MODE` - Modify channel settings (invite-only, topic restrictions, password protection, operator privileges, user limits)
- Communication via TCP/IP (IPv4 or IPv6)
- Non-blocking I/O using `poll()` (or equivalent)
- Compliance with C++98 standard

## Installation
1. Clone the repository:
   ```sh
   git clone https://https://github.com/Moyaxzan/42-ft_irc.git
   cd 42-ft_irc
   ```
2. Compile the project using the provided Makefile:
   ```sh
   make
   ```

## Usage
Run the server with the following command:
```sh
./ircserv <port> <password>
```
- `<port>`: The port on which the server will listen for incoming connections.
- `<password>`: The password required for clients to connect.

Example:
```sh
./ircserv 6667 mypassword
```

## Testing
To test the server, you can use an IRC client or `nc` (netcat):
```sh
nc -C 127.0.0.1 6667
```
You can then send commands, e.g.:
```sh
NICK user1
USER user1 0 * :Real Name
JOIN #channel
PRIVMSG #channel :Hello everyone!
```

## Requirements
- C++98 standard compliance
- Only allowed system functions: `socket`, `close`, `setsockopt`, `getsockname`, `getprotobyname`, `gethostbyname`, `getaddrinfo`, `freeaddrinfo`, `bind`, `connect`, `listen`, `accept`, `htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`, `send`, `recv`, `signal`, `sigaction`, `lseek`, `fstat`, `fcntl`, `poll` (or equivalent)
- No use of external libraries (e.g., Boost)

## License
This project is for educational purposes and follows the rules of the 42 curriculum.

