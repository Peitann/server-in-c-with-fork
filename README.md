# Server Multi-Client dengan Fork di C

## Pendahuluan
Server multi-client merupakan salah satu kebutuhan penting dalam pengembangan aplikasi berbasis jaringan. Program ini bertujuan untuk membangun server yang mampu menangani banyak klien secara bersamaan menggunakan teknik multiprocessing dengan fork(). Server ini menyediakan layanan echo, di mana pesan yang dikirim klien akan dikembalikan (di-echo) oleh server.

## Cara Menjalankan
1. Kompilasi kode:

```
gcc -o server server.c
gcc -o client client.c
```

2. Jalankan server:

```
./server
```

3. Jalankan client (di terminal lain):

```
./client
```


## Fitur
- Multi-client support dengan fork().
- Echo service untuk setiap klien.
- Penanganan sinyal untuk proses zombie.
