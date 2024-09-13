#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

#define PORT 8080
#define BUFFER_SIZE 1024

// XOR Encryption/Decryption function
void xor_encrypt_decrypt(char *message, char *key, int message_len, int key_len) {
    for (int i = 0; i < message_len; i++) {
        message[i] ^= key[i % key_len];
    }
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char key[] = "mysecretkey"; // Simple encryption key

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Connect to localhost

    // 2. Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Connected to the server!\n");

    // 3. Chat loop (send and receive messages)
    while (1) {
        // Get input from client user
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0'; // Remove newline character

        // Encrypt the message using XOR
        xor_encrypt_decrypt(buffer, key, strlen(buffer), strlen(key));

        // Send encrypted message to server
        send(sock, buffer, strlen(buffer), 0);

        // Receive encrypted response from server
        int len = recv(sock, buffer, BUFFER_SIZE, 0);
        if (len <= 0) {
            printf("Server disconnected\n");
            break;
        }

        // Decrypt the message using XOR
        xor_encrypt_decrypt(buffer, key, len, strlen(key));
        buffer[len] = '\0';  // Null-terminate the decrypted message
        printf("Server: %s\n", buffer);
    }

    // 4. Close socket and cleanup
    closesocket(sock);
    WSACleanup();

    return 0;
}
