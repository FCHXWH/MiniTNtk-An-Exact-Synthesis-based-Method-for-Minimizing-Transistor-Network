#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "TransistorNtk.h"
#include "TransistorNtkCNF.h"
#include "PathLimitation.h"
#include "GenerateSpice.h"
#include <set>
using namespace std;
// nTransistors: the number of transistors in the initial exact synthesis round
// nPreSetTransistors: the number of pre-determined transistors in each exact synthesis round
// vPreSetTransistors: the patterns of how to set the remaining transistors' corresponding literals
//pair<vector<int>, pair<int,int>> TransistorExactSynthesis(string dir, string FuncName, vector<string> Funcs, int nTransistors, int timeBound = 3600, int nPreSetTransistors = 0,
//	vector<vector<int>> vPreSetTransistors = {}) {
//	int SatFlag = 0;
//	vector<vector<int>> vPreSetTransistorsTmp = vPreSetTransistors;
//	for (int i = 0; i < Funcs.size(); i++)
//		FuncName.erase(FuncName.begin());
//
//	while (!SatFlag) {
//		// nTransistors = 7;
//		cout << endl << FuncName << " with " << nTransistors << " transistors: " << endl;
//		TransistorNtk TMultiOutNtk = ConstructMultiOutTransistorNtk(nTransistors, Funcs.size(), FuncName);
//		MultiOutTransistorCNF TMultiCnf(TMultiOutNtk, Funcs.size());
//		TMultiCnf.InitVecMultiOutsCnfs(Funcs, dir);
//		/*TMultiCnf.InitGlobalVars();
//		TMultiCnf.CreateGlobalClauses();*/
//		vector<int> Tran2InputVars = TMultiCnf.CreatePriorClauses();
//		int nInitTransistors = Tran2InputVars.size();
//		int nRemainTransistors = TMultiOutNtk.iTransistors.size() - Tran2InputVars.size();
//		nRemainTransistors = (nRemainTransistors < nPreSetTransistors) ? nRemainTransistors : nPreSetTransistors;
//		if (nRemainTransistors) {
//			int nCases = (!vPreSetTransistors.size()) ? int(pow(Tran2InputVars.size(), nRemainTransistors)) : vPreSetTransistors.size();
//			if (!vPreSetTransistors.size()) {
//				for (int i = 0; i < nCases; i++) {
//					int iCase = i;
//					vector<int> vIds(nRemainTransistors);
//					for (int j = 0; j < nRemainTransistors; j++) {
//						int iLiteral = iCase % Tran2InputVars.size();
//						vIds[j] = Tran2InputVars[iLiteral];
//						iCase /= Tran2InputVars.size();
//					}
//					vPreSetTransistors.push_back(vIds);
//				}
//			}
//			else {
//				vPreSetTransistors = vPreSetTransistorsTmp;
//				for (int i = 0; i < vPreSetTransistors.size(); i++)
//					for (int j = 0; j < Tran2InputVars.size(); j++)
//						vPreSetTransistors[i].erase(vPreSetTransistors[i].begin());
//			}
//			for (int i = 0; i < nCases; i++) {
//				MultiOutTransistorCNF TMultiCnfTmp = TMultiCnf;
//				vector<int> Tran2InputVarsTmp = Tran2InputVars;
//				// create priorclauses for remaining transistors
//				for (int j = 0; j < nRemainTransistors; j++) {
//					TMultiCnfTmp.CreateTransistorInputClauses(j + Tran2InputVars.size(), Tran2InputVars[vPreSetTransistors[i][j]]);
//					Tran2InputVarsTmp.push_back(Tran2InputVars[vPreSetTransistors[i][j]]);
//				}
//
//				TMultiCnfTmp.MultiOutCreateClauses(Tran2InputVarsTmp);
//				TMultiCnfTmp.GetAllClauses();
//				TMultiCnfTmp.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
//				system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
//				// system(("minisat ./" + TMultiOutNtk.NtkName + ".cnf ./" + TMultiOutNtk.NtkName + "_out.cnf").c_str());
//				Tran2InputVarsTmp = TMultiCnfTmp.ParseCnf(dir + TMultiOutNtk.NtkName + "_out.cnf", dir);
//				// TMultiCnfTmp.ParseCnf("./" + TMultiOutNtk.NtkName + "_out.cnf");
//				if (TMultiCnfTmp.GetSatResult()) {
//					SatFlag = 1;
//					return make_pair(Tran2InputVarsTmp, make_pair(TMultiCnfTmp.GetnVars(), nInitTransistors));
//					// fExcel << TMultiOutNtk.NtkName << "," << NPTransistorsWithINV(Tran2InputVarsTmp, TMultiCnfTmp.GetnVars(), nTransistors, OutInvFlags) << "," << elapsed_time.count() << "," << "SAT" << endl;
//					
//				}
//			}
//		}
//		else {
//			TMultiCnf.MultiOutCreateClauses(Tran2InputVars);
//			TMultiCnf.GetAllClauses();
//			TMultiCnf.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
//			system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
//			Tran2InputVars = TMultiCnf.ParseCnf(dir + TMultiOutNtk.NtkName + "_out.cnf", dir);
//			if (TMultiCnf.GetSatResult()) {
//				SatFlag = 1;
//				return make_pair(Tran2InputVars, make_pair(TMultiCnf.GetnVars(), nInitTransistors));
//				// fExcel << TMultiOutNtk.NtkName << "," << NPTransistorsWithINV(Tran2InputVars, TMultiCnf.GetnVars(), nTransistors, OutInvFlags) << "," << elapsed_time.count() << "," << "SAT" << endl;
//			}
//		}
//		nTransistors++;
//	}
//}

// nTransistors: the number of transistors in the initial exact synthesis round
// nPreSetTransistors: the number of pre-determined transistors in each exact synthesis round
// vPreSetTransistors: the patterns of how to set the remaining transistors' corresponding literals

pair<vector<string>, pair<int, int>> TransistorExactSynthesis(string dir, string FuncName, vector<string> Funcs, int nTransistors,
	vector<transistor>& Transistors, int mos, int INVOUT, 
	int timeBound = 3600, int nPreSetTransistors = 0,
	int DepthLimited = 1, int AccFlag1 = 1, int AccFlag2 = 1, vector<vector<int>> vPreSetTransistors = {}) {
	int SatFlag = 0;
	vector<vector<int>> vPreSetTransistorsTmp = vPreSetTransistors;
	for (int i = 0; i < Funcs.size(); i++)
		FuncName.erase(FuncName.begin());

	while (!SatFlag) {
		// nTransistors = 7;
		cout << endl << FuncName << " with " << nTransistors << " transistors: " << endl;
		TransistorNtk TMultiOutNtk = ConstructMultiOutTransistorNtk(nTransistors, Funcs.size(), FuncName);
		MultiOutTransistorCNF TMultiCnf(TMultiOutNtk, Funcs.size());
		// version: 2023/5/9
		TMultiCnf.SetAccFlags(AccFlag1, AccFlag2);
		// Acceleration 1: compute representative patterns
		TMultiCnf.InitVecMultiOutsCnfs(Funcs, dir);
		TMultiCnf.SetDepthLimitedFlag(DepthLimited);
		/*TMultiCnf.InitGlobalVars();
		TMultiCnf.CreateGlobalClauses();*/
		// derive the needed literals
		// Acceleration 2: pre-define some existed literals
		vector<int> Tran2InputVars = (TMultiCnf.GetAccFlag2()) ? TMultiCnf.CreatePriorClauses() : vector<int>{};
		int nRemainTransistors = TMultiOutNtk.iTransistors.size() - Tran2InputVars.size();
		nRemainTransistors = (nRemainTransistors < nPreSetTransistors) ? nRemainTransistors : nPreSetTransistors;
		if (nRemainTransistors) {
			int nCases = (!vPreSetTransistors.size()) ? int(pow(Tran2InputVars.size(), nRemainTransistors)) : vPreSetTransistors.size();
			if (!vPreSetTransistors.size()) {
				for (int i = 0; i < nCases; i++) {
					int iCase = i;
					vector<int> vIds(nRemainTransistors);
					for (int j = 0; j < nRemainTransistors; j++) {
						int iLiteral = iCase % Tran2InputVars.size();
						vIds[j] = Tran2InputVars[iLiteral];
						iCase /= Tran2InputVars.size();
					}
					vPreSetTransistors.push_back(vIds);
				}
			}
			else {
				vPreSetTransistors = vPreSetTransistorsTmp;
				for (int i = 0; i < vPreSetTransistors.size(); i++)
					for (int j = 0; j < Tran2InputVars.size(); j++)
						vPreSetTransistors[i].erase(vPreSetTransistors[i].begin());
			}
			for (int i = 0; i < nCases; i++) {
				MultiOutTransistorCNF TMultiCnfTmp = TMultiCnf;
				vector<int> Tran2InputVarsTmp = Tran2InputVars;
				vector<string> Literals;
				// create priorclauses for remaining transistors
				for (int j = 0; j < nRemainTransistors; j++) {
					TMultiCnfTmp.CreateTransistorInputClauses(j + Tran2InputVars.size(), Tran2InputVars[vPreSetTransistors[i][j]]);
					Tran2InputVarsTmp.push_back(Tran2InputVars[vPreSetTransistors[i][j]]);
				}

				TMultiCnfTmp.MultiOutCreateClauses(Tran2InputVarsTmp);
				TMultiCnfTmp.GetAllClauses();
				TMultiCnfTmp.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
				system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
				// system(("minisat ./" + TMultiOutNtk.NtkName + ".cnf ./" + TMultiOutNtk.NtkName + "_out.cnf").c_str());
				Literals = TMultiCnfTmp.ParseCnf(mos, Transistors, INVOUT, dir + TMultiOutNtk.NtkName + "_out.cnf", dir);
				// TMultiCnfTmp.ParseCnf("./" + TMultiOutNtk.NtkName + "_out.cnf");
				if (TMultiCnfTmp.GetSatResult()) {
					SatFlag = 1;
					return make_pair(Literals, make_pair(TMultiCnfTmp.GetExact(), (DepthLimited) ? TMultiCnfTmp.GetIsDepthLimited() : 1));
					// fExcel << TMultiOutNtk.NtkName << "," << NPTransistorsWithINV(Tran2InputVarsTmp, TMultiCnfTmp.GetnVars(), nTransistors, OutInvFlags) << "," << elapsed_time.count() << "," << "SAT" << endl;

				}
			}
		}
		else {
			vector<string> Literals;
			// version: 2023/5/4
			/*if (nTransistors >= TMultiCnf.GetAlLiterals().size() || nTransistors >= 18) {
				return make_pair(TMultiCnf.GetAlLiterals(), make_pair(1, 1));
			}*/
			if (nTransistors >= 18) {
				return make_pair(TMultiCnf.GetAlLiterals(), make_pair(1, 1));
			}
			// set the neighbour edges' flows as 0 for positive input patterns
			TMultiCnf.MultiOutCreateClauses(Tran2InputVars);
			TMultiCnf.GetAllClauses();
			TMultiCnf.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
			system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
			Literals = TMultiCnf.ParseCnf(mos, Transistors, INVOUT, dir + TMultiOutNtk.NtkName + "_out.cnf", dir);
			if (TMultiCnf.GetSatResult()) {
				if (!DepthLimited) {
					SatFlag = 1;
					vector<vector<int>> ActualTt = TMultiCnf.DeriveEachCnfTruthTable();
					vector<vector<int>> ResultTt = TMultiCnf.DeriveResultFunc();
					TMultiCnf.showResultPaths();
					if (ActualTt == ResultTt)
						TMultiCnf.SetExact(1);
					else
						TMultiCnf.SetExact(0);
					return make_pair(Literals, make_pair(TMultiCnf.GetExact(), (DepthLimited) ? TMultiCnf.GetIsDepthLimited() : 1));
				}
				int AddBlockConstraintsTimes = 0;
				while (AddBlockConstraintsTimes<50) {
					vector<vector<int>> ActualTt = TMultiCnf.DeriveEachCnfTruthTable();
					vector<vector<int>> ResultTt = TMultiCnf.DeriveResultFunc();
					TMultiCnf.showResultPaths();
					if (ActualTt == ResultTt)
						TMultiCnf.SetExact(1);
					else
						TMultiCnf.SetExact(0);
					if(TMultiCnf.GetIsDepthLimited())
						return make_pair(Literals, make_pair(TMultiCnf.GetExact(), (DepthLimited) ? TMultiCnf.GetIsDepthLimited() : 1));
					TMultiCnf.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
					system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
					Literals = TMultiCnf.ParseCnf(mos, Transistors, INVOUT, dir + TMultiOutNtk.NtkName + "_out.cnf", dir);
					AddBlockConstraintsTimes++;
				}
				

				
				// fExcel << TMultiOutNtk.NtkName << "," << NPTransistorsWithINV(Tran2InputVars, TMultiCnf.GetnVars(), nTransistors, OutInvFlags) << "," << elapsed_time.count() << "," << "SAT" << endl;
			}
		}
		nTransistors++;
	}
}

//vector<string> TransistorExactSynthesis(string dir, string FuncName, vector<string> Funcs, int nTransistors, int timeBound = 3600, int nPreSetTransistors = 0,
//	vector<vector<int>> vPreSetTransistors = {}) {
//	int SatFlag = 0;
//	vector<vector<int>> vPreSetTransistorsTmp = vPreSetTransistors;
//	for (int i = 0; i < Funcs.size(); i++)
//		FuncName.erase(FuncName.begin());
//
//	while (!SatFlag) {
//		// nTransistors = 7;
//		cout << endl << FuncName << " with " << nTransistors << " transistors: " << endl;
//		TransistorNtk TMultiOutNtk = ConstructMultiOutTransistorNtk(nTransistors, Funcs.size(), FuncName);
//		MultiOutTransistorCNF TMultiCnf(TMultiOutNtk, Funcs.size());
//		TMultiCnf.InitVecMultiOutsCnfs(Funcs, dir);
//		/*TMultiCnf.InitGlobalVars();
//		TMultiCnf.CreateGlobalClauses();*/
//		vector<int> Tran2InputVars = TMultiCnf.CreatePriorClauses();
//		int nRemainTransistors = TMultiOutNtk.iTransistors.size() - Tran2InputVars.size();
//		nRemainTransistors = (nRemainTransistors < nPreSetTransistors) ? nRemainTransistors : nPreSetTransistors;
//		if (nRemainTransistors) {
//			int nCases = (!vPreSetTransistors.size()) ? int(pow(Tran2InputVars.size(), nRemainTransistors)) : vPreSetTransistors.size();
//			if (!vPreSetTransistors.size()) {
//				for (int i = 0; i < nCases; i++) {
//					int iCase = i;
//					vector<int> vIds(nRemainTransistors);
//					for (int j = 0; j < nRemainTransistors; j++) {
//						int iLiteral = iCase % Tran2InputVars.size();
//						vIds[j] = Tran2InputVars[iLiteral];
//						iCase /= Tran2InputVars.size();
//					}
//					vPreSetTransistors.push_back(vIds);
//				}
//			}
//			else {
//				vPreSetTransistors = vPreSetTransistorsTmp;
//				for (int i = 0; i < vPreSetTransistors.size(); i++)
//					for (int j = 0; j < Tran2InputVars.size(); j++)
//						vPreSetTransistors[i].erase(vPreSetTransistors[i].begin());
//			}
//			for (int i = 0; i < nCases; i++) {
//				MultiOutTransistorCNF TMultiCnfTmp = TMultiCnf;
//				vector<int> Tran2InputVarsTmp = Tran2InputVars;
//				vector<string> Literals;
//				// create priorclauses for remaining transistors
//				for (int j = 0; j < nRemainTransistors; j++) {
//					TMultiCnfTmp.CreateTransistorInputClauses(j + Tran2InputVars.size(), Tran2InputVars[vPreSetTransistors[i][j]]);
//					Tran2InputVarsTmp.push_back(Tran2InputVars[vPreSetTransistors[i][j]]);
//				}
//
//				TMultiCnfTmp.MultiOutCreateClauses(Tran2InputVarsTmp);
//				TMultiCnfTmp.GetAllClauses();
//				TMultiCnfTmp.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
//				system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
//				// system(("minisat ./" + TMultiOutNtk.NtkName + ".cnf ./" + TMultiOutNtk.NtkName + "_out.cnf").c_str());
//				Literals = TMultiCnfTmp.ParseCnf(dir + TMultiOutNtk.NtkName + "_out.cnf", dir);
//				// TMultiCnfTmp.ParseCnf("./" + TMultiOutNtk.NtkName + "_out.cnf");
//				if (TMultiCnfTmp.GetSatResult()) {
//					SatFlag = 1;
//					return Literals;
//					// fExcel << TMultiOutNtk.NtkName << "," << NPTransistorsWithINV(Tran2InputVarsTmp, TMultiCnfTmp.GetnVars(), nTransistors, OutInvFlags) << "," << elapsed_time.count() << "," << "SAT" << endl;
//
//				}
//			}
//		}
//		else {
//			vector<string> Literals;
//			TMultiCnf.MultiOutCreateClauses(Tran2InputVars);
//			TMultiCnf.GetAllClauses();
//			TMultiCnf.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
//			system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
//			map<pair<int, int>, int> AllEdges;
//			map<pair<int, int>, string> transistors;
//			map<pair<int, int>, int> transistors_cnfvars;
//			// Literals = TMultiCnf.ParseCnf(dir + TMultiOutNtk.NtkName + "_out.cnf", AllEdges, transistors, transistors_cnfvars, dir);
//			Literals = TMultiCnf.GetTCnf(0).ParseCnf(dir + TMultiOutNtk.NtkName + "_out.cnf", AllEdges, transistors, transistors_cnfvars, dir);
//			if (TMultiCnf.GetSatResult()) {
//				int KPathLimitRounds = 0;
//				while (1) {
//					vector<vector<int>> PathLimitedClauses = IncrementalAddLimitedPathConstraints(AllEdges, transistors, transistors_cnfvars, TMultiCnf.GetTransistorNtk().Nodes.size(), TMultiCnf.GetMaxPathLength(), TMultiCnf.GetTransistorNtk().GetOutputNodeIDs());
//					if (!PathLimitedClauses.size()) {
//						SatFlag = 1;
//						return Literals;
//					}
//					else {
//						for (auto clause : PathLimitedClauses)
//							TMultiCnf.AddClause(clause);
//						TMultiCnf.WriteCnf(dir + TMultiOutNtk.NtkName + ".cnf");
//						system(("timeout " + to_string(timeBound) + " minisat " + dir + TMultiOutNtk.NtkName + ".cnf " + dir + TMultiOutNtk.NtkName + "_out.cnf").c_str());
//						AllEdges.clear();
//						transistors.clear();
//						transistors_cnfvars.clear();
//						Literals = TMultiCnf.ParseCnf(dir + TMultiOutNtk.NtkName + "_out.cnf", AllEdges, transistors, transistors_cnfvars, dir);
//					}
//					KPathLimitRounds++;
//				}
//				/*return Literals;*/
//			}
//		}
//		nTransistors++;
//	}
//}


pair<vector<string>, int> InputNeg(vector<string> Funcs, string dir = "") {
	vector<string> InputNegFuncs;
	set<string> Literals;
	for (auto Func : Funcs) {
		removeSpace(Func);
		// cout << "Initial Func: " << Func << endl;
		ofstream f1;
		f1.open(dir + "NegFunc.eqn");
		f1 << "Func=!(" << Func << ");" << endl;
		f1.close();
		system((string("sis -c \"read_eqn ") + dir + string("NegFunc.eqn;simplify;write_eqn ") + dir + string("NegFunc_out.eqn;\"")).c_str());
		string NegFunc = ParseEqn(dir + "NegFunc_out.eqn");
		// cout << "Neg Func: " << NegFunc << endl;
		vector<string> Products = (NegFunc.size()) ? split(NegFunc, '+') : vector<string>{};
		vector<vector<string>> vProducts;
		// derive the sum of products for onset and offset
		for (auto s : Products)
			vProducts.push_back(split(s, '*'));
		for (int i = 0; i < vProducts.size(); i++) {
			for (int j = 0; j < vProducts[i].size(); j++) {
				if (vProducts[i][j][0] == '!')
					vProducts[i][j].erase(vProducts[i][j].begin());
				else
					vProducts[i][j] = "!" + vProducts[i][j];
			}
		}
		// get the number of literals
		for (auto product : vProducts)
			for (auto literal : product)
				Literals.insert(literal);

		// recombine products
		NegFunc = "";
		for (int i = 0; i < vProducts.size(); i++) {
			string product = vProducts[i][0];
			for (int j = 1; j < vProducts[i].size(); j++) {
				product += "*";
				product += vProducts[i][j];
			}
			if (i)
				NegFunc += "+";
			NegFunc += product;
		}

		InputNegFuncs.push_back(NegFunc);
		// cout << "Input neg: " << NegFunc << endl;
	}
	return make_pair(InputNegFuncs, Literals.size());
}

int main(int argc, char* argv[]) {
	if (string(argv[1]) == "--help") {
		cout << "argv[1]: path of Booleans.txt;" << endl;
		cout << "argv[2]: number of pre-defined transistors;" << endl;
		cout << "argv[3]: runtime bound of SAT solving;" << endl;
		cout << "argv[4]: only solving PDN (=1);" << endl;
		cout << "argv[5]: limit the number of transistors in series (=1);" << endl;
		cout << "argv[6]: count the transistors of input inverters (=1);" << endl;
		cout << "argv[7]: acceleration technique 1: representative patterns;" << endl;
		cout << "argv[8]: acceleration technique 2: pre-defined transistors for existed literals in a given Boolean function." << endl;
		return 0;
	}
	int OnlyPDNFlag = atoi(argv[4]);
	int DepthLimited = atoi(argv[5]);
	int CountInputINV = atoi(argv[6]);
	int AccFlag1 = 1;
	int AccFlag2 = 1;
	map<string, pair<string, int>> BoolFuncs;
	ParseBoolFuncTxtFile(argv[1]+string("Booleans.txt"), BoolFuncs);

	ofstream fExcel;
	fExcel.open(argv[1] + string("PClassResults.csv"));
	if (DepthLimited)
		fExcel << "Boolean Func,#Transistors,Runtime(ms),SatResult,DepthLimited" << endl;
	else
		fExcel << "Boolean Func,#Transistors,Runtime(ms),SatResult" << endl;
	for (auto it = BoolFuncs.begin(); it != BoolFuncs.end(); it++) {
		string FuncName = (*it).first;
		vector<string> Funcs = split((*it).second.first, '|');
		int nTransistors = (*it).second.second;

		// version: 2023/4/4
		int OutInvFlag = (FuncName[0] == '!') ? 1 : 0;
		vector<int> OutInvFlags;
		for (int i = 0; i < Funcs.size(); i++) {
			if (FuncName[i] == '!')
				OutInvFlags.push_back(1);
			else
				OutInvFlags.push_back(0);
		}
		int SAT = 1;
		int SATDepthLimited = 1;
		auto start_time = chrono::high_resolution_clock::now();
		pair<vector<string>,pair<int,int>> LiteralsPDN, LiteralsPUN; // literals, whether satisfying given Boolean functions, whether satisfying the given depth limitation
		// version: 2023/6/5
		vector<transistor> transistors_PDN, transistors_PUN;
		// pair<vector<int>, pair<int,int>> Tran2InputVarsandnVars = TransistorExactSynthesis(argv[1], FuncName + "PDN", Funcs, nTransistors, atoi(argv[3]), atoi(argv[2]));
		LiteralsPDN = TransistorExactSynthesis(argv[1], FuncName + "PDN", Funcs, nTransistors, transistors_PDN, 0, OutInvFlags[0], atoi(argv[3]), atoi(argv[2]), DepthLimited, AccFlag1, AccFlag2);
		// version: 2023/4/29
		SAT *= LiteralsPDN.second.first;
		SATDepthLimited *= LiteralsPDN.second.second;
		/*vector<int> Tran2InputVars = Tran2InputVarsandnVars.first;
		int nVars = Tran2InputVarsandnVars.second.first;
		int nInitTransistors = Tran2InputVarsandnVars.second.second;
		vector<int> vPreSetTransistors = Tran2InputVars;*/
		
		// pair<vector<int>, pair<int, int>> Tran2InputVarsPUNandnVars = TransistorExactSynthesis(argv[1], FuncName + "PUN", InputNeg(Funcs), Tran2InputVars.size(), atoi(argv[3]), Tran2InputVars.size() - nInitTransistors, { vPreSetTransistors });
		if (!OnlyPDNFlag) {
			pair<vector<string>, int> InputNegFuncs = InputNeg(Funcs, argv[1]);
			LiteralsPUN = TransistorExactSynthesis(argv[1], FuncName + "PUN", InputNegFuncs.first, InputNegFuncs.second, transistors_PUN, 1, OutInvFlags[0], atoi(argv[3]), atoi(argv[2]), DepthLimited, AccFlag1, AccFlag2);
			SAT *= LiteralsPUN.second.first;
			SATDepthLimited *= LiteralsPUN.second.second;
		}
		auto end_time = chrono::high_resolution_clock::now();
		auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

		set<string> LiteralsAll;
		for (auto literal : LiteralsPDN.first)
			LiteralsAll.insert(literal);
		for (auto literal : LiteralsPUN.first)
			LiteralsAll.insert(literal);
		// version:2023/6/5
		GenerateTransistors(LiteralsAll, transistors_PUN, transistors_PDN, OutInvFlags[0]);
		string spiceName = FuncName;
		for (int c = 0; c < Funcs.size(); c++)
			spiceName.erase(spiceName.begin());
		writeSpice(spiceName + ".sp", spiceName, LiteralsAll, transistors_PUN, transistors_PDN);
		// version: 2023/4/29
		// remove the first characters '!', ' '
		for (int i = 0; i < Funcs.size(); i++)
			FuncName.erase(FuncName.begin());
		if (CountInputINV) {
			if (DepthLimited)
				fExcel << FuncName << "," << NPTransistorsWithINV(LiteralsAll, LiteralsPDN.first.size() + LiteralsPUN.first.size(), OutInvFlags) << "," << elapsed_time.count() << "," << string((SAT) ? "SAT" : "UNSAT") << "," << SATDepthLimited << endl;
			else
				fExcel << FuncName << "," << NPTransistorsWithINV(LiteralsAll, LiteralsPDN.first.size() + LiteralsPUN.first.size(), OutInvFlags) << "," << elapsed_time.count() << "," << string((SAT) ? "SAT" : "UNSAT") << endl;
			cout << "Total transistors: " << NPTransistorsWithINV(LiteralsAll, LiteralsPDN.first.size() + LiteralsPUN.first.size(), OutInvFlags) << endl;
		}	
		else {
			if (DepthLimited)
				fExcel << FuncName << "," << NPTransistorsWithoutINV(LiteralsPDN.first.size() + LiteralsPUN.first.size(), OutInvFlags) << "," << elapsed_time.count() << "," << string((SAT) ? "SAT" : "UNSAT") << "," << SATDepthLimited << endl;
			else
				fExcel << FuncName << "," << NPTransistorsWithoutINV(LiteralsPDN.first.size() + LiteralsPUN.first.size(), OutInvFlags) << "," << elapsed_time.count() << "," << string((SAT) ? "SAT" : "UNSAT") << endl;
			cout << "Total transistors: " << NPTransistorsWithoutINV(LiteralsPDN.first.size() + LiteralsPUN.first.size(), OutInvFlags) << endl;
		}


		//	// the following can be annotated
		//	int SatFlag = 0;
		//	int OutInvFlag = (FuncName[0] == '!') ? 1 : 0;
		//	// version_tmp: 2023/3/30
		//	vector<int> OutInvFlags;
		//	for (int i = 0; i < Funcs.size(); i++) {
		//		if (FuncName[i] == '!')
		//			OutInvFlags.push_back(1);
		//		else
		//			OutInvFlags.push_back(0);
		//	}
		//	for (int i = 0; i < Funcs.size(); i++)
		//		FuncName.erase(FuncName.begin());
		//	auto start_time = chrono::high_resolution_clock::now();
		//	while (!SatFlag) {
		//		// nTransistors = 7;
		//		cout << endl << FuncName << " with " << nTransistors << " transistors: " << endl;
		//		TransistorNtk TMultiOutNtk = ConstructMultiOutTransistorNtk(nTransistors, Funcs.size(), FuncName);
		//		MultiOutTransistorCNF TMultiCnf(TMultiOutNtk, Funcs.size());
		//		TMultiCnf.InitVecMultiOutsCnfs(Funcs);
		//		/*TMultiCnf.InitGlobalVars();
		//		TMultiCnf.CreateGlobalClauses();*/
		//		vector<int> Tran2InputVars = TMultiCnf.CreatePriorClauses();

		//		int nRemainTransistors = TMultiOutNtk.iTransistors.size() - Tran2InputVars.size();
		//		nRemainTransistors = (nRemainTransistors < atoi(argv[2])) ? nRemainTransistors : atoi(argv[2]);
		//		if (nRemainTransistors) {
		//			int nCases = int(pow(Tran2InputVars.size(), nRemainTransistors));
		//			for (int i = 0; i < nCases; i++) {
		//				MultiOutTransistorCNF TMultiCnfTmp = TMultiCnf;
		//				vector<int> Tran2InputVarsTmp = Tran2InputVars;
		//				// create priorclauses for remaining transistors
		//				int iCase = i;
		//				for (int j = 0; j < nRemainTransistors; j++) {
		//					int iLiteral = iCase % Tran2InputVars.size();
		//					TMultiCnfTmp.CreateTransistorInputClauses(j + Tran2InputVars.size(), Tran2InputVars[iLiteral]);
		//					Tran2InputVarsTmp.push_back(Tran2InputVars[iLiteral]);
		//					iCase /= Tran2InputVars.size();
		//				}

		//				TMultiCnfTmp.MultiOutCreateClauses(Tran2InputVarsTmp);
		//				TMultiCnfTmp.GetAllClauses();
		//				TMultiCnfTmp.WriteCnf(argv[1] + TMultiOutNtk.NtkName + ".cnf");
		//				system(("minisat " + string(argv[1]) + TMultiOutNtk.NtkName + ".cnf " + string(argv[1]) + TMultiOutNtk.NtkName + "_out.cnf").c_str());
		//				// system(("minisat ./" + TMultiOutNtk.NtkName + ".cnf ./" + TMultiOutNtk.NtkName + "_out.cnf").c_str());
		//				TMultiCnfTmp.ParseCnf(argv[1] + TMultiOutNtk.NtkName + "_out.cnf", argv[1]);
		//				// TMultiCnfTmp.ParseCnf("./" + TMultiOutNtk.NtkName + "_out.cnf");
		//				if (TMultiCnfTmp.GetSatResult()) {
		//					SatFlag = 1;
		//					auto end_time = chrono::high_resolution_clock::now();
		//					auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
		//					fExcel << TMultiOutNtk.NtkName << "," << NPTransistorsWithINV(Tran2InputVarsTmp, TMultiCnfTmp.GetnVars(), nTransistors, OutInvFlags) << "," << elapsed_time.count() << "," << "SAT" << endl;

		//					break;
		//				}
		//			}
		//		}
		//		else {
		//			TMultiCnf.MultiOutCreateClauses(Tran2InputVars);
		//			TMultiCnf.GetAllClauses();
		//			TMultiCnf.WriteCnf(argv[1] + TMultiOutNtk.NtkName + ".cnf");
		//			system(("timeout 3600 minisat " + string(argv[1]) + TMultiOutNtk.NtkName + ".cnf " + string(argv[1]) + TMultiOutNtk.NtkName + "_out.cnf").c_str());
		//			TMultiCnf.ParseCnf(argv[1] + TMultiOutNtk.NtkName + "_out.cnf", argv[1]);
		//			if (TMultiCnf.GetSatResult()) {
		//				SatFlag = 1;
		//				auto end_time = chrono::high_resolution_clock::now();
		//				auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
		//				fExcel << TMultiOutNtk.NtkName << "," << NPTransistorsWithINV(Tran2InputVars, TMultiCnf.GetnVars(), nTransistors, OutInvFlags) << "," << elapsed_time.count() << "," << "SAT" << endl;
		//				// fExcel << TMultiOutNtk.NtkName << "," << nTransistors << "," << "UNSAT" << endl;
		//			}
		//		}
		//		nTransistors++;
		//	}
		//	
		//}
		//
		//fExcel.close();
	}


	
	return 0;
}