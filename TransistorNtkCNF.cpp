#include "TransistorNtkCNF.h"
#include <algorithm>
#include <set>
#include <map>

MultiOutTransistorCNF::MultiOutTransistorCNF() {

}

MultiOutTransistorCNF::~MultiOutTransistorCNF() {

}


MultiOutTransistorCNF::MultiOutTransistorCNF(TransistorNtk ntk, int nOuts) :TransistorCNF(ntk) {
	/*TNtk = ntk;
	nCnfVars = 0;*/
	// TransistorCNF(ntk);
	nOutputs = nOuts;
}

//void MultiOutTransistorCNF::InitVecMultiOutsCnfs(vector<string> BoolFuncs) {
//	for (int i = 0; i < nOutputs; i++) {
//		TransistorNtk ntk = GetTransistorNtk();
//		ntk.RemoveNode(ntk.Nodes.size(), ntk.NtkName + "_copy1");
//		TransistorCNF TCnf(ntk);
//		TCnf.SetnCnfVars(GetnCnfVars());
//		TCnf.InputParser(BoolFuncs[i]);
//		TCnf.InitAccVecVars();
//		TCnf.CreateAccClausesWithoutGlobal();
//		vector<int> Tran2InputVars = TCnf.CreatePriorClauses();
//		TCnf.CreateAdvancedPosClauses(Tran2InputVars);
//		// update nCnfVars
//		SetnCnfVars(TCnf.GetnCnfVars());
//	}
//}

// version: 2023/3/19
// initialize parameters
//void MultiOutTransistorCNF::InitVecMultiOutsCnfs(vector<string> BoolFuncs) {
//	for (int i = 0; i < nOutputs; i++) {
//		TransistorNtk ntk = GetTransistorNtk();
//		ntk.RemoveNode(ntk.Nodes.size() - i - 1, ntk.NtkName + "_copy" + to_string(i));
//		TransistorCNF TCnf(ntk);
//		MultiOutsCnfs.push_back(TCnf);
//	}
//	MultiOutInputParser(BoolFuncs);
//}

// version: 2023/3/21
// initialize parameters
void MultiOutTransistorCNF::InitVecMultiOutsCnfs(vector<string> BoolFuncs, string dir) {
	for (int i = 0; i < nOutputs; i++) {
		TransistorCNF TCnf; // blanket version
		// version: 2023/5/9
		TCnf.SetAccFlags(GetAccFlag1(), GetAccFlag2());
		MultiOutsCnfs.push_back(TCnf);
	}
	MultiOutInputParser(BoolFuncs, dir);
}

void MultiOutTransistorCNF::MultiOutCreateClauses(vector<int> Tran2InputVars) {
	for (int i = 0; i < nOutputs; i++) {
		MultiOutsCnfs[i].SetnCnfVars(GetnCnfVars());
		MultiOutsCnfs[i].InitAccVecVars();
		MultiOutsCnfs[i].CreateAccClausesWithoutGlobal();
		MultiOutsCnfs[i].CreateAdvancedPosClauses(Tran2InputVars);
		// version: 2023/4/29
		if(DepthLimited)
			MultiOutsCnfs[i].CreateKLimitedPathConstraints(Ks[i]); // create K limited path constraints 2023/4/23
		SetnCnfVars(MultiOutsCnfs[i].GetnCnfVars());
	}
	// version:2023/3/22
	// CreateAccFinalClauses();
	int nInternalEdges = GetTransistorNtk().Edges.size() - GetTransistorNtk().iTransistors.size();
	vector<vector<int>> vvIEdgesCnfFs(GetTransistorNtk().Edges.size());
	for (int i = 0; i < nOutputs; i++) {
		for (int j = 0; j < MultiOutsCnfs[i].GetTransistorNtk().Edges.size(); j++) {
			TransistorEdge e = MultiOutsCnfs[i].GetTransistorNtk().Edges[j];
			if (!e.s.val) {
				for (int k = 0; k < e.CnfVecVarsF1.size(); k++) {
					if (k < MultiOutsCnfs[i].GetRepPattern(1).size()) {
						vvIEdgesCnfFs[e.TmpId].push_back(e.CnfVecVarsF1[k]);
						vvIEdgesCnfFs[e.TmpId].push_back(e.CnfVecVarsF2[k]);
					}
				}
			}
		}
	}
	for (int i = 0; i < vvIEdgesCnfFs.size(); i++) {
		TransistorEdge e = GetTransistorNtk().Edges[i];
		if (!e.s.val)
			CreateXNORClause(vvIEdgesCnfFs[i], { e.CnfVarCnt });
	}
}

void MultiOutTransistorCNF::MultiOutInputParser(vector<string> BoolFuncs, string dir) {
	vector<string> posFuncs, negFuncs;
	for (auto boolfunc : BoolFuncs) {
		// preprocess
		removeSpace(boolfunc);
		posFuncs.push_back(boolfunc);
		cout << "On-Func: " << boolfunc << endl;
		ofstream f1;
		f1.open(dir + "NegFunc.eqn");
		f1 << "Func=!(" << boolfunc << ");" << endl;
		f1.close();
		system((string("sis -c \"read_eqn ") + dir + string("NegFunc.eqn;simplify;write_eqn ") + dir + string("NegFunc_out.eqn;\"")).c_str());
		negFuncs.push_back(ParseEqn(dir + "NegFunc_out.eqn"));
		cout << "Off-Func: " << *(negFuncs.end() - 1) << endl;
	}
	
	MultiOutInputParser(posFuncs, negFuncs);
}

void MultiOutTransistorCNF::GetAllClauses() {
	vector<vector<int>> mainClauses = GetClauses();
	for (int i = 0; i < MultiOutsCnfs.size(); i++)
		for (auto clause : MultiOutsCnfs[i].GetClauses())
			mainClauses.push_back(clause);
	SetClauses(mainClauses);
}

void TransistorCNF::SetVarIndPosNeg(map<char, int>& VarId) {
	VarIndPos.resize(nInputs);
	VarIndNeg.resize(nInputs);
	for (int i = 0; i < svars.size(); i++) {
		char c = svars[i];
		assert(VarId[c] != 0);
		if (VarId[c] & 0b01)
			VarIndNeg[i] = 1;
		if (VarId[c] & 0b10)
			VarIndPos[i] = 1;
		VarId[c] = i;
	}
}

void MultiOutTransistorCNF::MultiOutInputParser(vector<string> onFuncs, vector<string> offFuncs) {
	set<char> vars;
	map<char, int> VarId;
	vector<vector<vector<string>>> vvOnProducts, vvOffProducts;
	for (int i = 0; i < onFuncs.size(); i++) {
		string onFunc = onFuncs[i], offFunc = offFuncs[i];
		// preprocess
		removeSpace(onFunc);
		removeSpace(offFunc);

		vector<string> onProducts = (onFunc.size()) ? split(onFunc, '+') : vector<string>{};
		vector<string> offProducts = (offFunc.size()) ? split(offFunc, '+') : vector<string>{};
		vector<vector<string>> vOnProducts, vOffProducts;
		
		// derive the sum of products for onset and offset
		for (auto s : onProducts)
			vOnProducts.push_back(split(s, '*'));
		for (auto s : offProducts)
			vOffProducts.push_back(split(s, '*'));
		vvOnProducts.push_back(vOnProducts); vvOffProducts.push_back(vOffProducts);
		// derive all variables and Var2Id (tmp)
		for (auto product : vOnProducts) {
			for (auto literal : product) {
				if (literal[0] == '!') {
					vars.insert(literal[1]);
					VarId[literal[1]] |= 1;
				}
				else {
					vars.insert(literal[0]);
					VarId[literal[0]] |= 2;
				}
			}
		}
	}
	// version: 2023/5/4
	for (auto vOnProducts : vvOnProducts)
		for (auto OnProducts : vOnProducts)
			for (auto literal : OnProducts)
				AllLiterals.push_back(literal);

	map<char, int> VarIdTmp = VarId;
	for (auto var : vars)
		updateSvars(var);
	SetnInputs(vars.size());
	SetVarIndPosNeg(VarId);
	VarId = VarIdTmp;
	// initial global variables
	InitGlobalVars();
	CreateGlobalClauses();

	//// version: 2023/3/26
	//updateNtkTmpIDs();
	//for (int i = 0; i < nOutputs; i++) {
	//	TransistorNtk ntk = GetTransistorNtk();
	//	vector<int> RemoveNodeIds;
	//	for (int j = 0; j < nOutputs; j++) {
	//		if (j != i)
	//			RemoveNodeIds.push_back(ntk.Nodes.size() - 1 - j);
	//	}
	//	for (auto id : RemoveNodeIds) {
	//		for (int j = ntk.Nodes.size() - 1; j >= 0; j--) {
	//			if (ntk.Nodes[j].TmpId == id) {
	//				ntk.RemoveNode(ntk.Nodes[j].id, ntk.NtkName + "_copy" + to_string(i));
	//				break;
	//			}
	//		}
	//	}
	//	MultiOutsCnfs[i].SetTNtk(ntk);
	//}

	// version: 2023/4/6
	updateNtkTmpIDs();
	for (int i = 0; i < nOutputs; i++) {
		TransistorNtk ntk = GetTransistorNtk();
		ntk.NtkName += ("_copy" + to_string(i));
		for (int j = 0; j < nOutputs; j++) {
			if (j != i)
				ntk.Nodes[ntk.Nodes.size() - nOutputs + j].type = 0;
		}
		MultiOutsCnfs[i].SetTNtk(ntk);
	}

	// update each MultiOutsCnfs[i]'s parameters and reppatterns
	for (int i = 0; i < nOutputs; i++) {
		for (auto var : vars)
			MultiOutsCnfs[i].updateSvars(var);
		MultiOutsCnfs[i].SetnInputs(vars.size());
		MultiOutsCnfs[i].SetVarIndPosNeg(VarId);
		// derive PosFunc and NegFunc
		MultiOutsCnfs[i].SetOnsets(MultiOutsCnfs[i].DeriveFunc(vvOnProducts[i], VarId));
		MultiOutsCnfs[i].SetOffsets(MultiOutsCnfs[i].DeriveFunc(vvOffProducts[i], VarId));
		// derive representative input patterns
		MultiOutsCnfs[i].DeriveRepPattern(1);
		MultiOutsCnfs[i].DeriveRepPattern(0);
		MultiOutsCnfs[i].Debug();
		// version: 2023/4/22
		Ks.push_back(MultiOutsCnfs[i].getK());
		VarId = VarIdTmp;
	}
	
}

TransistorCNF::TransistorCNF()
{
}

TransistorCNF::~TransistorCNF()
{
}

TransistorCNF::TransistorCNF(TransistorNtk ntk) {
	TNtk = ntk;
	nCnfVars = 0;
}

void TransistorCNF::CreateANDClause(vector<int> cnfvars) {
	for (auto cnfvar : cnfvars)
		clauses.push_back({ cnfvar });
}

void TransistorCNF::CreateXNORClause(vector<int> cnfvars1, vector<int> cnfvars2) {
	for (auto cnfvar1 : cnfvars1) {
		vector<int> clause = cnfvars2;
		clause.push_back(-cnfvar1);
		clauses.push_back(clause);
	}
	for (auto cnfvar2 : cnfvars2) {
		vector<int> clause = cnfvars1;
		clause.push_back(-cnfvar2);
		clauses.push_back(clause);
	}
}

void TransistorCNF::CreateORClause(vector<int> cnfvars) {
	clauses.push_back(cnfvars);
}

void TransistorCNF::CreateAtMostOneClause(vector<int> cnfvars) {
	for (int i = 0; i < cnfvars.size() - 1; i++)
		for (int j = i + 1; j < cnfvars.size(); j++)
			clauses.push_back({ -cnfvars[i],-cnfvars[j] });
}

void combinationsHelper(vector<int>& nums, int start, int n, vector<int>& combination, vector<vector<int>>& result) {
	if (n == 0) {
		result.push_back(combination);
		return;
	}
	for (int i = start; i <= nums.size() - n; i++) {
		combination.push_back(nums[i]);
		combinationsHelper(nums, i + 1, n - 1, combination, result);
		combination.pop_back();
	}
}

vector<vector<int>> combinations(vector<int>& nums, int n) {
	vector<vector<int>> result;
	vector<int> combination;
	combinationsHelper(nums, 0, n, combination, result);
	return result;
}

// version: 2023/4/23
void TransistorCNF::CreateAtMostKClause(vector<int> cnfvars, int K) {
	if (K < cnfvars.size()) {
		vector<vector<int>> vCnfvars = combinations(cnfvars, K + 1);
		for (int j = 0; j < vCnfvars.size(); j++) {
			for (int k = 0; k < vCnfvars[j].size(); k++)
				vCnfvars[j][k] = -vCnfvars[j][k];
			clauses.push_back(vCnfvars[j]);
		}
	}
}

void TransistorCNF::CreateKLimitedPathConstraints(int K) {
	for (int i = 0; i < PosRepPattern.size(); i++) {
		vector<int> TransistorFlowCnfVars;
		for (auto iTrans : TNtk.iTransistors) {
			TransistorEdge e = TNtk.Edges[iTrans];
			TransistorFlowCnfVars.push_back(e.CnfVecVarsF1[i]);
			TransistorFlowCnfVars.push_back(e.CnfVecVarsF2[i]);
		}
		CreateAtMostKClause(TransistorFlowCnfVars, K);
	}
}

// version: 2023/4/15
void TransistorCNF::CreateNoMoreNClauses(vector<int> cnfvars, int n) {
	for (int i = n + 1; i <= cnfvars.size(); i++) {
		vector<vector<int>> vCnfvars = combinations(cnfvars, i);
		for (int j = 0; j < vCnfvars.size(); j++) {
			for (int k = 0; k < vCnfvars[j].size(); k++)
				vCnfvars[j][k] = -vCnfvars[j][k];
			clauses.push_back(vCnfvars[j]);
		}
	}
}

void TransistorCNF::InitVars() {
	nCnfVars = 0;
	for (int i = 1; i < TNtk.Nodes.size() - 1; i++) {
		nCnfVars++;
		TNtk.Nodes[i].CnfVar = nCnfVars;
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		nCnfVars++; TNtk.Edges[i].CnfVarC1 = nCnfVars;
		nCnfVars++; TNtk.Edges[i].CnfVarC2 = nCnfVars;
		nCnfVars++; TNtk.Edges[i].CnfVarF1 = nCnfVars;
		nCnfVars++; TNtk.Edges[i].CnfVarF2 = nCnfVars;
		if (TNtk.Edges[i].s.val) {
			nCnfVars++;
			TNtk.Edges[i].CnfVarSw = nCnfVars;
		}
	}
}

void TransistorCNF::InitVecVars() {
	/*nCnfVars = 0;*/
	for (int i = 1; i < TNtk.Nodes.size() - 1; i++) {
		TNtk.Nodes[i].CnfVecVars.resize(1 << nInputs);
		for (int j = 0; j < (1 << nInputs); j++) {
			nCnfVars++;
			TNtk.Nodes[i].CnfVecVars[j] = nCnfVars;
		}
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TNtk.Edges[i].CnfVecVarsC1.resize(1 << nInputs);
		TNtk.Edges[i].CnfVecVarsC2.resize(1 << nInputs);
		TNtk.Edges[i].CnfVecVarsF1.resize(1 << nInputs);
		TNtk.Edges[i].CnfVecVarsF2.resize(1 << nInputs);
		if (TNtk.Edges[i].s.val)
			TNtk.Edges[i].CnfVecVarsSw.resize(1 << nInputs);
		for (int j = 0; j < (1 << nInputs); j++) {
			nCnfVars++; TNtk.Edges[i].CnfVecVarsC1[j] = nCnfVars;
			nCnfVars++; TNtk.Edges[i].CnfVecVarsC2[j] = nCnfVars;
			nCnfVars++; TNtk.Edges[i].CnfVecVarsF1[j] = nCnfVars;
			nCnfVars++; TNtk.Edges[i].CnfVecVarsF2[j] = nCnfVars;
			if (TNtk.Edges[i].s.val) {
				nCnfVars++;
				TNtk.Edges[i].CnfVecVarsSw[j] = nCnfVars;
			}
		}
	}
}

void TransistorCNF::InitAccVecVars() {
	/*for (int i = 1; i < TNtk.Nodes.size() - 1; i++) {
		TNtk.Nodes[i].CnfVecVars.resize(PosRepPattern.size() + NegRepPattern.size());
		for (int j = 0; j < TNtk.Nodes[i].CnfVecVars.size(); j++) {
			nCnfVars++;
			TNtk.Nodes[i].CnfVecVars[j] = nCnfVars;
		}
	}*/
	// version: 2023/4/6
	for (int i = 1; i < TNtk.Nodes.size(); i++) {
		if (TNtk.Nodes[i].type == 2)
			continue;
		TNtk.Nodes[i].CnfVecVars.resize(PosRepPattern.size() + NegRepPattern.size());
		for (int j = 0; j < TNtk.Nodes[i].CnfVecVars.size(); j++) {
			nCnfVars++;
			TNtk.Nodes[i].CnfVecVars[j] = nCnfVars;
		}
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TNtk.Edges[i].CnfVecVarsC1.resize(PosRepPattern.size() + NegRepPattern.size());
		TNtk.Edges[i].CnfVecVarsC2.resize(PosRepPattern.size() + NegRepPattern.size());
		TNtk.Edges[i].CnfVecVarsF1.resize(PosRepPattern.size() + NegRepPattern.size());
		TNtk.Edges[i].CnfVecVarsF2.resize(PosRepPattern.size() + NegRepPattern.size());
		if (TNtk.Edges[i].s.val)
			TNtk.Edges[i].CnfVecVarsSw.resize(PosRepPattern.size() + NegRepPattern.size());
		for (int j = 0; j < (PosRepPattern.size() + NegRepPattern.size()); j++) {
			nCnfVars++; TNtk.Edges[i].CnfVecVarsC1[j] = nCnfVars;
			nCnfVars++; TNtk.Edges[i].CnfVecVarsC2[j] = nCnfVars;
			nCnfVars++; TNtk.Edges[i].CnfVecVarsF1[j] = nCnfVars;
			nCnfVars++; TNtk.Edges[i].CnfVecVarsF2[j] = nCnfVars;
			if (TNtk.Edges[i].s.val) {
				nCnfVars++;
				TNtk.Edges[i].CnfVecVarsSw[j] = nCnfVars;
			}
		}
	}
}

void TransistorCNF::InitGlobalVars() {
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		if (TNtk.Edges[i].s.val) {
			TNtk.Edges[i].CnfVarInputs.resize(2 * nInputs);
			for (int j = 0; j < 2 * nInputs; j++) {
				nCnfVars++; TNtk.Edges[i].CnfVarInputs[j] = nCnfVars;
			}
		}
		else {
			nCnfVars++; TNtk.Edges[i].CnfVarCnt = nCnfVars;
		}
	}
}

void TransistorCNF::WriteCnf(string cnfpath) {
	ofstream fCnf;
	fCnf.open(cnfpath.c_str());
	fCnf << "p cnf " << nCnfVars << " " << clauses.size() << endl;
	for (auto clause : clauses) {
		for (auto cnfvar : clause)
			fCnf << cnfvar << " ";
		fCnf << "0" << endl;
	}
		
	fCnf.close();
}

void TransistorCNF::CreateGlobalClauses() {
	for (int i = 0; i < TNtk.iTransistors.size(); i++) {
		TransistorEdge Transistor = TNtk.Edges[TNtk.iTransistors[i]];
		vector<int> cnfvars;
		for (int j = 0; j < nInputs; j++) {
			if (VarIndPos[j])
				cnfvars.push_back(Transistor.CnfVarInputs[j]);
			if (VarIndNeg[j])
				cnfvars.push_back(Transistor.CnfVarInputs[nInputs + j]);
		}
		CreateORClause(cnfvars);
		CreateAtMostOneClause(cnfvars);
	}
}

//void TransistorCNF::CreateGlobalClauses() {
//	for (int i = 0; i < TNtk.iTransistors.size(); i++) {
//		TransistorEdge Transistor = TNtk.Edges[TNtk.iTransistors[i]];
//		CreateORClause(Transistor.CnfVarInputs);
//		CreateAtMostOneClause(Transistor.CnfVarInputs);
//	}
//}

void TransistorCNF::CreateNegClauses() {
	// 10110
	int iP = 22;
	int iTmp = iP;
	for (auto iTran : TNtk.iTransistors) {
		if (iTmp % 2)
			clauses.push_back({ TNtk.Edges[iTran].CnfVecVarsC1[iP] });
		else
			clauses.push_back({ -TNtk.Edges[iTran].CnfVecVarsC1[iP] });
		iTmp /= 2;
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		auto it = find(TNtk.iTransistors.begin(), TNtk.iTransistors.end(), e.id);
		if (it == TNtk.iTransistors.end())
			clauses.push_back({ e.CnfVecVarsC1[iP] });
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		if (n1.type == 1) {
			clauses.push_back({ -n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
		}
		if (n1.type == 2) {
			clauses.push_back({ n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
		}
		if (n2.type == 1) {
			clauses.push_back({ -n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
		}
		if (n2.type == 2) {
			clauses.push_back({ n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
		}
		if (!n1.type && !n2.type) {
			clauses.push_back({ n1.CnfVecVars[iP],-n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			clauses.push_back({ -n1.CnfVecVars[iP],n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
		}
	}
}

void TransistorCNF::CreateNegClauses(int iP) {
	// 10110
	for (auto iTran : TNtk.iTransistors) {
		vector<int> cnfvars;
		int iTmp = iP;
		for (int i = 0; i < nInputs; i++) {
			if (iTmp % 2) {
				if (VarIndPos[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[i]);
			}
			else {
				if (VarIndNeg[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[nInputs + i]);
			}
			iTmp /= 2;
		}
		CreateXNORClause(cnfvars, { TNtk.Edges[iTran].CnfVecVarsC1[iP] });
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		auto it = find(TNtk.iTransistors.begin(), TNtk.iTransistors.end(), e.id);
		if (it == TNtk.iTransistors.end())
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		if (n1.type == 1) {
			if (n2.type == 2)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ -n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (n1.type == 2) {
			if (n2.type == 1)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (n2.type == 1) {
			if (n1.type == 2)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ -n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (n2.type == 2) {
			if (n1.type == 1)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (!n1.type && !n2.type) {
			clauses.push_back({ n1.CnfVecVars[iP],-n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			clauses.push_back({ -n1.CnfVecVars[iP],n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
	}
}

void TransistorCNF::CreateAccNegClauses(int iP) {
	// 10110
	for (auto iTran : TNtk.iTransistors) {
		vector<int> cnfvars;
		for (int i = 0; i < nInputs; i++) {
			if (NegRepPattern[iP - PosRepPattern.size()][i]) {
				if (VarIndPos[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[i]);
			}
			else {
				if (VarIndNeg[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[nInputs + i]);
			}
		}
		CreateXNORClause(cnfvars, { TNtk.Edges[iTran].CnfVecVarsC1[iP] });
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		auto it = find(TNtk.iTransistors.begin(), TNtk.iTransistors.end(), e.id);
		if (it == TNtk.iTransistors.end())
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
	}
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		if (n1.type == 1) {
			if (n2.type == 2)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ -n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (n1.type == 2) {
			if (n2.type == 1)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (n2.type == 1) {
			if (n1.type == 2)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ -n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (n2.type == 2) {
			if (n1.type == 1)
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
			else
				clauses.push_back({ n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
		if (!n1.type && !n2.type) {
			clauses.push_back({ n1.CnfVecVars[iP],-n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			clauses.push_back({ -n1.CnfVecVars[iP],n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
			continue;
		}
	}
}

void TransistorCNF::CreatePosClauses() {
	for (int iP = (1 << TNtk.nTransistors) - 1; iP < (1 << TNtk.nTransistors); iP++) {
		// transistor value constraints
		int iTmp = iP;
		for (auto iTran : TNtk.iTransistors) {
			if (iTmp % 2)
				clauses.push_back({ TNtk.Edges[iTran].CnfVecVarsSw[iP] });
			else
				clauses.push_back({ -TNtk.Edges[iTran].CnfVecVarsSw[iP] });
			iTmp /= 2;
		}
		// node constraints
		/*for (int i = 0; i < TNtk.Edges.size(); i++) {
			TransistorEdge e = TNtk.Edges[i];
			TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
			if (n1.type == 1)
				clauses.push_back({ -n2.CnfVecVars[iP] });
			if (n2.type == 1)
				clauses.push_back({ -n1.CnfVecVars[iP] });
			if (n1.type == 2)
				clauses.push_back({ n2.CnfVecVars[iP] });
			if (n2.type == 2)
				clauses.push_back({ n1.CnfVecVars[iP] });
			if (!n1.type && !n2.type) {
				clauses.push_back({ n1.CnfVecVars[iP],n2.CnfVecVars[iP] });
				clauses.push_back({ -n1.CnfVecVars[iP],-n2.CnfVecVars[iP] });
			}
		}*/

		// edge capacity constraints
		for (int i = 0; i < TNtk.Edges.size(); i++) {
			TransistorEdge e = TNtk.Edges[i];
			TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
			if (n1.type == 1) {
				clauses.push_back({ n2.CnfVecVars[iP], e.CnfVecVarsC1[iP] });
				clauses.push_back({ -n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
				clauses.push_back({ -e.CnfVecVarsC2[iP] });
			}
			if (n1.type == 2) {
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
				clauses.push_back({ -n2.CnfVecVars[iP], e.CnfVecVarsC2[iP] });
				clauses.push_back({ n2.CnfVecVars[iP], -e.CnfVecVarsC2[iP] });
			}
			if (n2.type == 1) {
				clauses.push_back({ -e.CnfVecVarsC1[iP] });
				clauses.push_back({ n2.CnfVecVars[iP], e.CnfVecVarsC2[iP] });
				clauses.push_back({ -n2.CnfVecVars[iP], -e.CnfVecVarsC2[iP] });
			}
			if (n2.type == 2) {
				clauses.push_back({ -n1.CnfVecVars[iP], e.CnfVecVarsC1[iP] });
				clauses.push_back({ n1.CnfVecVars[iP], -e.CnfVecVarsC1[iP] });
				clauses.push_back({ -e.CnfVecVarsC2[iP] });
			}
			if (!n1.type && !n2.type) {
				if (e.s.val) {
					clauses.push_back({ -n1.CnfVecVars[iP],n2.CnfVecVars[iP],-e.CnfVecVarsSw[iP],e.CnfVecVarsC2[iP] });
					clauses.push_back({ n1.CnfVecVars[iP],-e.CnfVecVarsC2[iP] });
					clauses.push_back({ -n2.CnfVecVars[iP],-e.CnfVecVarsC2[iP] });
					clauses.push_back({ e.CnfVecVarsSw[iP],-e.CnfVecVarsC2[iP] });
					clauses.push_back({ n1.CnfVecVars[iP],-n2.CnfVecVars[iP],-e.CnfVecVarsSw[iP],e.CnfVecVarsC1[iP] });
					clauses.push_back({ -n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
					clauses.push_back({ n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
					clauses.push_back({ e.CnfVecVarsSw[iP],-e.CnfVecVarsC1[iP] });
				}
				else {
					clauses.push_back({ -n1.CnfVecVars[iP],n2.CnfVecVars[iP],e.CnfVecVarsC1[iP] });
					clauses.push_back({ n1.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
					clauses.push_back({ -n2.CnfVecVars[iP],-e.CnfVecVarsC1[iP] });
					clauses.push_back({ n1.CnfVecVars[iP],-n2.CnfVecVars[iP],e.CnfVecVarsC2[iP] });
					clauses.push_back({ -n1.CnfVecVars[iP],-e.CnfVecVarsC2[iP] });
					clauses.push_back({ n2.CnfVecVars[iP],-e.CnfVecVarsC2[iP] });
				}
			}
		}

		// edge flow constraints
		for (int i = 0; i < TNtk.Edges.size(); i++) {
			TransistorEdge e = TNtk.Edges[i];
			TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
			clauses.push_back({ e.CnfVecVarsC1[iP],-e.CnfVecVarsF1[iP] });
			clauses.push_back({ e.CnfVecVarsC2[iP],-e.CnfVecVarsF2[iP] });
		}
		for (int i = 0; i < TNtk.Nodes.size(); i++) {
			TransistorNode n = TNtk.Nodes[i];
			if (n.type == 1) {
				vector<int> clause, cnfOutVars, cnfInVars;
				for (auto eid : n.neighborEdges) {
					TransistorEdge eTmp = TNtk.Edges[eid];
					TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
					if (n1.id == n.id) {
						cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
						cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
					}
					else if (n2.id == n.id) {
						cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
						cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
					}
				}
				CreateORClause(cnfOutVars);
				CreateAtMostOneClause(cnfOutVars);
			}
			if (n.type == 2) {
				vector<int> clause, cnfOutVars, cnfInVars;
				for (auto eid : n.neighborEdges) {
					TransistorEdge eTmp = TNtk.Edges[eid];
					TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
					if (n1.id == n.id) {
						cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
						cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
					}
					else if (n2.id == n.id) {
						cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
						cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
					}
				}
				CreateORClause(cnfInVars);
			}
			if (n.type == 0) {
				vector<int> clause, cnfOutVars, cnfInVars;
				for (auto eid : n.neighborEdges) {
					TransistorEdge eTmp = TNtk.Edges[eid];
					TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
					if (n1.id == n.id) {
						cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
						cnfInVars.push_back(eTmp.CnfVecVarsF2[iP]);
					}
					else if (n2.id == n.id) {
						cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
						cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
					}
				}
				CreateAtMostOneClause(cnfInVars);
				CreateAtMostOneClause(cnfOutVars);
				CreateXNORClause(cnfInVars, cnfOutVars);
			}
		}
	}
}

void TransistorCNF::CreateFinalClauses() {
	for (auto e : TNtk.Edges) {
		if (!e.s.val) {
			vector<int> cnfvars;
			for (int i = 0; i < e.CnfVecVarsF1.size(); i++) {
				if (TruthTable[i]) {
					cnfvars.push_back(e.CnfVecVarsF1[i]);
					cnfvars.push_back(e.CnfVecVarsF2[i]);
				}
			}
			CreateXNORClause(cnfvars, { e.CnfVarCnt });
		}
	}
}

void TransistorCNF::CreateAccFinalClauses() {
	for (auto e : TNtk.Edges) {
		if (!e.s.val) {
			vector<int> cnfvars;
			for (int i = 0; i < e.CnfVecVarsF1.size(); i++) {
				if (i < PosRepPattern.size()) {
					cnfvars.push_back(e.CnfVecVarsF1[i]);
					cnfvars.push_back(e.CnfVecVarsF2[i]);
				}
			}
			CreateXNORClause(cnfvars, { e.CnfVarCnt });
		}
	}
}

void TransistorCNF::CreateClauses() {
	CreateGlobalClauses();
	for (int i = 0; i < TruthTable.size(); i++) {
		if (TruthTable[i])
			CreatePosClauses(i);
		else
			CreateNegClauses(i);
	}
	CreateFinalClauses();
}

void TransistorCNF::CreateAccClauses() {
	CreateGlobalClauses();
	for (int i = 0; i < PosRepPattern.size(); i++)
		CreateAccPosClauses(i);
	for (int i = 0; i < NegRepPattern.size(); i++)
		CreateAccNegClauses(i + PosRepPattern.size());
	CreateAccFinalClauses();
}

void TransistorCNF::CreateAccClausesWithoutGlobal() {
	for (int i = 0; i < PosRepPattern.size(); i++)
		CreateAccPosClauses(i);
	for (int i = 0; i < NegRepPattern.size(); i++)
		CreateAccNegClauses(i + PosRepPattern.size());
	// CreateAccFinalClauses(); // just annotate for debug! Successfully!!!
}

void TransistorCNF::CreatePosClauses(int iP) {
	// transistor value constraints
	for (auto iTran : TNtk.iTransistors) {
		vector<int> cnfvars;
		int iTmp = iP;
		// version: 2023/1/10
		for (int i = 0; i < nInputs; i++) {
			if (iTmp % 2) {
				if (VarIndPos[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[i]);
			}
			else {
				if (VarIndNeg[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[nInputs + i]);
			}
			iTmp /= 2;
		}
		CreateXNORClause(cnfvars, { TNtk.Edges[iTran].CnfVecVarsSw[iP] });
	}

	// edge capacity constraints
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		if (n1.type == 1) {
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
			clauses.push_back({ -e.CnfVecVarsC2[iP] });
			continue;
		}
		if (n1.type == 2) {
			clauses.push_back({ -e.CnfVecVarsC1[iP] });
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC2[iP] });
			continue;
		}
		if (n2.type == 1) {
			clauses.push_back({ -e.CnfVecVarsC1[iP] });
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC2[iP] });
			continue;
		}
		if (n2.type == 2) {
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
			clauses.push_back({ -e.CnfVecVarsC2[iP] });
			continue;
		}
		if (!n1.type && !n2.type) {
			if (e.s.val) {
				CreateXNORClause({ e.CnfVecVarsSw[iP] }, { e.CnfVecVarsC1[iP] });
				CreateXNORClause({ e.CnfVecVarsSw[iP] }, { e.CnfVecVarsC2[iP] });
			}
			else {
				CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
				CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC2[iP] });
			}
			continue;
		}
	}

	// edge flow constraints
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		clauses.push_back({ e.CnfVecVarsC1[iP],-e.CnfVecVarsF1[iP] });
		clauses.push_back({ e.CnfVecVarsC2[iP],-e.CnfVecVarsF2[iP] });
	}
	for (int i = 0; i < TNtk.Nodes.size(); i++) {
		TransistorNode n = TNtk.Nodes[i];
		if (n.type == 1) {
			vector<int> clause, cnfOutVars, cnfInVars;
			for (auto eid : n.neighborEdges) {
				TransistorEdge eTmp = TNtk.Edges[eid];
				TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
				if (n1.id == n.id) {
					cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
				else if (n2.id == n.id) {
					cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
			}
			CreateORClause(cnfOutVars);
			CreateAtMostOneClause(cnfOutVars);
		}
		if (n.type == 2) {
			vector<int> clause, cnfOutVars, cnfInVars;
			for (auto eid : n.neighborEdges) {
				TransistorEdge eTmp = TNtk.Edges[eid];
				TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
				if (n1.id == n.id) {
					cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
				else if (n2.id == n.id) {
					cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
			}
			CreateORClause(cnfInVars);
		}
		if (n.type == 0) {
			vector<int> clause, cnfOutVars, cnfInVars;
			for (auto eid : n.neighborEdges) {
				TransistorEdge eTmp = TNtk.Edges[eid];
				TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
				if (n1.id == n.id) {
					cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfInVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
				else if (n2.id == n.id) {
					cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
			}
			CreateAtMostOneClause(cnfInVars);
			CreateAtMostOneClause(cnfOutVars);
			CreateXNORClause(cnfInVars, cnfOutVars);
		}
	}
}

// version: 2023/3/18
// also global clauses
vector<int> TransistorCNF::CreatePriorClauses() {
	vector<int> VarInputIds;
	for (int i = 0; i < VarIndPos.size(); i++) {
		if (VarIndPos[i])
			VarInputIds.push_back(i);
	}
	for (int i = 0; i < VarIndNeg.size(); i++) {
		if (VarIndNeg[i])
			VarInputIds.push_back(i + nInputs);
	}
	// set the corresponding literals of transistors in advance
	for (int i = 0; i < TNtk.iTransistors.size(); i++) {
		TransistorEdge e = TNtk.Edges[TNtk.iTransistors[i]];
		if (i >= VarInputIds.size())
			break;
		for (int j = 0; j < e.CnfVarInputs.size(); j++) {
			if (j == VarInputIds[i])
				clauses.push_back({ e.CnfVarInputs[j] });
			else
				clauses.push_back({ -e.CnfVarInputs[j] });
		}
	}
	return VarInputIds;
}

// version: 2023/3/20
void TransistorCNF::CreateTransistorInputClauses(int iTransistor,int iLiteral) {
	// set the corresponding literal of the transistor
	TransistorEdge e = TNtk.Edges[TNtk.iTransistors[iTransistor]];
	for (int j = 0; j < e.CnfVarInputs.size(); j++) {
		if (j == iLiteral)
			clauses.push_back({ e.CnfVarInputs[j] });
		else
			clauses.push_back({ -e.CnfVarInputs[j] });
	}
}

void TransistorCNF::CreateAdvancedPosClauses(vector<int> Tran2InputVars) {
	// current version: Tran2InputVars.size() == TNtk.iTransistors.size()
	// future version:  the number of transistors is larger than the number of Tran2InputVars
	for (int i = 0; i < PosRepPattern.size(); i++) {
		for (int j = 0; j < Tran2InputVars.size(); j++) {
			TransistorEdge e = TNtk.Edges[TNtk.iTransistors[j]];
			// whether this transistor edge is closed
			int VarId = (Tran2InputVars[j] >= nInputs) ? Tran2InputVars[j] - nInputs : Tran2InputVars[j];
			int TranOpen;
			// compute whether the current transistor is closed
			if (Tran2InputVars[j] >= nInputs)
				TranOpen = 1 - PosRepPattern[i][VarId];
			else
				TranOpen = PosRepPattern[i][VarId];
			if (!TranOpen) {
				// post-process the connected edges with the current transistor
				TransistorNode n1 = TNtk.Nodes[e.id1];
				TransistorNode n2 = TNtk.Nodes[e.id2];
				for (auto eid : n1.neighborEdges) {
					TransistorEdge e1 = TNtk.Edges[eid];
					clauses.push_back({ -e1.CnfVecVarsF1[i] });
					clauses.push_back({ -e1.CnfVecVarsF2[i] });
				}
				for (auto eid : n2.neighborEdges) {
					TransistorEdge e2 = TNtk.Edges[eid];
					clauses.push_back({ -e2.CnfVecVarsF1[i] });
					clauses.push_back({ -e2.CnfVecVarsF2[i] });
				}
			}
		}
	}
}

void TransistorCNF::CreateAdvancedNegClauses(vector<int> Tran2InputVars) {
	for (int i = 0; i < NegRepPattern.size(); i++) {
		for (int j = 0; j < TNtk.iTransistors.size(); j++) {
			TransistorEdge e = TNtk.Edges[TNtk.iTransistors[j]];
			int VarId = (Tran2InputVars[j] >= nInputs) ? Tran2InputVars[j] - nInputs : Tran2InputVars[j];
			int TranOpen;
			if (Tran2InputVars[j] >= nInputs)
				TranOpen = 1 - NegRepPattern[i][VarId];
			else
				TranOpen = NegRepPattern[i][VarId];
			if (!TranOpen) {
				// post-process the connected edges with the current transistor
				TransistorNode n1 = TNtk.Nodes[e.id1];
				TransistorNode n2 = TNtk.Nodes[e.id2];
				CreateXNORClause({ n1.CnfVecVars[PosRepPattern.size() + i] }, { n2.CnfVecVars[PosRepPattern.size() + i] });
			}
		}
	}
}

void TransistorCNF::CreateAccPosClauses(int iP) {
	// transistor value constraints
	for (auto iTran : TNtk.iTransistors) {
		vector<int> cnfvars;
		// version: 2023/1/10
		for (int i = 0; i < nInputs; i++) {
			if (PosRepPattern[iP][i]) {
				if (VarIndPos[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[i]);
			}
			else {
				if (VarIndNeg[i])
					cnfvars.push_back(TNtk.Edges[iTran].CnfVarInputs[nInputs + i]);
			}
		}
		CreateXNORClause(cnfvars, { TNtk.Edges[iTran].CnfVecVarsSw[iP] });
	}

	// edge capacity constraints
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		if (n1.type == 1) {
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
			clauses.push_back({ -e.CnfVecVarsC2[iP] });
			continue;
		}
		if (n1.type == 2) {
			clauses.push_back({ -e.CnfVecVarsC1[iP] });
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC2[iP] });
			continue;
		}
		if (n2.type == 1) {
			clauses.push_back({ -e.CnfVecVarsC1[iP] });
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC2[iP] });
			continue;
		}
		if (n2.type == 2) {
			CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
			clauses.push_back({ -e.CnfVecVarsC2[iP] });
			continue;
		}
		if (!n1.type && !n2.type) {
			if (e.s.val) {
				CreateXNORClause({ e.CnfVecVarsSw[iP] }, { e.CnfVecVarsC1[iP] });
				CreateXNORClause({ e.CnfVecVarsSw[iP] }, { e.CnfVecVarsC2[iP] });
			}
			else {
				CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC1[iP] });
				CreateXNORClause({ e.CnfVarCnt }, { e.CnfVecVarsC2[iP] });
			}
			continue;
		}
	}

	// edge flow constraints
	for (int i = 0; i < TNtk.Edges.size(); i++) {
		TransistorEdge e = TNtk.Edges[i];
		TransistorNode n1 = TNtk.Nodes[e.id1], n2 = TNtk.Nodes[e.id2];
		clauses.push_back({ e.CnfVecVarsC1[iP],-e.CnfVecVarsF1[iP] });
		clauses.push_back({ e.CnfVecVarsC2[iP],-e.CnfVecVarsF2[iP] });
		// version: 2023/3/22
		clauses.push_back({ -e.CnfVecVarsF1[iP],-e.CnfVecVarsF2[iP] });
	}
	for (int i = 0; i < TNtk.Nodes.size(); i++) {
		TransistorNode n = TNtk.Nodes[i];
		if (n.type == 1) {
			vector<int> clause, cnfOutVars, cnfInVars;
			for (auto eid : n.neighborEdges) {
				TransistorEdge eTmp = TNtk.Edges[eid];
				TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
				if (n1.id == n.id) {
					cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
					// cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]); // version: 2023/4/15
					cnfInVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
				else if (n2.id == n.id) {
					cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
			}
			CreateORClause(cnfOutVars);
			CreateAtMostOneClause(cnfOutVars);
		}
		if (n.type == 2) {
			vector<int> clause, cnfOutVars, cnfInVars;
			for (auto eid : n.neighborEdges) {
				TransistorEdge eTmp = TNtk.Edges[eid];
				TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
				if (n1.id == n.id) {
					cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
					// cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]); // version: 2023/4/15
					cnfInVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
				else if (n2.id == n.id) {
					cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
			}
			CreateORClause(cnfInVars);
		}
		if (n.type == 0) {
			vector<int> clause, cnfOutVars, cnfInVars;
			for (auto eid : n.neighborEdges) {
				TransistorEdge eTmp = TNtk.Edges[eid];
				TransistorNode n1 = TNtk.Nodes[eTmp.id1], n2 = TNtk.Nodes[eTmp.id2];
				if (n1.id == n.id) {
					cnfOutVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfInVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
				else if (n2.id == n.id) {
					cnfInVars.push_back(eTmp.CnfVecVarsF1[iP]);
					cnfOutVars.push_back(eTmp.CnfVecVarsF2[iP]);
				}
			}
			CreateAtMostOneClause(cnfInVars);
			CreateAtMostOneClause(cnfOutVars);
			CreateXNORClause(cnfInVars, cnfOutVars);
		}
	}
}

void TransistorCNF::ParsePosCnf(string cnfpath) {
	ifstream fCnf;
	string sline;
	stringstream ss;
	fCnf.open(cnfpath.c_str());
	getline(fCnf, sline);
	if (sline != "SAT")
		cout << "UNSAT" << endl;
	else {
		vector<int> vals;
		getline(fCnf, sline);
		ss << sline;
		int val;
		int iP = (1 << TNtk.nTransistors) - 1;
		while (ss >> val) {
			if (val > 0)
				vals.push_back(1);
			if (val < 0)
				vals.push_back(0);
		}
		cout << "Edge flows: " << endl;
		for (auto e : TNtk.Edges) {
			cout << e.id1 << "->" << e.id2 << ": " << vals[e.CnfVecVarsF1[iP] - 1] << endl;
			cout << e.id2 << "->" << e.id1 << ": " << vals[e.CnfVecVarsF2[iP] - 1] << endl;
			cout << endl;
		}
		cout << "Node type: " << endl;
		for (int i = 1; i < TNtk.Nodes.size() - 1; i++) {
			TransistorNode n = TNtk.Nodes[i];
			cout << n.id << ": ";
			if (vals[n.CnfVecVars[iP] - 1])
				cout << "Drain" << endl;
			else
				cout << "Source" << endl;
		}
	}
	fCnf.close();
}

void TransistorCNF::ParseNegCnf(string cnfpath) {
	ifstream fCnf;
	string sline;
	stringstream ss;
	fCnf.open(cnfpath.c_str());
	getline(fCnf, sline);
	if (sline != "SAT")
		cout << "UNSAT" << endl;
	else {
		vector<int> vals;
		getline(fCnf, sline);
		ss << sline;
		int val;
		int iP = 22;
		while (ss >> val) {
			if (val > 0)
				vals.push_back(1);
			if (val < 0)
				vals.push_back(0);
		}
		cout << "Edge capacity: " << endl;
		for (auto e : TNtk.Edges) {
			cout << e.id1 << "->" << e.id2 << ": " << vals[e.CnfVecVarsC1[iP] - 1] << endl;
			cout << endl;
		}
		cout << "Node type: " << endl;
		for (int i = 1; i < TNtk.Nodes.size() - 1; i++) {
			TransistorNode n = TNtk.Nodes[i];
			cout << n.id << ": ";
			if (vals[n.CnfVecVars[iP] - 1])
				cout << "Drain" << endl;
			else
				cout << "Source" << endl;
		}
	}
	fCnf.close();
}

void DrawAbstractedGraph(string filename, map<pair<int, int>, string> transistors, vector<pair<int, int>> edges, vector<int> OutIds = {}) {
	Graph graph(filename, false);
	ofstream f1;
	vector<size_t> vNodes;
	f1.open((filename + ".dot").c_str());
	auto& registry = graph.GetRegistry();
	int nNodes = transistors.size() * 2 + 2;
	
	for (int i = 0; i < nNodes; i++) {
		auto n = registry.RegisterNode(to_string(i));
		if (find(OutIds.begin(), OutIds.end(), i) != OutIds.end())
			registry.RegisterNodeAttr(n, Attrs_color, "red");
		graph.AddNode(n);
		vNodes.push_back(n);
	}
	// transistor edges
	map<string, int> counterMap;
	for (auto it : transistors) {
		counterMap[it.second] += 1;
		auto n = registry.RegisterNode(it.second + to_string(counterMap[it.second]));
		registry.RegisterNodeAttr(n, Attrs_shape, "polygon");
		graph.AddNode(n);
		graph.AddEdge(registry.RegisterEdge(n, it.first.first));
		graph.AddEdge(registry.RegisterEdge(n, it.first.second));
	}
	// internal edges
	for (int i = 0; i < edges.size(); i++)
		graph.AddEdge(registry.RegisterEdge(edges[i].first, edges[i].second));
		
	f1 << graph.Dump() << endl;
	f1.close();
	system(("dot " + filename + ".dot -T pdf -o " + filename + ".pdf").c_str());

}

// version: 2023/4/6
void DrawAbstractedGraph(string filename, vector<pair<pair<int, int>, string>> transistors, vector<pair<int, int>> edges, vector<int> OutIds = {}) {
	Graph graph(filename, false);
	ofstream f1;
	vector<size_t> vNodes;
	f1.open((filename + ".dot").c_str());
	auto& registry = graph.GetRegistry();
	int nNodes = transistors.size() * 2 + 2;

	for (int i = 0; i < nNodes; i++) {
		auto n = registry.RegisterNode(to_string(i));
		if (find(OutIds.begin(), OutIds.end(), i) != OutIds.end())
			registry.RegisterNodeAttr(n, Attrs_color, "red");
		graph.AddNode(n);
		vNodes.push_back(n);
	}
	// transistor edges
	map<string, int> counterMap;
	for (auto it : transistors) {
		counterMap[it.second] += 1;
		auto n = registry.RegisterNode(it.second + to_string(counterMap[it.second]));
		registry.RegisterNodeAttr(n, Attrs_shape, "polygon");
		graph.AddNode(n);
		graph.AddEdge(registry.RegisterEdge(n, it.first.first));
		graph.AddEdge(registry.RegisterEdge(n, it.first.second));
	}
	// internal edges
	for (int i = 0; i < edges.size(); i++)
		graph.AddEdge(registry.RegisterEdge(edges[i].first, edges[i].second));

	f1 << graph.Dump() << endl;
	f1.close();
	system(("dot " + filename + ".dot -T pdf -o " + filename + ".pdf").c_str());

}

void BreadFirstSearch(int expandNode, set<int>& Nodes, map<int,vector<int>> neighborNodes) {
	Nodes.insert(expandNode);
	vector<int> cntNodes = neighborNodes[expandNode];
	for (auto node : cntNodes) {
		if (!Nodes.count(node))
			BreadFirstSearch(node, Nodes, neighborNodes);
	}
}



vector<set<int>> GraphCluster(vector<pair<int, int>> edges) {
	map<int, vector<int>> neighborNodes;
	for (auto edge : edges) {
		neighborNodes[edge.first].push_back(edge.second);
		neighborNodes[edge.second].push_back(edge.first);
	}
	int nNodes = neighborNodes.size();
	vector<set<int>> clusters;
	set<int> remainNodes;
	for (int i = 0; i < nNodes; i++)
		remainNodes.insert(i);
	while (remainNodes.size()) {
		int expandNode = *remainNodes.begin();
		set<int> cluster;
		BreadFirstSearch(expandNode, cluster, neighborNodes);
		clusters.push_back(cluster);
		// update remaining nodes
		set<int> updateNodes;
		for (auto n : remainNodes)
			if (!cluster.count(n))
				updateNodes.insert(n);
		remainNodes = updateNodes;
	}
	return clusters;
 }

//vector<string> TransistorCNF::ParseCnf(string cnfpath, string path) {
//	ifstream fCnf;
//	string sline;
//	stringstream ss;
//
//	// version: 2023/02/22
//	vector<pair<int, int>> edges;
//	map<pair<int, int>, string> transistors;
//
//	// version: 2023/4/4
//	// vector<int> Tran2InputVars;
//	vector<string> Literals;
//
//	fCnf.open(cnfpath.c_str());
//	getline(fCnf, sline);
//	if (sline != "SAT") {
//		cout << "UNSAT" << endl;
//		SatResult = 0;
//	}
//	else {
//		SatResult = 1;
//		vector<int> vals;
//		getline(fCnf, sline);
//		ss << sline;
//		int val;
//		int iP = (1 << TNtk.nTransistors) - 1;
//		while (ss >> val) {
//			if (val > 0)
//				vals.push_back(1);
//			if (val < 0)
//				vals.push_back(0);
//		}
//		// cout << "Edge connections: " << endl;
//		for (auto e : TNtk.Edges) {
//			if (!e.s.val) {
//				if (vals[e.CnfVarCnt - 1]) {
//					// cout << e.id1 << "-" << e.id2 << endl;
//					edges.push_back(make_pair(e.id1, e.id2));
//				}
//			}
//		}
//		// cout << "Transistor types: " << endl;
//		for (auto eid : TNtk.iTransistors) {
//			TransistorEdge e = TNtk.Edges[eid];
//			// cout << e.id1 << "-" << e.id2 << ": ";
//			for (int i = 0; i < e.CnfVarInputs.size(); i++) {
//				int ind;
//				if (i >= nInputs)
//					ind = VarIndNeg[i - nInputs];
//				else
//					ind = VarIndPos[i];
//				// ind: indicate whether this literal exists
//				// some CnfVarInputs are useless cnf variables
//				if (ind && vals[e.CnfVarInputs[i] - 1]) {
//					if (i >= nInputs) {
//						string s = "-";
//						s.push_back(svars[i - nInputs]);
//						Literals.push_back(string{ '!',svars[i - nInputs] });
//						// cout << "-" << svars[i - nInputs] << endl;
//						transistors[make_pair(e.id1, e.id2)] = s;
//					}
//					else {
//						string s = "";
//						s.push_back(svars[i]);
//						Literals.push_back(string{ svars[i] });
//						// cout << svars[i] << endl;
//						transistors[make_pair(e.id1, e.id2)] = s;
//					}
//					/*if (i >= nInputs)
//						cout << "-" << char('a' + i - nInputs) << endl;
//					else
//						cout << char('a' + i) << endl;*/
//				}
//			}
//		}
//
//		/*cout << "Debug: " << endl;
//		TransistorEdge e35 = TNtk.Edges[25], e59 = TNtk.Edges[38], e39 = TNtk.Edges[29];
//		for (int i = 0; i < e35.CnfVecVarsF1.size(); i++) {
//			cout << "Pattern " << i << ": ";
//			cout << "f35=" << vals[e35.CnfVecVarsF1[i] - 1] << "; f53=" << vals[e35.CnfVecVarsF2[i] - 1] << " ";
//			cout << "f39=" << vals[e39.CnfVecVarsF1[i] - 1] << "; f93=" << vals[e39.CnfVecVarsF2[i] - 1] << " ";
//			cout << "f59=" << vals[e59.CnfVecVarsF1[i] - 1] << "; f95=" << vals[e59.CnfVecVarsF2[i] - 1];
//			cout << endl;
//		}*/
//	}
//	fCnf.close();
//
//	// DrawAbstractedGraph("Abstracted_" + TNtk.NtkName, transistors, edges);
//
//	vector<set<int>> clusters = GraphCluster(edges);
//
//	int nNodes = 2 * transistors.size() + 2;
//	for (int i = 0; i < clusters.size(); i++)
//		if (clusters[i].count(nNodes - 1))
//			swap(clusters[i], *(clusters.end() - 1));
//
//	/*for (int i = 0; i < clusters.size(); i++) {
//		cout << "cluster " << i << " : ";
//		for (auto node : clusters[i])
//			cout << node << " ";
//		cout << endl;
//	}*/
//
//	map<pair<int, int>,string> transistors_new;
//	vector<pair<pair<int, int>, string>> transistors_pairs;
//	for (auto it : transistors) {
//		int id1 = it.first.first;
//		int id2 = it.first.second;
//		int id1_new, id2_new;
//		for (int i = 0; i < clusters.size(); i++) {
//			if (clusters[i].count(id1))
//				id1_new = i;
//			if (clusters[i].count(id2))
//				id2_new = i;
//		}
//		transistors_new[make_pair(id1_new, id2_new)] = it.second;
//		transistors_pairs.push_back(make_pair(make_pair(id1_new, id2_new), it.second));
//	}
//	// version:2023/3/22
//	// Find the output clusters
//	vector<int> OutClusters;
//	for (int i = 0; i < clusters.size(); i++) {
//		int Flag = 0;
//		for (int j = 0; j < GetTransistorNtk().nOutputs; j++) {
//			int OutId = (GetTransistorNtk().Nodes.end() - 1 - j)->id;
//			if (clusters[i].count(OutId))
//				OutClusters.push_back(i);
//		}
//	}
//	// version: 2023/4/6
//	// DrawAbstractedGraph(path + "Transistors_" + TNtk.NtkName, transistors_new, {}, OutClusters);
//	DrawAbstractedGraph(path + "Transistors_" + TNtk.NtkName, transistors_pairs, {}, OutClusters);
//	return Literals;
//}

void TransistorCNF::CreateBlockClause(vector<int> vals) {
	vector<int> clause;
	for (int i = 0; i < vals.size(); i++) {
		if (vals[i])
			clause.push_back(-1 * (i + 1));
		else
			clause.push_back(i + 1);
	}
	clauses.push_back(clause);
}

vector<string> TransistorCNF::ParseCnf(int mos, vector<transistor>& Transistors, int OutINV, string cnfpath, string path) {
	ifstream fCnf;
	string sline;
	stringstream ss;

	// version: 2023/02/22
	vector<pair<int, int>> edges;
	map<pair<int, int>, string> transistors;

	// version: 2023/4/4
	// vector<int> Tran2InputVars;
	vector<string> Literals;

	fCnf.open(cnfpath.c_str());
	getline(fCnf, sline);
	if (sline != "SAT") {
		cout << "UNSAT" << endl;
		SatResult = 0;
	}
	else {
		SatResult = 1;
		vector<int> vals;
		getline(fCnf, sline);
		ss << sline;
		int val;
		int iP = (1 << TNtk.nTransistors) - 1;
		while (ss >> val) {
			if (val > 0)
				vals.push_back(1);
			if (val < 0)
				vals.push_back(0);
		}
		// cout << "Edge connections: " << endl;
		for (auto e : TNtk.Edges) {
			if (!e.s.val) {
				if (vals[e.CnfVarCnt - 1]) {
					// cout << e.id1 << "-" << e.id2 << endl;
					edges.push_back(make_pair(e.id1, e.id2));
				}
			}
		}
		// cout << "Transistor types: " << endl;
		for (auto eid : TNtk.iTransistors) {
			TransistorEdge e = TNtk.Edges[eid];
			// cout << e.id1 << "-" << e.id2 << ": ";
			for (int i = 0; i < e.CnfVarInputs.size(); i++) {
				int ind;
				if (i >= nInputs)
					ind = VarIndNeg[i - nInputs];
				else
					ind = VarIndPos[i];
				// ind: indicate whether this literal exists
				// some CnfVarInputs are useless cnf variables
				if (ind && vals[e.CnfVarInputs[i] - 1]) {
					if (i >= nInputs) {
						// version: 2023/4/26
						// string s = "-";
						string s = "!";
						s.push_back(svars[i - nInputs]);
						// Literals.push_back(string{ '!',svars[i - nInputs] }); // version: 2023/4/26
						// cout << "-" << svars[i - nInputs] << endl;
						transistors[make_pair(e.id1, e.id2)] = s;
					}
					else {
						string s = "";
						s.push_back(svars[i]);
						// Literals.push_back(string{ svars[i] }); // version: 2023/4/26
						// cout << svars[i] << endl;
						transistors[make_pair(e.id1, e.id2)] = s;
					}
					/*if (i >= nInputs)
						cout << "-" << char('a' + i - nInputs) << endl;
					else
						cout << char('a' + i) << endl;*/
				}
			}
		}

		/*cout << "Debug: " << endl;
		TransistorEdge e35 = TNtk.Edges[25], e59 = TNtk.Edges[38], e39 = TNtk.Edges[29];
		for (int i = 0; i < e35.CnfVecVarsF1.size(); i++) {
			cout << "Pattern " << i << ": ";
			cout << "f35=" << vals[e35.CnfVecVarsF1[i] - 1] << "; f53=" << vals[e35.CnfVecVarsF2[i] - 1] << " ";
			cout << "f39=" << vals[e39.CnfVecVarsF1[i] - 1] << "; f93=" << vals[e39.CnfVecVarsF2[i] - 1] << " ";
			cout << "f59=" << vals[e59.CnfVecVarsF1[i] - 1] << "; f95=" << vals[e59.CnfVecVarsF2[i] - 1];
			cout << endl;
		}*/
		// version: 2023/4/30
		CreateBlockClause(vals);
	}
	fCnf.close();

	// DrawAbstractedGraph("Abstracted_" + TNtk.NtkName, transistors, edges);

	vector<set<int>> clusters = GraphCluster(edges);

	int nNodes = 2 * transistors.size() + 2;
	for (int i = 0; i < clusters.size(); i++)
		if (clusters[i].count(nNodes - 1))
			swap(clusters[i], *(clusters.end() - 1));

	/*for (int i = 0; i < clusters.size(); i++) {
		cout << "cluster " << i << " : ";
		for (auto node : clusters[i])
			cout << node << " ";
		cout << endl;
	}*/

	map<pair<int, int>, string> transistors_new;
	vector<pair<pair<int, int>, string>> transistors_pairs;
	for (auto it : transistors) {
		int id1 = it.first.first;
		int id2 = it.first.second;
		int id1_new, id2_new;
		for (int i = 0; i < clusters.size(); i++) {
			if (clusters[i].count(id1))
				id1_new = i;
			if (clusters[i].count(id2))
				id2_new = i;
		}
		// version: 2023/4/26
		// for the case: two ports of a transistor connected to a single node in the converted transistor network
		if (id1_new != id2_new) {
			Literals.push_back(it.second);
			transistors_pairs.push_back(make_pair(make_pair(id1_new, id2_new), it.second));
		}
			

		transistors_new[make_pair(id1_new, id2_new)] = it.second;
		// transistors_pairs.push_back(make_pair(make_pair(id1_new, id2_new), it.second));
	}

	// version:2023/3/22
	// Find the output clusters
	vector<int> OutClusters;
	for (int i = 0; i < clusters.size(); i++) {
		int Flag = 0;
		for (int j = 0; j < GetTransistorNtk().nOutputs; j++) {
			// version: 2023/4/26
			// int OutId = (GetTransistorNtk().Nodes.end() - 1 - j)->id;
			int OutId = (GetTransistorNtk().Nodes.end() - GetTransistorNtk().nOutputs + j)->id;
			if (clusters[i].count(OutId))
				OutClusters.push_back(i);
		}
	}

	// version: 2023/4/26
	// Get all possible paths in converted transistor networks
	vector<vector<int>> graph(clusters.size() + transistors_pairs.size());
	int InputClusterID;
	for (int i = 0; i < clusters.size(); i++) {
		if (clusters[i].count(0)) {
			InputClusterID = i;
			break;
		}
	}
	map<int, string> transistorNodes;
	map<int, pair<int, int>> transistorPortIDs;
	for (int i = 0; i < transistors_pairs.size(); i++) {
		pair<pair<int, int>, string> Edge = transistors_pairs[i];
		transistorNodes[clusters.size() + i] = Edge.second;
		transistorPortIDs[clusters.size() + i] = make_pair(Edge.first.first, Edge.first.second);
		graph[Edge.first.first].push_back(clusters.size() + i);
		graph[clusters.size() + i].push_back(Edge.first.first);
		graph[Edge.first.second].push_back(clusters.size() + i);
		graph[clusters.size() + i].push_back(Edge.first.second);
	}
	ResultPaths = {};
	for (auto OutClusterID : OutClusters) {
		ResultPaths.push_back(getSimpleTransistorPaths(graph, InputClusterID, OutClusterID, transistorNodes));
	}

	//version: 2023/6/5
	// currently only support single output functions
	// vector<pair<pair<int, int>, string>> transistors_ordered_pairs = bfs(graph,InputClusterID,OutClusters[0],)
	if (SatResult) {
		vector<vector<int>> TransistorPathIDs = getSimpleTransistorIDPaths(graph, InputClusterID, OutClusters[0], transistorNodes);
		map<pair<int, int>, int> transistorMaxCounts;
		for (auto TransistorPath : TransistorPathIDs) {
			for (auto TransistorID : TransistorPath) {
				int val = transistorMaxCounts[transistorPortIDs[TransistorID]];
				transistorMaxCounts[transistorPortIDs[TransistorID]] = max(val, int(TransistorPath.size()));
			}
		}
		/*vector<pair<pair<int, int>, int>> transistors_Counts;
		for (auto transistor_pair : transistors_pairs) {
			transistors_Counts.push_back(make_pair(transistor_pair.first, transistorMaxCounts[transistor_pair.first]));
		}*/
		Transistors = transformTransistors(clusters.size(), InputClusterID, OutClusters[0], transistors_pairs, transistorMaxCounts, mos, OutINV);
	}
		
	
	
	// version: 2023/4/6
	// DrawAbstractedGraph(path + "Transistors_" + TNtk.NtkName, transistors_new, {}, OutClusters);
	DrawAbstractedGraph(path + "Transistors_" + TNtk.NtkName, transistors_pairs, {}, OutClusters);

	
	return Literals;
}

vector<string> TransistorCNF::ParseCnf(string cnfpath, map<pair<int, int>, int>& AllEdges, map<pair<int, int>, string>& transistors, map<pair<int, int>, int>& transistors_cnfvar, string path) {
	ifstream fCnf;
	string sline;
	stringstream ss;

	// version: 2023/02/22
	vector<pair<int, int>> edges;
	// map<pair<int, int>, string> transistors; // version: 2023/4/22

	// version: 2023/4/4
	// vector<int> Tran2InputVars;
	vector<string> Literals;

	vector<int> vals;
	fCnf.open(cnfpath.c_str());
	getline(fCnf, sline);
	if (sline != "SAT") {
		cout << "UNSAT" << endl;
		SatResult = 0;
	}
	else {
		SatResult = 1;
		
		getline(fCnf, sline);
		ss << sline;
		int val;
		int iP = (1 << TNtk.nTransistors) - 1;
		while (ss >> val) {
			if (val > 0)
				vals.push_back(1);
			if (val < 0)
				vals.push_back(0);
		}
		// cout << "Edge connections: " << endl;
		for (auto e : TNtk.Edges) {
			if (!e.s.val) {
				if (vals[e.CnfVarCnt - 1]) {
					// cout << e.id1 << "-" << e.id2 << endl;
					edges.push_back(make_pair(e.id1, e.id2));
					AllEdges[make_pair(e.id1, e.id2)] = e.CnfVarCnt;
				}
			}
		}
		// cout << "Transistor types: " << endl;
		for (auto eid : TNtk.iTransistors) {
			TransistorEdge e = TNtk.Edges[eid];
			// cout << e.id1 << "-" << e.id2 << ": ";
			for (int i = 0; i < e.CnfVarInputs.size(); i++) {
				int ind;
				if (i >= nInputs)
					ind = VarIndNeg[i - nInputs];
				else
					ind = VarIndPos[i];
				// ind: indicate whether this literal exists
				// some CnfVarInputs are useless cnf variables
				if (ind && vals[e.CnfVarInputs[i] - 1]) {
					if (i >= nInputs) {
						string s = "-";
						s.push_back(svars[i - nInputs]);
						transistors[make_pair(e.id1, e.id2)] = s;
						transistors_cnfvar[make_pair(e.id1, e.id2)] = e.CnfVarInputs[i];
						Literals.push_back(string{ '!',svars[i - nInputs] });
						// cout << "-" << svars[i - nInputs] << endl;
						// transistors[make_pair(e.id1, e.id2)] = s;
					}
					else {
						string s = "";
						s.push_back(svars[i]);
						transistors[make_pair(e.id1, e.id2)] = s;
						transistors_cnfvar[make_pair(e.id1, e.id2)] = e.CnfVarInputs[i];
						Literals.push_back(string{ svars[i] });
						// cout << svars[i] << endl;
						// transistors[make_pair(e.id1, e.id2)] = s;
					}
					/*if (i >= nInputs)
						cout << "-" << char('a' + i - nInputs) << endl;
					else
						cout << char('a' + i) << endl;*/
				}
			}
		}

		/*cout << "Debug: " << endl;
		TransistorEdge e35 = TNtk.Edges[25], e59 = TNtk.Edges[38], e39 = TNtk.Edges[29];
		for (int i = 0; i < e35.CnfVecVarsF1.size(); i++) {
			cout << "Pattern " << i << ": ";
			cout << "f35=" << vals[e35.CnfVecVarsF1[i] - 1] << "; f53=" << vals[e35.CnfVecVarsF2[i] - 1] << " ";
			cout << "f39=" << vals[e39.CnfVecVarsF1[i] - 1] << "; f93=" << vals[e39.CnfVecVarsF2[i] - 1] << " ";
			cout << "f59=" << vals[e59.CnfVecVarsF1[i] - 1] << "; f95=" << vals[e59.CnfVecVarsF2[i] - 1];
			cout << endl;
		}*/
	}
	fCnf.close();

	// DrawAbstractedGraph("Abstracted_" + TNtk.NtkName, transistors, edges);

	vector<set<int>> clusters = GraphCluster(edges);

	int nNodes = 2 * transistors.size() + 2;
	for (int i = 0; i < clusters.size(); i++)
		if (clusters[i].count(nNodes - 1))
			swap(clusters[i], *(clusters.end() - 1));

	/*for (int i = 0; i < clusters.size(); i++) {
		cout << "cluster " << i << " : ";
		for (auto node : clusters[i])
			cout << node << " ";
		cout << endl;
	}*/

	map<pair<int, int>, string> transistors_new;
	vector<pair<pair<int, int>, string>> transistors_pairs;
	for (auto it : transistors) {
		int id1 = it.first.first;
		int id2 = it.first.second;
		int id1_new, id2_new;
		for (int i = 0; i < clusters.size(); i++) {
			if (clusters[i].count(id1))
				id1_new = i;
			if (clusters[i].count(id2))
				id2_new = i;
		}
		transistors_new[make_pair(id1_new, id2_new)] = it.second;
		transistors_pairs.push_back(make_pair(make_pair(id1_new, id2_new), it.second));
	}

	for (int i = 0; i < PosRepPattern.size(); i++) {
		for (auto iTran : TNtk.iTransistors) {
			TransistorEdge e = TNtk.Edges[iTran];
			int id1_new, id2_new;
			for (int i = 0; i < clusters.size(); i++) {
				if (clusters[i].count(e.id1))
					id1_new = i;
				if (clusters[i].count(e.id2))
					id2_new = i;
			}
			if (vals[e.CnfVecVarsF1[i] - 1])
				cout << id1_new << "->" << id2_new << "=" << transistors[make_pair(e.id1, e.id2)] << " ";
			if (vals[e.CnfVecVarsF2[i] - 1])
				cout << id2_new << "->" << id1_new << "=" << transistors[make_pair(e.id1, e.id2)] << " ";
		}
		cout << endl;
	}

	// version:2023/3/22
	// Find the output clusters
	vector<int> OutClusters;
	for (int i = 0; i < clusters.size(); i++) {
		int Flag = 0;
		for (int j = 0; j < GetTransistorNtk().nOutputs; j++) {
			int OutId = (GetTransistorNtk().Nodes.end() - 1 - j)->id;
			if (clusters[i].count(OutId))
				OutClusters.push_back(i);
		}
	}
	// version: 2023/4/6
	// DrawAbstractedGraph(path + "Transistors_" + TNtk.NtkName, transistors_new, {}, OutClusters);
	DrawAbstractedGraph(path + "Transistors_" + TNtk.NtkName, transistors_pairs, {}, OutClusters);
	return Literals;
}

//void TransistorCNF::DeriveTruthTable() {
//	TruthTable.resize(1 << nInputs);
//	for (int i = 0; i < (1 << nInputs); i++) {
//		int iTmp = i;
//		vector<int> bits(nInputs, 0);
//		for (int j = 0; j < nInputs; j++) {
//			bits[j] = iTmp % 2;
//			iTmp /= 2;
//		}
//
//		int val = 0;
//		for (int j = 0; j < Func.size(); j++) {
//			int product = 1;
//			for (int k = 0; k < Func[j].size(); k++) {
//				if (Func[j][k] >= nInputs)
//					product *= (1 - bits[Func[j][k] - nInputs]);
//				else
//					product *= bits[Func[j][k]];
//			}
//			val += product;
//		}
//		if (val)
//			TruthTable[i] = 1;
//		else
//			TruthTable[i] = 0;
//	}
//}

// version: 2023/4/26
vector<int> TransistorCNF::DeriveTruthTable() {
	TruthTable.resize(1 << nInputs);
	for (int i = 0; i < (1 << nInputs); i++) {
		int iTmp = i;
		vector<int> bits(nInputs, 0);
		for (int j = 0; j < nInputs; j++) {
			bits[j] = iTmp % 2;
			iTmp /= 2;
		}

		int val = 0;
		for (int j = 0; j < Onsets.size(); j++) {
			int product = 1;
			for (int k = 0; k < Onsets[j].size(); k++) {
				if (Onsets[j][k] >= nInputs)
					product *= (1 - bits[Onsets[j][k] - nInputs]);
				else
					product *= bits[Onsets[j][k]];
			}
			val += product;
		}
		if (val)
			TruthTable[i] = 1;
		else
			TruthTable[i] = 0;
	}
	for (auto i : TruthTable)
		cout << i << " ";
	cout << endl;
	return TruthTable;
}

void removeSpace(string& s) {
	for (int i = 0; i < s.size();) {
		if (s[i] == ' ') {
			s.erase(s.begin() + i);
			i--;
		}
		// version: 2023/03/02
		if (s[i] == '.')
			s[i] = '*';
		i++;
	}
}

vector<string> split(string s, char sign) {
	int pos = s.find(sign);
	if (pos == string::npos)
		return { s };
	string substr1 = s.substr(0, pos);
	string substr2 = s.substr(pos + 1, s.size() - pos - 1);
	vector<string> vs = split(substr2, sign);
	vs.insert(vs.begin(), substr1);
	return vs;
}

//void TransistorCNF::InputParser(string boolfunc) {
//	vector<string> products = split(boolfunc, '+');
//	vector<vector<string>> vProducts;
//	set<char> vars;
//	map<char, int> tmp;
//	for (auto s : products)
//		vProducts.push_back(split(s, '*'));
//	for (auto product : vProducts) {// !??*!??+!??*!??*??+!??*!??*!??
//		for (auto literal : product) {
//			if (literal[0] == '!') {
//				vars.insert(literal[1]);
//				tmp[literal[1]] |= 1;
//			}
//			else {
//				vars.insert(literal[0]);
//				tmp[literal[0]] |= 2;
//			}
//		}
//	}
//	for (auto var : vars)
//		svars += var;
//
//	int id = 0;
//	nInputs = vars.size();
//	VarIndPos.resize(nInputs);
//	VarIndNeg.resize(nInputs);
//	for (int i = 0; i < svars.size(); i++) {
//		char c = svars[i];
//		assert(tmp[c] != 0);
//		if (tmp[c] & 0b01)
//			VarIndNeg[i] = 1;
//		if (tmp[c] & 0b10)
//			VarIndPos[i] = 1;
//		tmp[c] = i;
//	}
//	/*for (auto it = vars.begin(); it != vars.end(); it++) {
//		assert(tmp[*it] == 0);
//		if (tmp[*it] & 0x01)
//			VarIndNeg[id] = 1;
//		if (tmp[*it] & 0x10)
//			VarIndPos[id] = 1;
//		tmp[*it] = id;
//		id++;
//	}*/
//	
//	for (auto product : vProducts) {
//		vector<int> func;
//		for (auto literal : product) {
//			if (literal[0] == '!')
//				func.push_back(nInputs + tmp[literal[1]]);
//			else
//				func.push_back(tmp[literal[0]]);
//		}
//		Func.push_back(func);
//	}
//}

//vector<vector<int>> TransistorCNF::DeriveFunc(vector<vector<string>> vProducts,map<char,int> VarId) {
//	vector<vector<int>> vFunc;
//	for (auto product : vProducts) {
//		vector<int> func;
//		for (auto literal : product) {
//			if (literal[0] == '!')
//				func.push_back(nInputs + VarId[literal[1]]);
//			else
//				func.push_back(VarId[literal[0]]);
//		}
//		vFunc.push_back(func);
//	}
//	return vFunc;
//}

// version: 2023/4/3
vector<vector<int>> TransistorCNF::DeriveFunc(vector<vector<string>> vProducts, map<char, int> VarId) {
	vector<vector<int>> vFunc;
	for (auto product : vProducts) {
		vector<int> func;
		for (auto literal : product) {
			if (literal[0] == '!')
				func.push_back(nInputs + VarId[literal[1]]);
			else
				func.push_back(VarId[literal[0]]);
		}
		if (find(vFunc.begin(), vFunc.end(), func) == vFunc.end())
			vFunc.push_back(func);
	}
	return vFunc;
}

// version: 2023/4/26
vector<vector<int>> TransistorCNF::DeriveResultFunc() {
	map<char, int> VarId;
	ResultTruthTables = {};
	for (int i = 0; i < svars.size(); i++)
		VarId[svars[i]] = i;
	for (int i = 0; i < ResultPaths.size(); i++) {
		vector<vector<int>> vFunc;
		for (auto product : ResultPaths[i]) {
			vector<int> func;
			for (auto literal : product) {
				if (literal[0] == '!')
					func.push_back(nInputs + VarId[literal[1]]);
				else
					func.push_back(VarId[literal[0]]);
			}
			if (find(vFunc.begin(), vFunc.end(), func) == vFunc.end())
				vFunc.push_back(func);
		}
		vector<int> Tt;
		Tt.resize(1 << nInputs);
		for (int i = 0; i < (1 << nInputs); i++) {
			int iTmp = i;
			vector<int> bits(nInputs, 0);
			for (int j = 0; j < nInputs; j++) {
				bits[j] = iTmp % 2;
				iTmp /= 2;
			}

			int val = 0;
			for (int j = 0; j < vFunc.size(); j++) {
				int product = 1;
				for (int k = 0; k < vFunc[j].size(); k++) {
					if (vFunc[j][k] >= nInputs)
						product *= (1 - bits[vFunc[j][k] - nInputs]);
					else
						product *= bits[vFunc[j][k]];
				}
				val += product;
			}
			if (val)
				Tt[i] = 1;
			else
				Tt[i] = 0;
		}
		ResultTruthTables.push_back(Tt);
	}
	for (auto Tt : ResultTruthTables) {
		for (auto i : Tt)
			cout << i << " ";
		cout << endl;
	}
	return ResultTruthTables;
}

// version: 2023/3/19
// update: VarIndPos, VarIndNeg, Onsets, Offsets, svars
void TransistorCNF::InputParser(string onFunc,string offFunc) {
	// preprocess
	removeSpace(onFunc);
	removeSpace(offFunc);
	/*cout << onFunc << endl;
	cout << offFunc << endl;*/
	vector<string> onProducts = (onFunc.size()) ? split(onFunc, '+') : vector<string>{};
	vector<string> offProducts = (offFunc.size()) ? split(offFunc, '+') : vector<string>{};
	vector<vector<string>> vOnProducts, vOffProducts;
	set<char> vars;
	map<char, int> VarId;
	// derive the sum of products for onset and offset
	for (auto s : onProducts)
		vOnProducts.push_back(split(s, '*'));
	for (auto s : offProducts)
		vOffProducts.push_back(split(s, '*'));
	// derive all variables and Var2Id (tmp)
	for (auto product : vOnProducts) {
		for (auto literal : product) {
			if (literal[0] == '!') {
				vars.insert(literal[1]);
				VarId[literal[1]] |= 1;
			}
			else {
				vars.insert(literal[0]);
				VarId[literal[0]] |= 2;
			}
		}
	}
	for (auto var : vars)
		svars += var;

	nInputs = vars.size();
	VarIndPos.resize(nInputs);
	VarIndNeg.resize(nInputs);
	for (int i = 0; i < svars.size(); i++) {
		char c = svars[i];
		assert(VarId[c] != 0);
		if (VarId[c] & 0b01)
			VarIndNeg[i] = 1;
		if (VarId[c] & 0b10)
			VarIndPos[i] = 1;
		VarId[c] = i;
	}

	// derive PosFunc and NegFunc
	Onsets = DeriveFunc(vOnProducts, VarId);
	Offsets = DeriveFunc(vOffProducts, VarId);

	// derive representative input patterns
	DeriveRepPattern(1);
	DeriveRepPattern(0);

	// check number of transistors
	int nTra = 0;
	for (auto n : VarIndPos)
		nTra += n;
	for (auto n : VarIndNeg)
		nTra += n;
	// assert(TNtk.nTransistors >= nTra);
}

string ParseEqn(string EqnPath) {
	ifstream f1;
	string Func;
	f1.open(EqnPath.c_str());
	string line;
	int flag = 0;
	while (getline(f1, line)) {
		if (!flag) {
			if (line.find("Func") == 0) {
				Func += split(line, '=')[1];
				flag = 1;
			}
		}
		else
			Func += line;
	}
	f1.close();
	removeSpace(Func);
	// version: 2023/3/29
	Func.erase(Func.end() - 1);
	return Func;
}

// version: 2023/03/03
void TransistorCNF::InputParser(string boolfunc) {
	string posFunc, negFunc;
	// preprocess
	removeSpace(boolfunc);
	posFunc = boolfunc;
	cout << "On-Func: " << posFunc << endl;
	ofstream f1;
	f1.open("NegFunc.eqn");
	f1 << "Func=!(" << posFunc << ");" << endl;
	f1.close();
	system("sis -c \"read_eqn NegFunc.eqn;simplify;write_eqn NegFunc_out.eqn;\"");
	negFunc = ParseEqn("NegFunc_out.eqn");
	cout << "Off-Func: " << negFunc << endl;
	InputParser(posFunc, negFunc);
}

void ParseBoolFuncTxtFile(string TxtPath, map<string, pair<string, int>>& BoolFuncs) {
	ifstream f1;
	string sline;
	f1.open(TxtPath.c_str());
	while (getline(f1,sline)) {
		if (sline[0] == '#')
			continue;
		if (sline != "") {
			vector<string> FuncInfo = split(sline, ':');
			string FuncName = split(FuncInfo[0], '(')[0];
			stringstream ss;
			int nTransistors;
			ss << split(split(FuncInfo[0], '(')[1], ')')[0];
			ss >> nTransistors;
			BoolFuncs[FuncName] = make_pair(FuncInfo[1], nTransistors);
		}
	}
	f1.close();
}

void TransistorCNF::DeriveRepPattern(int flag) {
	unsigned unate = 0;
	// version: 2023/5/9
	if (AccTech1Flag) {
		for (int i = 0; i < nInputs; i++)
			if (VarIndPos[i] + VarIndNeg[i] == 1) // only pos or neg literal exists
				unate |= (1 << i);
	}
	vector<vector<int>> sets = (flag) ? Onsets : Offsets;
	for (auto onProduct : sets) {
		vector<vector<int>> patterns;
		vector<int> pattern(nInputs, 2);
		for (auto literal : onProduct) {
			int var = ((literal >= nInputs) ? literal - nInputs : literal);
			pattern[var] = ((literal >= nInputs) ? 0 : 1);
		}
		patterns.push_back(vector<int>(nInputs, 0)); // an initial pattern
		for (int i = 0; i < pattern.size(); i++) {
			int nPa = patterns.size();
			for (int j = 0; j < nPa; j++) {
				if (pattern[i] != 2)
					patterns[j][i] = pattern[i];
				else {
					if (unate & (1 << i)) {
						if (VarIndPos[i] == 0 && VarIndNeg[i] == 1)
							patterns[j][i] = (flag) ? 1 : 0;
						else if (VarIndPos[i] == 1 && VarIndNeg[i] == 0)
							patterns[j][i] = (flag) ? 0 : 1;
						else
							assert(0);
					}
					else {
						patterns[j][i] = 0;
						vector<int> tmp = patterns[j];
						tmp[i] = 1;
						patterns.push_back(tmp);
					}
				}
			}
		}
		if (flag) {
			if (!PosRepPattern.size())
				PosRepPattern = patterns;
			else
				PosRepPattern.insert(PosRepPattern.end(), patterns.begin(), patterns.end());
			// version: 2023/4/3
			sort(PosRepPattern.begin(), PosRepPattern.end());
			auto last = unique(PosRepPattern.begin(), PosRepPattern.end());
			PosRepPattern.erase(last, PosRepPattern.end());
		}
		else {
			if (!NegRepPattern.size())
				NegRepPattern = patterns;
			else
				NegRepPattern.insert(NegRepPattern.end(), patterns.begin(), patterns.end());
			// version: 2023/4/3
			sort(NegRepPattern.begin(), NegRepPattern.end());
			auto last = unique(NegRepPattern.begin(), NegRepPattern.end());
			NegRepPattern.erase(last, NegRepPattern.end());
		}	
	}
}

void TransistorCNF::showPatterns(vector<vector<int>> patterns) {
	for (int i = 0; i < patterns.size(); i++) {
		for (int j = 0; j < patterns[i].size(); j++) {
			if (patterns[i][j] >= nInputs)
				cout << "!" << svars[patterns[i][j] - nInputs];
			else
				cout << svars[patterns[i][j]];
			if (j < patterns[i].size() - 1)
				cout << "*";
		}
		if (i < patterns.size() - 1)
			cout << "+";
	}
}

void TransistorCNF::showRepPatterns(vector<vector<int>> patterns) {
	for (int i = 0; i < patterns.size(); i++) {
		for (int j = 0; j < patterns[i].size(); j++) {
			if (!patterns[i][j])
				cout << "!" << svars[j];
			else
				cout << svars[j];
			if (j < patterns[i].size() - 1)
				cout << "*";
		}
		if (i < patterns.size() - 1)
			cout << "+";
	}
}

void TransistorCNF::Debug() {
	/*cout << "Onset: ";
	showPatterns(Onsets);
	cout << endl;*/

	cout << "On-RepPatterns: ";
	showRepPatterns(PosRepPattern);
	cout << endl;

	/*cout << "Offset: ";
	showPatterns(Offsets);
	cout << endl;*/

	cout << "Off-RepPatterns: ";
	showRepPatterns(NegRepPattern);
	cout << endl;
}

void SummaryInExcelFile(string ExcelPath) {

}

int NPTransistorsWithINV(vector<int> Tran2InputVars, int nVars,int nInternalTransistors,int OutINVFlag) {
	int nTotalTransistors = nInternalTransistors * 2;
	for (auto iLiteral : Tran2InputVars)
		if (iLiteral >= nVars)
			nTotalTransistors += 2;
	if (OutINVFlag)
		nTotalTransistors += 2;
	return nTotalTransistors;
}

int NPTransistorsWithINV(vector<int> Tran2InputVars, int nVars, int nInternalTransistors, vector<int> OutINVFlags) {
	// int nTotalTransistors = nInternalTransistors * 2;
	int nTotalTransistors = nInternalTransistors;
	for (auto iLiteral : Tran2InputVars)
		if (iLiteral >= nVars)
			nTotalTransistors += 2;
	for (int i = 0; i < OutINVFlags.size(); i++)
		if (OutINVFlags[i])
			nTotalTransistors += 2;
	return nTotalTransistors;
}

int NPTransistorsWithoutINV(int nInternalTransistors, vector<int> OutINVFlags) {
	// int nTotalTransistors = nInternalTransistors * 2;
	int nTotalTransistors = nInternalTransistors;
	for (int i = 0; i < OutINVFlags.size(); i++)
		if (OutINVFlags[i])
			nTotalTransistors += 2;
	return nTotalTransistors;
}

int NPTransistorsWithINV(set<string> InputLiterals, int nInternalTransistors, vector<int> OutINVFlags) {
	int nTotalTransistors = nInternalTransistors;
	for (auto Literal : InputLiterals)
		if (Literal[0] == '!')
			nTotalTransistors += 2;
	for (int i = 0; i < OutINVFlags.size(); i++)
		if (OutINVFlags[i])
			nTotalTransistors += 2;
	return nTotalTransistors;
}