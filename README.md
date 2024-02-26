# MiniTNtk-An-Exact-Synthesis-based-Method-for-Minimizing-Transistor-Network

Transistor network minimization is an important step in designing new standard cells. Existing methods for minimizing transistor networks all rely on some heuristic techniques. Hence, there is still room for further improvement. In this work, we propose MiniTNtk, an exact synthesis-based method for minimizing transistor networks. It models the generation of the transistor network for a Boolean function as a Boolean satisfiability (SAT) problem and can return a transistor network with the fewest transistors. Furthermore, sometimes, it is necessary to limit the number of transistors in series.

## Reference paper:
[1] **Weihua Xiao**, Shanshan Han, Yue Yang, Shaoze Yang, Cheng Zheng, Jingsong Chen, Tingyuan Liang, Lei Li, and Weikang Qian, "[MiniTNtk: An Exact Synthesis-based Method for Minimizing
Transistor Network](https://ieeexplore.ieee.org/document/10323691)," in *Proceedings of the 2023 International Conference on Computer Aided Design (ICCAD)*, San Francisco, CA, USA, 2023, pp. 01-09.

## Requirements
- OS: 64-bit Windows/Linux
- Gurobi (version >= 9.0)
- visual studio (version >= 2017)
- gcc
- g++

Note that this project uses C++ interface of Gurobi optimizer 9.0, a mixed ILP solver, to solve the ILP problems (link: https://www.gurobi.com).

## Getting Started
### Configuration in VS Studio
- Install Gurobi at a self-defined path `Gurobi_Path`
- Set up a C++ project in Visio Studio;
- Add the source files and header files in the folder `src/`;
- Configure the library path of Gurobi for this project:

  Project->Property->
  - VC++ directory->Include directory->Edit->New line: `Gurobi_Path\win64\include`;
  - VC++ directory->Library directory->Edit->New line: `Gurobi_Path\win64\lib`;
  - Linker->Input->Additional dependencies->Edit->New line： `gurobi_c++mdd2017.lib`;
  - Linker->Input->Additional dependencies->Edit->New line： `gurobi90.lib`.
  
### Configuration in Code
- `PARAMETER_W` : parameter to control the weight between adder tree's delay and area, e.g., area+ `PARAMETER_W` * delay;
- `PARAMETER_L` : speedup technique that control the optimized bits of adder tree;
- `MULT_SIZE` : size of optimized multiplier;
- `stages_num` : number of compressor tree's stages, which us consistent with wallace tree's.
- `Time_Bound_s` : runtime upper bound of optimization;

### Example
Optimize a 10 * 10  multiplier, set: 
- MULT_SIZE = 10;
- stages_num = 5;
- PARAMETER_W = 10;
- PARAMETER_L = 8;

In this case, the results of this GOMIL multiplier are saved into `Tune_WL/gurobi_10_10.sol` (compression tree) and `Tune_WL/gurobi_glo_adder_10.sol` (adder tree part).

## Post-Processing
### Generate the result file of optimized Compressor tree
- Set up a C++ project in Visio Studio;
- Add the source file `script_for_Compressor_tree/main.cpp`;
- Configuration in code:
  - `filename0` : path of optimization solution file, e.g., `Tune_WL/gurobi_10_10.sol`;
  - `filename1` : path of generated compressor result file, e.g., `Tune_WL/10_10.txt`;
  - `stage_num` : number of compressor tree's stages;
  - `num_of_elements` : 2 * MULT_SIZE -1;
- Run this code and the result file .txt is saved in `Tune_WL/`.

### Generate verilog files for GOMIL multipliers
- Set up a C++ project in Visio Studio;
- Add the source files and header files in the folder `script_Verilog_Generator/`;
- Configuration in code:
  
  `Compressor_Tree.cpp`:
  - `wordlength` : MULT_SIZE;
  - `stage_number` : number of compressor tree's stages;
  - `filename1` : path of generated compressor result file, e.g., `Tune_WL/10_10.txt`;
  
  `fast_mult.cpp`:
  - `splits_num` : number of segments in the optimized prefix tree;
  
  `CSL.cpp`:
  - `splits` : the segments of the optimized prefix tree;
  - `cssa_skip_points` : indicate where to use a carry select skip adder (cssa) in one segment;
  
  `Prefix_Tree.cpp`
  - `splits`& `carry_bit_index` : the connection order of internal nodes in the optimized prefix tree, which can be achieved from `Tune_WL/gurobi_glo_adder_10.sol`;
  - `cssa_skip_point` & `compensated_nodes` : the inserted cssa in one segment.
  
  Note that all the above configurations about optimized prefix tree can be achieved from adder tree's optimization solution file, e.g., `Tune_WL/gurobi_glo_adder_10.sol`, and we will modify the code to generate these configurations automatically!
  
- Run this code and all related verilog files .v are generated in `script_Verilog_Generator/`.
