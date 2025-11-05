# 📁 File Transfer Protocol (FTP) – C++ Project

This project implements a simple **File Transfer Protocol (FTP)** using **socket programming in C++**.  
It demonstrates file transfer between a **server** and **client** on a local network.

---

## ⚙️ Features
- File transfer between client and server using TCP  
- Reliable connection and data transmission  
- Automatic creation of a new file (`downloaded.txt`) after transfer  
- Error handling for missing or corrupted files  

---

## 🧠 How It Works
1. The **server** hosts a sample file named `test.txt`.
2. The **client** connects to the server and requests the file.
3. The file data is sent over a TCP socket connection.
4. The client saves the received file as `downloaded.txt`.

---

## 🧩 Technologies Used
- C++  
- Socket Programming  
- TCP/IP Networking  

---

## 🖥️ How to Run
1. Compile both files:
   ```bash
   g++ server.cpp -o server
   g++ client.cpp -o client
2.first run the server
./server
3.run the client in another terminal
./client
The file test.txt from the server will be received and saved as downloaded.txt in the client’s folder.
