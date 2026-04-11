#ifndef NETWORK_H
#define NETWORK_H

#include "types.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// Frame structure exactly as per Annex II (320 bytes total)
typedef struct {
    unsigned char type;           // 1 byte
    char origin[20];              // IP:Port of sender (e.g., "192.168.1.3:9003")
    char destination[20];         // Name of destination realm
    unsigned short data_length;   // 2 bytes
    char data[275];               // 320 - 1 - 20 - 20 - 2 - 2 = 275 bytes
    unsigned short checksum;      // 2 bytes
} Frame;

#define FRAME_SIZE 320

// Frame types from protocol
#define TYPE_ALLIANCE_REQUEST 0x01
#define TYPE_ALLIANCE_RESPONSE 0x03
#define TYPE_PRODUCT_LIST_REQUEST 0x11
#define TYPE_PRODUCT_LIST_RESPONSE 0x12
#define TYPE_ORDER_REQUEST 0x14
#define TYPE_ACK 0x31
#define TYPE_NACK 0x69
#define TYPE_DISCONNECT 0x27
#define TYPE_ERROR_UNKNOWN_REALM 0x21
#define TYPE_ERROR_UNAUTHORIZED 0x25

// Function prototypes
int setup_listener(int port);
int connect_to_realm(char *ip, int port);
int send_frame(int sock, Frame *frame);
int receive_frame(int sock, Frame *frame);
unsigned short calculate_checksum(Frame *frame);
int validate_frame(Frame *frame);
void build_frame(Frame *frame, unsigned char type, char *origin, char *destination, char *data, unsigned short data_len);
void close_connection(int sock);

#endif