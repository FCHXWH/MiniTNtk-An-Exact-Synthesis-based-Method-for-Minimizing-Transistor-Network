#pragma once
#ifndef __TRANSISTORNTK_CNF_H__
#define __TRANSISTORNTK_CNF_H__
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include "TransistorNtk.h"
#include "PathLimitation.h"
#include <UGraphviz/UGraphviz.hpp>
#include <map>
#include <set>
#include "GenerateSpice.h"
using namespace Ubpa::UGraphviz;
using namespace std;

class TransistorCNF
{
private:
	TransistorNtk TNtk;
	int nCnfVars;
	vector<int> p; // 0: source 1: Drain
	vector<int> s, c, f;
	vector<vector<int>> clauses;
	// new added
	int nInputs; // number of vars in boolean function 
	vector<vector<int>> Func;
	vector<int> TruthTable;

	// version: 2023/2/10
	vector<int> VarIndPos, VarIndNeg; // indicate whether literals exist?

	// version: 2023/2/14
	vector<vector<int>> Onsets, Offsets;
	vector<vector<int>> PosRepPattern, NegRepPattern;
	string svars;

	//version: 2023/03/06
	int SatResult;

	// version: 2023/04/26
	vector<vector<vector<string>>> ResultPaths;
	vector<vector<int>> ResultTruthTables;

	// version: 2023/5/9
	int AccTech1Flag, AccTech2Flag; // flag1: representative patterns; flag2: pre-define transistors for existed literals of the given factored form

public:
	TransistorCNF();
	TransistorCNF(TransistorNtk ntk);
	~TransistorCNF();
	

	void InitVars();
	void InitVecVars();
	void CreatePosClauses();
	void CreateNegClauses();
	void CreateANDClause(vector<int> cnfvars);
	void CreateXNORClause(vector<int> cnfvars1, vector<int> cnfvars2);
	void CreateORClause(vector<int> cnfvars);
	void CreateAtMostOneClause(vector<int> cnfvars);
	void WriteCnf(string cnfpath);
	vector<string> ParseCnf(int mos, vector<transistor>& transistors, int OutINV, string cnfpath, string path = "");
	void ParsePosCnf(string cnfpath);
	void ParseNegCnf(string cnfpath);

	void InitGlobalVars();
	void CreateGlobalClauses();
	void CreatePosClauses(int iP);
	void CreateNegClauses(int iP);
	void CreateClauses();
	vector<int> DeriveTruthTable();
	void CreateFinalClauses();
	
	// version: 2023/1/14
	void InputParser(string boolfunc);
	void InputParser(string onFunc, string offFunc);
	void CreateAccClauses();
	vector<vector<int>> DeriveFunc(vector<vector<string>> vProducts, map<char, int> VarId);
	void DeriveRepPattern(int flag);
	void showPatterns(vector<vector<int>> patterns);
	void showRepPatterns(vector<vector<int>> patterns);
	void Debug();
	void InitAccVecVars();
	void CreateAccFinalClauses();
	void CreateAccPosClauses(int iP);
	void CreateAccNegClauses(int iP);
	vector<int> CreatePriorClauses();
	void CreateAdvancedPosClauses(vector<int> Tran2InputVars);  // set flows of those edges connected with opened transistors as 0
	void CreateAdvancedNegClauses(vector<int> Tran2InputVars);  

	// version:2023/3/17
	int GetSatResult() { return SatResult; }
	int GetnCnfVars() { return nCnfVars; }
	TransistorNtk GetTransistorNtk() { return TNtk; }
	void SetnCnfVars(int CnfVarsNum) { nCnfVars = CnfVarsNum; }

	// version:2023/3/18
	void CreateAccClausesWithoutGlobal();

	// version:2023/3/19
	void updateSvars(char var) { svars += var; }
	void SetnInputs(int VarsNum) { nInputs = VarsNum; }
	void SetVarIndPosNeg(map<char, int>& VarId);
	void SetOnsets(vector<vector<int>> sets) { Onsets = sets; }
	void SetOffsets(vector<vector<int>> sets) { Offsets = sets; }

	vector<vector<int>> GetClauses() { return clauses; }
	void SetClauses(vector<vector<int>> NewClauses) { clauses = NewClauses; }

	// version:2023/3/20
	void CreateTransistorInputClauses(int iTransistor, int iLiteral);

	// version: 2023/3/21
	void SetTNtk(TransistorNtk ntk) { TNtk = ntk; }

	// version: 2023/3/22
	vector<vector<int>> GetRepPattern(int Flag) { return (Flag) ? PosRepPattern : NegRepPattern; }

	// version: 2023/3/26
	void updateNtkTmpIDs() { TNtk.updateTmpID(); }

	// version:2023/3/29
	int GetnVars() { return svars.size(); }

	// version: 2023/4/14
	void CreateNoMoreNClauses(vector<int> cnfvars, int n);

	// version: 2023/4/22
	vector<string> ParseCnf(string cnfpath, map<pair<int, int>, int>& AllEdges, map<pair<int, int>, string>& transistors, map<pair<int, int>, int>& transistors_cnfvar, string path = "");
	void AddClause(vector<int> clause) { clauses.push_back(clause); }
	int getK() {
		int max = 0;
		for (auto Onset : Onsets) {
			if (Onset.size() > max)
				max = Onset.size();
		}
		return max;
	}

	// version: 2023/4/23
	void CreateAtMostKClause(vector<int> cnfvars, int K);
	void CreateKLimitedPathConstraints(int K);

	// version: 2023/4/26
	string GetSvars() { return svars; }
	vector<vector<string>> GetResultPaths(int i) { return ResultPaths[i]; }
	vector<vector<int>> DeriveResultFunc();
	void showResultPaths() {
		cout << "Resulting paths are: " << endl;
		for (auto SingleOutPaths : ResultPaths) {
			for (auto path : SingleOutPaths) {
				for (auto t : path)
					cout << t << " ";
				cout << endl;
			}
			cout << endl;
		}
	}

	vector<int> GetDepths() {
		vector<int> MaxLength(ResultPaths.size(), 0);
		for (int i = 0; i < ResultPaths.size(); i++) {
			for (auto path : ResultPaths[i])
				if (path.size() > MaxLength[i])
					MaxLength[i] = path.size();
		}
		return MaxLength;
	}
	// version: 2023/4/30
	void CreateBlockClause(vector<int> val);

	int GetLiterals() {
		int num = 0;
		for (auto onset : Onsets)
			num += onset.size();
		return num;
	}

	/*vector<string> GetVecLiterals() {

	}*/
	// version: 2023/5/9
	void SetAccFlags(int flag1, int flag2) {
		AccTech1Flag = flag1;
		AccTech2Flag = flag2;
	}
	int GetAccFlag1() { return AccTech1Flag; }
	int GetAccFlag2() { return AccTech2Flag; }
};

class MultiOutTransistorCNF :public TransistorCNF {
private:
	vector<TransistorCNF> MultiOutsCnfs;
	int nOutputs;
	/*int nCnfVars;
	TransistorNtk TNtk;
	vector<vector<int>> clauses;*/
	// version: 2023/4/22
	// int K; // limited path length
	vector<int> Ks; // each corresponding to an output

	// version: 2023/4/29
	int Exact; // whether the derived transistor network after SAT solving is exactly with the user-needed function
	int DepthLimited; // =1: limit the number of transistors in series

	// version: 2023/5/4
	vector<string> AllLiterals;

public:
	MultiOutTransistorCNF();
	~MultiOutTransistorCNF();
	MultiOutTransistorCNF(TransistorNtk NTK, int nOuts);

	void InitVecMultiOutsCnfs(vector<string> BoolFuncs, string dir = "");
	void MultiOutInputParser(vector<string> BoolFuncs, string dir = "");
	void MultiOutInputParser(vector<string> onFuncs, vector<string> offFuncs);

	void GetAllClauses();

	void MultiOutCreateClauses(vector<int> Tran2InputVars);

	// version: 2023/4/22
	vector<int> GetMaxPathLength() { return Ks; }

	// version: 2023/4/23
	TransistorCNF GetTCnf(int i) { return MultiOutsCnfs[i]; }

	// version: 2023/4/26
	vector<vector<int>> DeriveEachCnfTruthTable() {
		vector<vector<int>> vTruthtables;
		for (int i = 0; i < nOutputs; i++)
			vTruthtables.push_back(MultiOutsCnfs[i].DeriveTruthTable());
		return vTruthtables;
	}

	// version: 2023/4/29
	void SetExact(int b) { Exact = b; }
	int GetExact() { return Exact; }
	void SetDepthLimitedFlag(int flag) { DepthLimited = flag; }
	int GetIsDepthLimited() {
		vector<int> Depths = GetDepths();
		for (int i = 0; i < Depths.size(); i++)
			if (Depths[i] > Ks[i])
				return 0;
		return 1;
	}

	// version: 2023/5/4
	int GetTotalLiteralNums() {
		int num = 0;
		for (int i = 0; i < MultiOutsCnfs.size(); i++)
			num += MultiOutsCnfs[i].GetLiterals();
		return num;
	}

	vector<string> GetAlLiterals() {
		return AllLiterals;
	}
};
void removeSpace(string& s);
vector<string> split(string s, char sign);
string ParseEqn(string EqnPath);
int NPTransistorsWithoutINV(int nInternalTransistors, vector<int> OutINVFlags);
int NPTransistorsWithINV(vector<int> Tran2InputVars, int nVars, int nInternalTransistors, int OutINVFlag);
// version: 2023/3/30
int NPTransistorsWithINV(vector<int> Tran2InputVars, int nVars, int nInternalTransistors, vector<int> OutINVFlags);
// version: 2023/4/5
int NPTransistorsWithINV(set<string> InputLiterals, int nInternalTransistors, vector<int> OutINVFlags);
void ParseBoolFuncTxtFile(string TxtPath, map<string, pair<string, int>>& BoolFuncs);
#endif // !__TRANSISTORNTK_CNF_H__
