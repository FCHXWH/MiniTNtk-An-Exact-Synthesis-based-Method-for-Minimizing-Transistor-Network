#ifndef __GENERATE_SPICE__
#define __GENERATE_SPICE__
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "PathLimitation.h"
using namespace std;
typedef struct transistor_ {
	string Gate;
	string Source;
	string Drain;
	string Substrate;
	string Mos;
	float W = 0;
	float L = 0.05; // 45 NM
	// float L = 16; // 7 NM
	int lambda = 2;
	float basicW = 0.25; // 45 NM
	// float basicW = 23; // 7 NM
	int nFin;
}transistor;

vector<transistor> DeriveSourceDrains(vector<pair<pair<int, int>, string>> transistors_pairs);
vector<transistor> transformTransistors(int nClusters, int InputClusterID, int OutClusterID, vector<pair<pair<int, int>, string>> transistors_pairs, int mos);
void writeSpice(string file, string CircktName, set<string> AllLiterals, vector<transistor> transistorsPUN, vector<transistor> transistorsPDN);
void GenerateTransistors(set<string> AllLiterals, vector<transistor>& transistorsPUN, vector<transistor>& transistorsPDN);
void GenerateTransistors(set<string> AllLiterals, vector<transistor>& transistorsPUN, vector<transistor>& transistorsPDN, int OutINV);
vector<transistor> transformTransistors(int nClusters, int InputClusterID, int OutClusterID, vector<pair<pair<int, int>, string>> transistors_pairs, map<pair<int, int>, int> transistors_Counts, int mos, int OutINV);
#endif // !__GENERATE_SPICE__
