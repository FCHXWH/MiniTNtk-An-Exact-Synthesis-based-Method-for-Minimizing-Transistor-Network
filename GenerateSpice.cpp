#include "GenerateSpice.h"
#include <queue>
#include <string>
#include <unordered_set>

void GetTransistorDirections(vector<vector<int>>& graph, vector<bool>& visited, int start, int end, vector<int>& path, set<pair<int, int>>& TransistorDirections, 
    map<int, string> transistors) {
    visited[start] = true;
    path.push_back(start);

    // If the end node is reached, add the current path to the result
    if (start == end) {
        if (CheckPath(path, transistors)) {
            for (int i = 0; i < path.size(); i++)
                if (transistors.count(path[i]))
                    TransistorDirections.insert(make_pair(path[i - 1], path[i + 1]));
        }
    }
    else {
        // Recur for all adjacent vertices
        for (auto i : graph[start]) {
            if (!visited[i]) {
                GetTransistorDirections(graph, visited, i, end, path, TransistorDirections, transistors);
            }
        }
    }

    // Backtrack and remove the current node from the path
    visited[start] = false;
    path.pop_back();
}

vector<transistor> transformTransistors(int nClusters, int InputClusterID, int OutClusterID, vector<pair<pair<int, int>, string>> transistors_pairs, int mos) {
	map<int, string> transistorNodes;
	vector<vector<int>> graph(nClusters + transistors_pairs.size());
    // generate all nodes' names
    vector<string> NodeNames(nClusters, "");
    for (int i = 0; i < nClusters; i++) {
        if (i == InputClusterID) {
            if (mos)
                NodeNames[i] = "VCC";
            else
                NodeNames[i] = "GND";
        }
        else if (i == OutClusterID)
            NodeNames[i] = "Y";
        else
            NodeNames[i] = ((mos) ? "pN" : "nN") + to_string(i);
    }

	for (int i = 0; i < transistors_pairs.size(); i++) {
		pair<pair<int, int>, string> Edge = transistors_pairs[i];
		transistorNodes[nClusters + i] = Edge.second;
		graph[Edge.first.first].push_back(nClusters + i);
		graph[nClusters + i].push_back(Edge.first.first);
		graph[Edge.first.second].push_back(nClusters + i);
		graph[nClusters + i].push_back(Edge.first.second);
	}
    set<pair<int, int>> TransistorDirections;
    vector<bool> visited(graph.size(), false);
    vector<int> path;
    vector<vector<string>> paths;
    vector<transistor> transistors;
    GetTransistorDirections(graph, visited, InputClusterID, OutClusterID, path, TransistorDirections, transistorNodes);
    for (auto t : transistors_pairs) {
        pair<int, int> transistor_ids = t.first;
        string Gate = t.second;
        transistor T;
        if (TransistorDirections.count(transistor_ids)) {
            T.Source = NodeNames[transistor_ids.first];
            T.Drain = NodeNames[transistor_ids.second];
            T.Mos = (mos) ? "PMOS" : "NMOS";
            T.Substrate = (mos) ? "VCC" : "GND";
            // T.Gate += (Gate[0] == '-') ? Gate[1] : Gate[0];
            T.Gate = Gate;
        }
        else if (TransistorDirections.count(make_pair(transistor_ids.second, transistor_ids.first))) {
            T.Source = NodeNames[transistor_ids.second];
            T.Drain = NodeNames[transistor_ids.first];
            T.Mos = (mos) ? "PMOS" : "NMOS";
            T.Substrate = (mos) ? "VCC" : "GND";
            // T.Gate += (Gate[0] == '-') ? Gate[1] : Gate[0];
            T.Gate = Gate;
        }
        transistors.push_back(T);
        /*if (T.Gate[0] == '-') {
            transistor INVPT,INVNT;
            INVPT.Source = "VCC"; INVPT.Drain = Gate; INVPT.Gate += Gate[1]; INVPT.Mos = "PMOS"; INVPT.Substrate = "VCC";
            INVPT.Source = Gate; INVPT.Drain = "GND"; INVPT.Gate += Gate[1]; INVPT.Mos = "NMOS"; INVPT.Substrate = "GND";
            transistors.push_back(INVPT);
            transistors.push_back(INVNT);
        }*/
    }
    return transistors;
}

vector<transistor> transformTransistors(int nClusters, int InputClusterID, int OutClusterID, vector<pair<pair<int, int>, string>> transistors_pairs, map<pair<int, int>, int> transistors_Counts, int mos, int OutINV = 0) {
    map<int, string> transistorNodes;
    vector<vector<int>> graph(nClusters + transistors_pairs.size());
    // generate all nodes' names
    vector<string> NodeNames(nClusters, "");
    for (int i = 0; i < nClusters; i++) {
        if (i == InputClusterID) {
            if (mos)
                NodeNames[i] = "VCC";
            else
                NodeNames[i] = "GND";
        }
        else if (i == OutClusterID) {
            if (OutINV)
                NodeNames[i] = "INVY";
            else
                NodeNames[i] = "Y";
        }
        else
            NodeNames[i] = ((mos) ? "pN" : "nN") + to_string(i);
    }

    for (int i = 0; i < transistors_pairs.size(); i++) {
        pair<pair<int, int>, string> Edge = transistors_pairs[i];
        transistorNodes[nClusters + i] = Edge.second;
        graph[Edge.first.first].push_back(nClusters + i);
        graph[nClusters + i].push_back(Edge.first.first);
        graph[Edge.first.second].push_back(nClusters + i);
        graph[nClusters + i].push_back(Edge.first.second);
    }
    set<pair<int, int>> TransistorDirections;
    vector<bool> visited(graph.size(), false);
    vector<int> path;
    vector<vector<string>> paths;
    vector<transistor> transistors;
    GetTransistorDirections(graph, visited, InputClusterID, OutClusterID, path, TransistorDirections, transistorNodes);
    for (auto t : transistors_pairs) {
        pair<int, int> transistor_ids = t.first;
        string Gate = t.second;
        transistor T;
        if (TransistorDirections.count(transistor_ids)) {
            T.Source = NodeNames[transistor_ids.first];
            T.Drain = NodeNames[transistor_ids.second];
            T.Mos = (mos) ? "PMOS" : "NMOS";
            T.Substrate = (mos) ? "VCC" : "GND";
            // T.Gate += (Gate[0] == '-') ? Gate[1] : Gate[0];
            T.Gate = Gate;
        }
        else if (TransistorDirections.count(make_pair(transistor_ids.second, transistor_ids.first))) {
            T.Source = NodeNames[transistor_ids.second];
            T.Drain = NodeNames[transistor_ids.first];
            T.Mos = (mos) ? "PMOS" : "NMOS";
            T.Substrate = (mos) ? "VCC" : "GND";
            // T.Gate += (Gate[0] == '-') ? Gate[1] : Gate[0];
            T.Gate = Gate;
        }
        if (mos) {
            T.W = T.lambda * transistors_Counts[transistor_ids] * T.basicW;
            T.nFin = T.lambda * transistors_Counts[transistor_ids];
        }
        else {
            T.W = transistors_Counts[transistor_ids] * T.basicW;
            T.nFin = transistors_Counts[transistor_ids];
        }

        transistors.push_back(T);
        /*if (T.Gate[0] == '-') {
            transistor INVPT,INVNT;
            INVPT.Source = "VCC"; INVPT.Drain = Gate; INVPT.Gate += Gate[1]; INVPT.Mos = "PMOS"; INVPT.Substrate = "VCC";
            INVPT.Source = Gate; INVPT.Drain = "GND"; INVPT.Gate += Gate[1]; INVPT.Mos = "NMOS"; INVPT.Substrate = "GND";
            transistors.push_back(INVPT);
            transistors.push_back(INVNT);
        }*/
    }
    return transistors;
}

void TransistorsReorder(vector<transistor>& Transistors, int mos) {
    for (int i = 0; i < Transistors.size();i++) {
        transistor T = Transistors[i];
        if (T.Source == "Y" | T.Drain == "Y") {
            if (mos)
                swap(Transistors[0], Transistors[i]);
            else
                swap(Transistors[Transistors.size() - 1], Transistors[i]);
        }
    }
}

void writeSpice(string file, string CircktName, set<string> AllLiterals, vector<transistor> transistorsPUN, vector<transistor> transistorsPDN) {
    set<string> PinNames;
    PinNames.insert("VCC");
    PinNames.insert("GND");
    PinNames.insert("Y");
    for (auto literal : AllLiterals) {
        if (literal[0] == '!')
            PinNames.insert(literal.substr(1, 1));
        else
            PinNames.insert(literal);
    }
    ofstream f;
    f.open(file.c_str());
    f << ".subckt " << CircktName << " ";
    for (auto Pin : PinNames)
        f << Pin << " ";
    f << endl;
    int i = 0;
    TransistorsReorder(transistorsPUN, 1);
    TransistorsReorder(transistorsPDN, 0);
    for (auto t : transistorsPUN) {
       // f << "M" + to_string(i) << " " << t.Drain << " " << t.Gate << " " << t.Source << " " << t.Substrate << " " << t.Mos << " " << "W=" << t.W << "n " << "L=" << t.L << "n " << "nfin=" << t.nFin << endl;
        // f << "+ ad=0p pd=0u as=0p ps=0u" << endl;
        // 45 nm
        f << "M" + to_string(i) << " " << t.Drain << " " << t.Gate << " " << t.Source << " " << t.Substrate << " " << t.Mos << " " << "W=" << t.W << "u " << "L=" << t.L << "u " << endl;
        f << "+ ad=0p pd=0u as=0p ps=0u" << endl;
        i++;
    }
    for (auto t : transistorsPDN) {
        // f << "M" + to_string(i) << " " << t.Drain << " " << t.Gate << " " << t.Source << " " << t.Substrate << " " << t.Mos << " " << "W=" << t.W << "n " << "L=" << t.L << "n " << "nfin=" << t.nFin << endl;
        // f << "+ ad=0p pd=0u as=0p ps=0u" << endl;
        f << "M" + to_string(i) << " " << t.Drain << " " << t.Gate << " " << t.Source << " " << t.Substrate << " " << t.Mos << " " << "W=" << t.W << "u " << "L=" << t.L << "u " << endl;
        f << "+ ad=0p pd=0u as=0p ps=0u" << endl;
        i++;
    }
    f << ".ends " << CircktName << endl;
    f.close();
}

void GenerateTransistors(set<string> AllLiterals, vector<transistor>& transistorsPUN, vector<transistor>& transistorsPDN) {
    for (auto literal : AllLiterals) {
        if (literal[0] == '!') {
            transistor INVPT, INVNT;
            INVPT.Source = "VCC"; INVPT.Drain = literal; INVPT.Gate += literal[1]; INVPT.Mos = "PMOS"; INVPT.Substrate = "VCC";
            INVNT.Source = "GND"; INVNT.Drain = literal; INVNT.Gate += literal[1]; INVNT.Mos = "NMOS"; INVNT.Substrate = "GND";
            transistorsPUN.push_back(INVPT);
            transistorsPDN.push_back(INVNT);
        }
    }
    
}

void GenerateTransistors(set<string> AllLiterals, vector<transistor>& transistorsPUN, vector<transistor>& transistorsPDN, int OutINV) {
    for (auto literal : AllLiterals) {
        if (literal[0] == '!') {
            transistor INVPT, INVNT;
            INVPT.Source = "VCC"; INVPT.Drain = literal; INVPT.Gate += literal[1]; INVPT.Mos = "PMOS"; INVPT.Substrate = "VCC"; INVPT.W = INVPT.basicW * INVPT.lambda; INVPT.nFin = INVPT.lambda;
            INVNT.Source = "GND"; INVNT.Drain = literal; INVNT.Gate += literal[1]; INVNT.Mos = "NMOS"; INVNT.Substrate = "GND"; INVNT.W = INVNT.basicW; INVNT.nFin = 1;
            transistorsPUN.push_back(INVPT);
            transistorsPDN.push_back(INVNT);
        }
    }

    if (OutINV) {
        transistor INVOUTPT, INVOUTNT;
        INVOUTPT.Source = "VCC"; INVOUTPT.Drain = "Y"; INVOUTPT.Gate = "INVY"; INVOUTPT.Mos = "PMOS"; INVOUTPT.Substrate = "VCC"; INVOUTPT.W = INVOUTPT.basicW * INVOUTPT.lambda; INVOUTPT.nFin = INVOUTPT.lambda;
        INVOUTNT.Source = "GND"; INVOUTNT.Drain = "Y"; INVOUTNT.Gate = "INVY"; INVOUTNT.Mos = "NMOS"; INVOUTNT.Substrate = "GND"; INVOUTNT.W = INVOUTPT.basicW; INVOUTNT.nFin = 1;
        transistorsPUN.push_back(INVOUTPT);
        transistorsPDN.push_back(INVOUTNT);
    }
}