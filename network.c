#include "network.h"
#include "io.h"

/********************
 *
 * @Name: calculate_checksum
 * @Def: Calculates checksum for frame validation (sum of all bytes % 65536)
 * @Arg: frame = pointer to the frame
 * @Ret: Calculated checksum value
 *
 ********************/
unsigned short calculate_checksum(Frame *frame) {
    unsigned long sum = 0;
    unsigned char *bytes = (unsigned char *)frame;
    
    for (int i = 0; i < FRAME_SIZE - 2; i++) {
        sum += bytes[i];
    }
    return (unsigned short)(sum % 65536);
}

/********************
 *
 * @Name: validate_frame
 * @Def: Validates received frame using checksum
 * @Arg: frame = pointer to the frame to validate
 * @Ret: 1 if valid, 0 if invalid
 *
 ********************/
int validate_frame(Frame *frame) {
    unsigned short calc_checksum = calculate_checksum(frame);
    return (calc_checksum == frame->checksum);
}

/********************
 *
 * @Name: build_frame
 * @Def: Constructs a frame with given parameters
 * @Arg: frame = pointer to frame structure to fill
 *       type = frame type (hex)
 *       origin = sender's IP:Port string
 *       destination = destination realm name
 *       data = payload data
 *       data_len = length of data
 * @Ret: None
 *
 ********************/
void build_frame(Frame *frame, unsigned char type, char *origin, char *destination, char *data, unsigned short data_len) {
    memset(frame, 0, FRAME_SIZE);
    
    frame->type = type;
    
    strncpy(frame->origin, origin, 19);
    frame->origin[19] = '\0';
    
    strncpy(frame->destination, destination, 19);
    frame->destination[19] = '\0';
    
    frame->data_length = data_len;
    if (data_len > 0 && data != NULL) {
        memcpy(frame->data, data, data_len < 275 ? data_len : 275);
    }
    
    frame->checksum = calculate_checksum(frame);
}

/********************
 *
 * @Name: setup_listener
 * @Def: Creates a listening socket for incoming connections (like server.c)
 * @Arg: port = port number to listen on
 * @Ret: Socket fd on success, -1 on error
 *
 ********************/
int setup_listener(int port) {
    int fd_socket = 1;
    int opt = 1;

    fd_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_socket < 0) {
        write(1, "ERROR: Could not create socket\n", strlen("ERROR: Could not create socket\n"));
        return -1;
    }
    
    setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(fd_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        write(1, "ERROR: Could not bind to port\n", strlen("ERROR: Could not bind to port\n"));
        close(fd_socket);
        return -1;
    }

    if (listen(fd_socket, 10) < 0) {
        write(1, "ERROR: Could not listen on socket\n", strlen("ERROR: Could not listen on socket\n"));
        close(fd_socket);
        return -1;
    }
    
    // Set non-blocking mode for select/poll compatibility
    fcntl(fd_socket, F_SETFL, O_NONBLOCK);

    return fd_socket;
}

/********************
 *
 * @Name: connect_to_realm
 * @Def: Connects to another Maester (like client.c's openClientConn)
 * @Arg: ip = IP address to connect to
 *       port = port number
 * @Ret: Connected socket fd on success, -1 on error
 *
 ********************/
int connect_to_realm(char *ip, int port) {
    struct sockaddr_in s_addr;
    int fd_socket = -1;

    fd_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd_socket < 0) {
        write(1, "ERROR: Could not create socket\n", strlen("ERROR: Could not create socket\n"));
        return -1;
    }

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &s_addr.sin_addr) <= 0) {
        write(1, "ERROR: Could not convert IP address\n", strlen("ERROR: Could not convert IP address\n"));
        close(fd_socket);
        return -1;
    }

    if (connect(fd_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
        // Don't print error here - let caller handle (might be expected for async)
        close(fd_socket);
        return -1;
    }

    return fd_socket;
}

/********************
 *
 * @Name: send_frame
 * @Def: Sends a complete 320-byte frame (like client.c's write)
 * @Arg: sock = socket file descriptor
 *       frame = pointer to frame to send
 * @Ret: 1 on success, 0 on failure
 *
 ********************/
int send_frame(int sock, Frame *frame) {
    int sent = 0;

    frame->checksum = calculate_checksum(frame);
    
    sent = send(sock, frame, FRAME_SIZE, 0);
    
    if (sent != FRAME_SIZE) {
        return 0; // Partial send or error
    }
    return 1;
}

/********************
 *
 * @Name: receive_frame
 * @Def: Receives exactly 320 bytes into a frame (like server.c's readUntil pattern)
 * @Arg: sock = socket file descriptor
 *       frame = pointer to frame structure to fill
 * @Ret: 1 if valid frame received, 0 if error/invalid
 *
 ********************/
int receive_frame(int sock, Frame *frame) {
    int total_received = 0;
    int bytes_read;
    char *buffer = (char *)frame;
    
    // Read exactly FRAME_SIZE bytes (320 bytes)
    while (total_received < FRAME_SIZE) {
        bytes_read = recv(sock, buffer + total_received, FRAME_SIZE - total_received, 0);
        
        if (bytes_read <= 0) {
            // Connection closed or error
            return 0;
        }
        
        total_received += bytes_read;
    }
    
    // Validate checksum
    return validate_frame(frame);
}

/********************
 *
 * @Name: accept_connection
 * @Def: Accepts an incoming connection on the listening socket (non-blocking)
 * @Arg: listen_fd = the listening socket file descriptor
 * @Ret: Connected socket fd on success, -1 if no pending connection or error
 *
 ********************/
int accept_connection(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd;
 
    client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
 
    return client_fd;
}
 

/********************
 *
 * @Name: close_connection
 * @Def: Properly closes a socket connection
 * @Arg: sock = socket file descriptor
 * @Ret: None
 *
 ********************/
void close_connection(int sock) {
    close(sock);
}