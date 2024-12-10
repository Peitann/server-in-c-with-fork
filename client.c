#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080           // Port server
#define BUFFER_SIZE 1024    // Ukuran buffer untuk data yang dikirim/diterima

int main() {
    int sock = 0; // Variabel untuk socket
    struct sockaddr_in server_addr; // Struktur alamat server
    char buffer[BUFFER_SIZE] = {0}; // Buffer untuk menerima pesan
    char message[BUFFER_SIZE];     // Buffer untuk mengirim pesan

    // Buat socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Konfigurasi alamat server
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // Konversi port ke network byte order

    // Konversi alamat IP ke binary dan simpan di struktur
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

    // Hubungkan ke server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to server. Type messages to send:\n");

    // Loop utama untuk komunikasi dengan server
    while (1) {
        printf("You: ");
        fgets(message, BUFFER_SIZE, stdin); // Baca input pengguna
        message[strcspn(message, "\n")] = '\0'; // Hapus newline dari input

        // Kirim pesan ke server
        send(sock, message, strlen(message), 0);

        // Terima respon dari server
        int bytes_read = read(sock, buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null-terminate pesan
            printf("Server: %s\n", buffer);
        } else {
            printf("Server disconnected.\n"); // Jika tidak ada data, server terputus
            break;
        }
    }

    close(sock); // Tutup socket
    return 0;
}
