#pragma once
#ifndef __TRANSISTOR_NTK__
#define __TRANSISTOR_NTK__
#include <iostream>
#include <vector>
#include <UGraphviz/UGraphviz.hpp>
#include <sstream>
#include <fstream>
using namespace std;
using namespace Ubpa::UGraphviz;
class Switch
{
public:
	Switch();
	Switch(string s);
	~Switch();
	string name;
	int val; // 0: no switch 1: has a switch
private:

};

class TransistorNode
{
public:
	TransistorNode();
	TransistorNode(string s, int Id);
	~TransistorNode();
	vector<int> neighborEdges;
	int CnfVar;
	vector<int> CnfVecVars;
	string name;
	int type; // 0: ports of transistors 1: VDD 2:Output
	int id;
	int TmpId; // for multi-out Ntk case
private:

};



class TransistorEdge
{
public:
	TransistorEdge();
	TransistorEdge(int n1, int n2, string sw, int Id);
	~TransistorEdge();
	int id; // edge id
	int id1, id2;
	int CnfVarC1, CnfVarC2; // id1->id2, id2->id1
	int CnfVarF1, CnfVarF2; // id1->id2, id2->id1
	int CnfVarSw;
	vector<int> CnfVecVarsC1, CnfVecVarsC2;
	vector<int> CnfVecVarsF1, CnfVecVarsF2;
	vector<int> CnfVecVarsSw; // transistor variable

	Switch s;

	// new added
	int CnfVarCnt; // Internal connection variable
	vector<int> CnfVarInputs; // input selection variables
	// version: 2023/3/22
	int TmpId; // for multi-out Ntk case

	//// version: 2023/4/15
	//int CnfBoundVarC1, CnfBoundVarC2;
	//int CnfBoundVarF1, CnfBoundVarF2;
	//int CnfBoundVarSw;

private:

};

class TransistorNtk
{
public:
	int nTransistors; // number of transistors in Ntk
	vector<int> iTransistors; // ids of all transistor edges
	vector<int> Tt;
	vector<TransistorEdge> Edges;
	vector<TransistorNode> Nodes;
	TransistorNtk();
	~TransistorNtk();

	// version: 2023/03/06
	string NtkName;

	// version: 2023/03/15
	void RemoveNode(int iNode, string newNtkName);
	void DrawTransistorNtk();

	// version: 2023/3/22
	int nOutputs;

	// version: 2023/3/26
	void updateTmpID() {
		for (int i = 0; i < Nodes.size(); i++)
			Nodes[i].TmpId = Nodes[i].id;
		for (int i = 0; i < Edges.size(); i++)
			Edges[i].TmpId = Edges[i].id;
	}

	// version: 2023/4/22 0 1 2 3
	vector<int> GetOutputNodeIDs() {
		vector<int> outputIDs;
		for (int i = 0; i < nOutputs; i++)
			outputIDs.push_back(Nodes[Nodes.size() - nOutputs + i].id);
		return outputIDs;
	}
private:

};



TransistorNtk ReadTransistorNtk(string path);
TransistorNtk ConstructTransistorNtk(int nTransistors, string Name);

// version:2023/03/15
TransistorNtk ConstructMultiOutTransistorNtk(int nTransistors, int nOutputs, string Name);
#endif // !__TRANSISTOR_NTK__

