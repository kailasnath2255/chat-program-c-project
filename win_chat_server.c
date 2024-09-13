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
    SOCKET server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    char key[] = "mysecretkey"; // Simple encryption key

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // 1. Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 2. Bind socket to address
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_sock);
        WSACleanup();
        return 1;
    }

    // 3. Listen for connections
    listen(server_sock, 1);
    printf("Server listening on port %d...\n", PORT);

    // 4. Accept client connection
    int client_len = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
    if (client_sock == INVALID_SOCKET) {
        printf("Accept failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_sock);
        WSACleanup();
        return 1;
    }
    printf("Client connected!\n");

    // 5. Chat loop (receive and send messages)
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive encrypted message from client
        int len = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (len <= 0) {
            printf("Client disconnected\n");
            break;
        }

        // Decrypt the message using XOR
        xor_encrypt_decrypt(buffer, key, len, strlen(key));
        printf("Client: %s\n", buffer);

        // Get response from server user
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0'; // Remove newline character

        // Encrypt the response using XOR
        xor_encrypt_decrypt(buffer, key, strlen(buffer), strlen(key));

        // Send encrypted message to client
        send(client_sock, buffer, strlen(buffer), 0);
    }

    // 6. Close sockets and cleanup
    closesocket(client_sock);
    closesocket(server_sock);
    WSACleanup();

    return 0;
}
