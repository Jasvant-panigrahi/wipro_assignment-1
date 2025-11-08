# wipro_assignment-1
# 🧭 Linux File Explorer (C++17)

A **console-based File Explorer Application** built in **C++17** using the **`<filesystem>`** library.  
This project replicates key Linux terminal commands such as `ls`, `cd`, `cp`, `mv`, `rm`, `mkdir`, `touch`, `chmod`, and `search` —  
providing a practical way to explore, manage, and manipulate files and directories through a simple command-line interface.

---

## 📌 Project Overview

This project was developed as part of **Assignment 1 (Linux OS)**.  
It demonstrates how C++ can interface directly with the **Linux file system** to perform real-world file operations while following the principles of modular programming and error handling.

---

## 🎯 Objective

To develop a **console-based file explorer** that performs all essential Linux file operations such as:
- Listing files and directories
- Navigating directories
- Copying, moving, and deleting files
- Searching files recursively
- Managing file permissions

---

## ⚙️ Features

✅ **List Files and Directories**
- Command: `ls` and `ls -l`
- Displays all files in the current or specified directory, with an optional detailed view.

✅ **Navigate Directories**
- Command: `cd <path>` and `pwd`
- Allows movement through directories and displays the current working directory.

✅ **File and Directory Operations**
- `cp <src> <dst>` — Copy files/directories  
- `mv <src> <dst>` — Move or rename  
- `rm <path>` — Delete files/directories  
- `mkdir <dirname>` — Create directories  
- `touch <filename>` — Create empty files  

✅ **Search Files**
- Command: `search <name>`  
- Recursively searches for files matching a given pattern within directories.

✅ **Change File Permissions**
- Command: `chmod <octal> <path>`  
- Modify file access permissions using octal notation (e.g., `755`).

---

## 🧩 Technologies Used

| Tool / Library | Purpose |
|-----------------|----------|
| **C++17** | Core programming language |
| **<filesystem>** | File and directory management |
| **<iostream> / <fstream>** | Input-output operations |
| **Linux Environment** | OS interface and testing |
| **g++** | Compiler used for building the project |

---

## 🧠 Implementation Details

- The main logic is encapsulated in the **`FileExplorer`** class.
- Uses a **REPL (Read-Eval-Print Loop)** structure to continuously take user input.
- Implements robust **error handling** for invalid paths and operations.
- Designed with **modularity** to allow easy extension of commands.

---

## 📸 Output

![Bash Output 0](output/bash-output_0.png)
![Bash Output 1](output/bash-output_1.png)
![Program Output 1.0](output/output-1.0.png)
![Program Output 1.1](output/output-1.1.png)




## 💻 Example Output

```bash
Simple File Explorer — type 'help' for commands
"/home/compiler" $ pwd
"/home/compiler"

"/home/compiler" $ ls
Desktop
Documents
Downloads
main.cpp
test.txt

"/home/compiler" $ mkdir Project_Files
"/home/compiler" $ cd Project_Files
"/home/compiler/Project_Files" $ touch demo.txt
"/home/compiler/Project_Files" $ ls -l
-rw-r--r--        0 2025-11-08 12:34:25 demo.txt

"/home/compiler/Project_Files" $ cp demo.txt copy_demo.txt
"/home/compiler/Project_Files" $ ls
demo.txt
copy_demo.txt

"/home/compiler/Project_Files" $ mv copy_demo.txt final_demo.txt
"/home/compiler/Project_Files" $ ls
demo.txt
final_demo.txt

"/home/compiler/Project_Files" $ chmod 755 final_demo.txt
"/home/compiler/Project_Files" $ stat final_demo.txt
-rwxr-xr-x        0 2025-11-08 12:36:10 final_demo.txt
Absolute: /home/compiler/Project_Files/final_demo.txt
Type: file

"/home/compiler/Project_Files" $ search demo
/home/compiler/Project_Files/demo.txt
/home/compiler/Project_Files/final_demo.txt

"/home/compiler/Project_Files" $ rm demo.txt
"/home/compiler/Project_Files" $ ls
final_demo.txt

"/home/compiler/Project_Files" $ cd ..
"/home/compiler" $ ls
Desktop
Documents
Downloads
Project_Files
main.cpp

"/home/compiler" $ exit
Bye!

