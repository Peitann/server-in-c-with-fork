#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080           // Port yang digunakan oleh server
#define BUFFER_SIZE 1024    // Ukuran buffer untuk data yang dikirim/diterima

// Fungsi untuk menangani setiap klien secara independen
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(client_socket, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytes_read] = '\0'; // Tambahkan null terminator
        printf("Client: %s\n", buffer); // Tampilkan pesan dari klien

        // Kirim kembali pesan ke klien (echo)
        send(client_socket, buffer, bytes_read, 0);
    }

    printf("Client disconnected.\n"); // Klien terputus
    close(client_socket); // Tutup socket klien
}

// Fungsi untuk menangani sinyal SIGCHLD dan menghindari zombie process
void sigchld_handler(int sig) {
    (void)sig; // Menghindari peringatan unused parameter
    while (waitpid(-1, NULL, WNOHANG) > 0); // Tunggu semua proses anak yang selesai
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Pasang handler untuk sinyal SIGCHLD
    signal(SIGCHLD, sigchld_handler);

    // Buat socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Konfigurasi alamat server
    server_addr.sin_family = AF_INET;            // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;    // Terima koneksi dari semua IP
    server_addr.sin_port = htons(PORT);          // Konversi port ke network byte order

    // Ikat socket ke alamat IP dan port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Mulai mendengarkan koneksi masuk
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", PORT);

    // Loop utama untuk menerima klien
    while (1) {
        // Terima koneksi klien
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected.\n");

        // Buat proses anak untuk menangani klien
        pid_t pid = fork();
        if (pid == 0) {
            // Proses anak
            close(server_socket); // Tutup socket server di proses anak
            handle_client(client_socket); // Panggil fungsi untuk menangani klien
            exit(0);
        } else if (pid > 0) {
            // Proses induk
            close(client_socket); // Tutup socket klien di proses induk
        } else {
            perror("Fork failed");
            close(client_socket);
        }
    }

    close(server_socket); // Tutup socket server
    return 0;
}
