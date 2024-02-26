#include "TransistorNtk.h"
#include <map>
#include <algorithm>
Switch::Switch()
{
}

Switch::Switch(string s) {
	name = s;
	if (name == "Null")
		val = 0;
	else
		val = 1;
}

Switch::~Switch()
{
}

TransistorNode::TransistorNode()
{
}

//TransistorNode::TransistorNode(string s, int Id) {
//	name = s;
//	id = Id;
//	if (name == "VDD")
//		type = 1;
//	else if (name == "Output")
//		type = 2;
//	else
//		type = 0;
//
//}

TransistorNode::TransistorNode(string s, int Id) {
	name = s;
	id = Id;
	if (name == "VDD")
		type = 1;
	else if (name.find("Output") != string::npos)
		type = 2;
	else
		type = 0;

}

TransistorNode::~TransistorNode()
{
}

TransistorEdge::TransistorEdge()
{
}

TransistorEdge::TransistorEdge(int n1, int n2, string sw, int Id) {
	id1 = n1;
	id2 = n2;
	id = Id;
	s = Switch(sw);
}

TransistorEdge::~TransistorEdge()
{
}

TransistorNtk::TransistorNtk()
{
}

TransistorNtk::~TransistorNtk()
{
}

TransistorNtk ConstructTransistorNtk(int nTransistors, string Name) {
	TransistorNtk TNtk;
	TNtk.nTransistors = nTransistors;
	// version: 2023/03/06
	TNtk.NtkName = Name;
	map<pair<int, int>, int> edge_map;
	// construct all nodes
	TransistorNode vdd = TransistorNode("VDD", TNtk.Nodes.size());
	TNtk.Nodes.push_back(vdd);
	for (int i = 0; i < nTransistors; i++) {
		TransistorNode n1 = TransistorNode("n" + to_string(2 * i + 1), TNtk.Nodes.size());
		TNtk.Nodes.push_back(n1);
		TransistorNode n2 = TransistorNode("n" + to_string(2 * i + 2), TNtk.Nodes.size());
		TNtk.Nodes.push_back(n2);
		edge_map[make_pair(n1.id, n2.id)] = 1;
	}
	TransistorNode out = TransistorNode("Output", TNtk.Nodes.size());
	TNtk.Nodes.push_back(out);

	// construct all possible edges
	for (int i = 0; i < TNtk.Nodes.size() - 1; i++) {
		for (int j = i + 1; j < TNtk.Nodes.size(); j++) {
			TransistorNode& n1 = TNtk.Nodes[i], & n2 = TNtk.Nodes[j];
			if (edge_map[make_pair(n1.id, n2.id)]) {
				TransistorEdge e = TransistorEdge(n1.id, n2.id, "t", TNtk.Edges.size());
				TNtk.Edges.push_back(e);
				TNtk.iTransistors.push_back(e.id);
				n1.neighborEdges.push_back(e.id);
				n2.neighborEdges.push_back(e.id);
			}
			else {
				TransistorEdge e = TransistorEdge(n1.id, n2.id, "Null", TNtk.Edges.size());
				TNtk.Edges.push_back(e);
				n1.neighborEdges.push_back(e.id);
				n2.neighborEdges.push_back(e.id);
			}
		}
	}
	return TNtk;
}

TransistorNtk ConstructMultiOutTransistorNtk(int nTransistors, int nOutputs, string Name) {
	TransistorNtk TNtk;
	TNtk.nTransistors = nTransistors;
	// version: 2023/03/06
	TNtk.NtkName = Name;
	map<pair<int, int>, int> edge_map;
	// construct all nodes
	TransistorNode vdd = TransistorNode("VDD", TNtk.Nodes.size());
	TNtk.Nodes.push_back(vdd);
	for (int i = 0; i < nTransistors; i++) {
		TransistorNode n1 = TransistorNode("n" + to_string(2 * i + 1), TNtk.Nodes.size());
		TNtk.Nodes.push_back(n1);
		TransistorNode n2 = TransistorNode("n" + to_string(2 * i + 2), TNtk.Nodes.size());
		TNtk.Nodes.push_back(n2);
		edge_map[make_pair(n1.id, n2.id)] = 1;
	}
	TNtk.nOutputs = nOutputs;
	for (int i = 0; i < nOutputs; i++) {
		TransistorNode out = TransistorNode("Output" + to_string(i), TNtk.Nodes.size());
		TNtk.Nodes.push_back(out);
	}

	// construct all possible edges
	for (int i = 0; i < TNtk.Nodes.size() - 1; i++) {
		for (int j = i + 1; j < TNtk.Nodes.size(); j++) {
			TransistorNode& n1 = TNtk.Nodes[i], & n2 = TNtk.Nodes[j];
			if (!(n1.type == 2 && n2.type == 2)) {
				if (edge_map[make_pair(n1.id, n2.id)]) {
					TransistorEdge e = TransistorEdge(n1.id, n2.id, "t", TNtk.Edges.size());
					TNtk.Edges.push_back(e);
					TNtk.iTransistors.push_back(e.id);
					n1.neighborEdges.push_back(e.id);
					n2.neighborEdges.push_back(e.id);
				}
				else {
					TransistorEdge e = TransistorEdge(n1.id, n2.id, "Null", TNtk.Edges.size());
					TNtk.Edges.push_back(e);
					n1.neighborEdges.push_back(e.id);
					n2.neighborEdges.push_back(e.id);
				}
			}
		}
	}
	return TNtk;
}


TransistorNtk ReadTransistorNtk(string path) {
	ifstream f;
	stringstream ss;
	int nTransistors;
	string tmp;
	int flag;
	TransistorNtk TNtk;
	f.open(path.c_str());
	map<pair<int, int>, int> edge_map;
	while (getline(f, tmp)) {
		if (tmp == "# Transistors") {
			flag = 0;
			continue;
		}
		else if (tmp == "# Edges") {
			flag = 1;
			continue;
		}
		else if (tmp == "# Transistors_Num") {
			flag = 2;
			continue;
		}
		if (!flag) { // Transistors
			string s; int id1, id2;
			ss << tmp;
			ss >> s >> id1 >> id2;
			ss.clear(); ss.str("");
			TransistorEdge e(id1, id2, s, TNtk.Edges.size());
			TNtk.iTransistors.push_back(e.id);
			TNtk.Nodes[id1].neighborEdges.push_back(e.id);
			TNtk.Nodes[id2].neighborEdges.push_back(e.id);
			TNtk.Edges.push_back(e);
		}
		else if (flag == 1) {// Edges
			string s; int id1, id2;
			ss << tmp;
			ss >> id1;
			while (ss >> id2) {
				if (edge_map[make_pair(int(min(id1, id2)), int(max(id1, id2)))])
					continue;
				edge_map[make_pair(int(min(id1, id2)), int(max(id1, id2)))] = 1;
				TransistorEdge e(int(min(id1, id2)), int(max(id1, id2)), "Null", TNtk.Edges.size());
				TNtk.Edges.push_back(e);
				TNtk.Nodes[id1].neighborEdges.push_back(e.id);
				TNtk.Nodes[id2].neighborEdges.push_back(e.id);
			}
			ss.clear();
			ss.str("");
		}
		else if (flag == 2) {
			ss << tmp;
			ss >> nTransistors;
			ss.clear(); ss.str("");
			TNtk.nTransistors = nTransistors;
			TransistorNode vdd = TransistorNode("VDD", TNtk.Nodes.size());
			TNtk.Nodes.push_back(vdd);
			for (int i = 0; i < nTransistors; i++) {
				TransistorNode n1 = TransistorNode("n" + to_string(2 * i + 1), TNtk.Nodes.size());
				TNtk.Nodes.push_back(n1);
				TransistorNode n2 = TransistorNode("n" + to_string(2 * i + 2), TNtk.Nodes.size());
				TNtk.Nodes.push_back(n2);
			}
			TransistorNode out = TransistorNode("Output", TNtk.Nodes.size());
			TNtk.Nodes.push_back(out);
		}

	}
	f.close();
	return TNtk;
}

// 
void TransistorNtk::RemoveNode(int iNode, string newNtkName) {
	vector<int> deletedEdges = Nodes[iNode].neighborEdges;
	NtkName = newNtkName;
	// un-bind the connection between the to-be-deleted edges and nodes
	for (int i = 0; i < deletedEdges.size(); i++) {
		int iEdge = deletedEdges[i];
		TransistorNode& n1 = Nodes[Edges[iEdge].id1];
		TransistorNode& n2 = Nodes[Edges[iEdge].id2];
		vector<int>::iterator it1 = find(n1.neighborEdges.begin(), n1.neighborEdges.end(), iEdge);
		vector<int>::iterator it2 = find(n2.neighborEdges.begin(), n2.neighborEdges.end(), iEdge);
		n1.neighborEdges.erase(it1); n2.neighborEdges.erase(it2);
	}

	// update all nodes' index after removing Nodes[iNode]
	for (int i = iNode + 1; i < Nodes.size(); i++) {
		Nodes[i].id--;
	}
	for (int i = 0; i < Edges.size(); i++) {
		int id1 = Nodes[Edges[i].id1].id, id2 = Nodes[Edges[i].id2].id;
		Edges[i].id1 = (id1 < id2) ? id1 : id2;
		Edges[i].id2 = (id1 > id2) ? id1 : id2;
	}
	// version: 2023/3/26
	//// version: 2023/3/22
	//// preserve each edge's id before changing
	//for (int i = 0; i < Edges.size(); i++)
	//	Edges[i].TmpId = Edges[i].id;

	// update all edges' index after removing all connected edges of Nodes[iNode]
	for (auto iEdge : deletedEdges)
		for (int i = iEdge + 1; i < Edges.size(); i++)
			Edges[i].id--;
	for (int i = 0; i < Nodes.size(); i++) {
		for (int j = 0; j < Nodes[i].neighborEdges.size(); j++) {
			Nodes[i].neighborEdges[j] = Edges[Nodes[i].neighborEdges[j]].id;
		}
	}
	// update iTransistors
	for (auto& tid : iTransistors)
		tid = Edges[tid].id;
	// remove node and connected edges
	Nodes.erase(Nodes.begin() + iNode);
	int count = 0;
	for (int i = 0; i < Edges.size(); i++) {
		if (find(deletedEdges.begin(), deletedEdges.end(), count) != deletedEdges.end()) {
			Edges.erase(Edges.begin() + i);
			i--;
		}
		count++;
	}
}

void TransistorNtk::DrawTransistorNtk() {
	Graph graph(NtkName, false);
	ofstream f1;
	f1.open(NtkName + ".dot");
	auto& registry = graph.GetRegistry();
	for (int i = 0; i < Nodes.size(); i++) {
		auto n = registry.RegisterNode("n" + to_string(i));
		registry.RegisterNodeAttr(n, Attrs_shape, "circle");
		graph.AddNode(n);
	}
	for (int i = 0; i < iTransistors.size(); i++) {
		TransistorEdge e = Edges[iTransistors[i]];
		auto n = registry.RegisterNode("t" + to_string(i) + "_e" + to_string(iTransistors[i]));
		registry.RegisterNodeAttr(n, Attrs_shape, "polygon");
		graph.AddNode(n);
		size_t eid = registry.RegisterEdge(n, e.id1);
		graph.AddEdge(eid);
		eid = registry.RegisterEdge(n, e.id2);
		graph.AddEdge(eid);	
	}
	for (int i = 0; i < Edges.size(); i++) {
		TransistorEdge e = Edges[i];
		if (!e.s.val) {
			auto n = registry.RegisterNode("e" + to_string(i));
			registry.RegisterNodeAttr(n, Attrs_shape, "triangle");
			graph.AddNode(n);
			size_t eid = registry.RegisterEdge(n, e.id1);
			graph.AddEdge(eid);
			eid = registry.RegisterEdge(n, e.id2);
			graph.AddEdge(eid);
		}
	}
	f1 << graph.Dump() << endl;
	f1.close();
	system(("dot " + NtkName + ".dot -T pdf -o " + NtkName + ".pdf").c_str());
}
