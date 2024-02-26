# MiniTNtk-An-Exact-Synthesis-based-Method-for-Minimizing-Transistor-Network

Transistor network minimization is an important step in designing new standard cells. Existing methods for minimizing transistor networks all rely on some heuristic techniques. Hence, there is still room for further improvement. In this work, we propose MiniTNtk, an exact synthesis-based method for minimizing transistor networks. It models the generation of the transistor network for a Boolean function as a Boolean satisfiability (SAT) problem and can return a transistor network with the fewest transistors. Furthermore, sometimes, it is necessary to limit the number of transistors in series.

## Reference paper:
[1] **Weihua Xiao**, Shanshan Han, Yue Yang, Shaoze Yang, Cheng Zheng, Jingsong Chen, Tingyuan Liang, Lei Li, and Weikang Qian, "[MiniTNtk: An Exact Synthesis-based Method for Minimizing
Transistor Network](https://ieeexplore.ieee.org/document/10323691)," in *Proceedings of the 2023 International Conference on Computer Aided Design (ICCAD)*, San Francisco, CA, USA, 2023, pp. 01-09.

## Requirements
- OS: Linux
- Language: C++ (version >= 2017)
- visual studio (version >= 2017)
- gcc
- g++ (version >= 9.4.0)

## Getting Started
### Download WSL (if your PC is Windows OS)
- https://learn.microsoft.com/en-us/windows/wsl/install (link and tutorial)

### Download gcc & g++:
- sudo apt install gcc;
- sudo apt install g++;

### Download MiniSAT:
- sudo apt-get install build-essential; 
- git clone https://github.com/niklasso/minisat.git;
- cd minisat; 
- make config prefix=$PREFIX;
- make install;
- sudo cp ./build/dynamic/bin/minisat /usr/bin/minisat

### Download Graphviz:
- sudo apt-get install graphviz;

### Download sis:
- https://github.com/FCHXWH/sis

### Compile with Visual Studio 2019
- Set up a new Linux project in Visual Studio 2019;

![image](https://github.com/FCHXWH/MiniTNtk-An-Exact-Synthesis-based-Method-for-Minimizing-Transistor-Network/assets/37615445/410287f3-0678-4659-8342-0afcc9d7148c)

- Add all source files into the project;
  
![image](https://github.com/FCHXWH/MiniTNtk-An-Exact-Synthesis-based-Method-for-Minimizing-Transistor-Network/assets/37615445/de3b0c7f-bc4a-4e60-80a0-bc38bda17abf)

- Configure Include path;
  
![image](https://github.com/FCHXWH/MiniTNtk-An-Exact-Synthesis-based-Method-for-Minimizing-Transistor-Network/assets/37615445/71820ecb-8f11-432d-a503-ec99c3129900)

- Compile & generate executable file (MiniTNtk.out).

## How to use
- Use `MiniTNtk.out --help`

  ![image](https://github.com/FCHXWH/MiniTNtk-An-Exact-Synthesis-based-Method-for-Minimizing-Transistor-Network/assets/37615445/ba90609c-151f-4887-8ea8-2ca4378f4f92)
