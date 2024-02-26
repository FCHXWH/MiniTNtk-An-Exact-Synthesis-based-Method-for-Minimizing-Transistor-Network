#include "PathLimitation.h"
#include <queue>
#include <unordered_set>

int CheckPath(vector<int> path, map<pair<int, int>, string> transistors) {
    map<string, int> VarMap;
    for (int i = 0; i < path.size() - 1; i++) {
        pair<int, int> edge = (path[i] > path[i + 1]) ? make_pair(path[i + 1], path[i]) : make_pair(path[i], path[i + 1]);
        if (transistors.count(edge)) {
            string transistor = transistors[edge];
            if (transistor[0] == '-') {
                transistor.erase(transistor.begin());
                VarMap[transistor] |= 2;
            }
            else
                VarMap[transistor] |= 1;
        }

    }
    for (auto item : VarMap) {
        if (item.second == 3)
            return false;
    }
    return true;
}

int CheckPath(vector<int> path, map<int, string> transistors) {
    map<string, int> VarMap;
    for (int i = 0; i < path.size(); i++) {
        if (transistors.count(path[i])) {
            string transistor = transistors[path[i]];
            if (transistor[0] == '!') {
                transistor.erase(transistor.begin());
                VarMap[transistor] |= 2;
            }
            else
                VarMap[transistor] |= 1;
        }

    }
    for (auto item : VarMap) {
        if (item.second == 3)
            return false;
    }
    return true;
}
vector<pair<pair<int, int>, string>> bfs(vector<vector<int>>& graph, int start_node, map<int, string> transistors)
 {
    queue<int> q;
    unordered_set<int> visited;
    vector<int> BfsNodes;
    vector<pair<pair<int, int>, string>> transistor_pairs;
    q.push(start_node);
    visited.insert(start_node);

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        BfsNodes.push_back(node);
        // cout << node << " "; // Process the node (in this case, we print it)
        for (int neighbor : graph[node]) {
            if (visited.find(neighbor) == visited.end()) {
                q.push(neighbor);
                visited.insert(neighbor);
            }
        }
    }
    for (int i = 0; i < BfsNodes.size(); i++) {
        if (transistors.count(BfsNodes[i]))
            transistor_pairs.push_back(make_pair(make_pair(BfsNodes[i - 1], BfsNodes[i + 1]), transistors[BfsNodes[i]]));
    }
    return transistor_pairs;
}

int CountPathTransistors(vector<int> path, map<pair<int, int>, string> transistors) {
    int counter = 0;
    for (int i = 0; i < path.size() - 1; i++) {
        pair<int, int> edge = (path[i] > path[i + 1]) ? make_pair(path[i + 1], path[i]) : make_pair(path[i], path[i + 1]);
        counter += transistors.count(edge);
    }
    return counter;
}

vector<int> GetPathInternalEdges(vector<int> path, map<pair<int, int>, int> AllEdges) {
    vector<int> InternalEdges;
    for (int i = 0; i < path.size() - 1; i++) {
        pair<int, int> edge = (path[i] > path[i + 1]) ? make_pair(path[i + 1], path[i]) : make_pair(path[i], path[i + 1]);
        if (AllEdges.count(edge)) {
            InternalEdges.push_back(AllEdges[edge]);
        }
    }
    return InternalEdges;
}

vector<int> GetPathTransistorEdges(vector<int> path, map<pair<int, int>, int> Transistor_cnfvars) {
    vector<int> TransistorEdges;
    for (int i = 0; i < path.size() - 1; i++) {
        pair<int, int> edge = (path[i] > path[i + 1]) ? make_pair(path[i + 1], path[i]) : make_pair(path[i], path[i + 1]);
        if (Transistor_cnfvars.count(edge)) {
            TransistorEdges.push_back(Transistor_cnfvars[edge]);
        }
    }
    return TransistorEdges;
}



// Recursive function to find all simple paths in the graph
void findAllPaths(vector<vector<int>>& graph, vector<bool>& visited, int start, int end, vector<int>& path, vector<vector<int>>& paths,
    map<pair<int, int>, string> transistors) {
    visited[start] = true;
    path.push_back(start);

    // If the end node is reached, add the current path to the result
    if (start == end) {
        if (CheckPath(path, transistors))
            paths.push_back(path);
    }
    else {
        // Recur for all adjacent vertices
        for (auto i : graph[start]) {
            if (!visited[i]) {
                findAllPaths(graph, visited, i, end, path, paths, transistors);
            }
        }
    }

    // Backtrack and remove the current node from the path
    visited[start] = false;
    path.pop_back();
}

void findAllPaths(vector<vector<int>>& graph, vector<bool>& visited, int start, int end, vector<int>& path, vector<vector<string>>& paths,
    map<int, string> transistors) {
    visited[start] = true;
    path.push_back(start);

    // If the end node is reached, add the current path to the result
    if (start == end) {
        if (CheckPath(path, transistors)) {
            vector<string> spath;
            for (auto node : path)
                if (transistors.count(node))
                    spath.push_back(transistors[node]);
            paths.push_back(spath);
        }
            
    }
    else {
        // Recur for all adjacent vertices
        for (auto i : graph[start]) {
            if (!visited[i]) {
                findAllPaths(graph, visited, i, end, path, paths, transistors);
            }
        }
    }

    // Backtrack and remove the current node from the path
    visited[start] = false;
    path.pop_back();
}

void findAllPathIDs(vector<vector<int>>& graph, vector<bool>& visited, int start, int end, vector<int>& path, vector<vector<int>>& paths,
    map<int, string> transistors) {
    visited[start] = true;
    path.push_back(start);

    // If the end node is reached, add the current path to the result
    if (start == end) {
        if (CheckPath(path, transistors)) {
            vector<int> TransistorPathID;
            for (auto node : path)
                if (transistors.count(node))
                    TransistorPathID.push_back(node);
            paths.push_back(TransistorPathID);
        }

    }
    else {
        // Recur for all adjacent vertices
        for (auto i : graph[start]) {
            if (!visited[i]) {
                findAllPathIDs(graph, visited, i, end, path, paths, transistors);
            }
        }
    }

    // Backtrack and remove the current node from the path
    visited[start] = false;
    path.pop_back();
}

// Function to find all simple paths between two nodes in a graph
vector<vector<int>> getSimplePaths(vector<vector<int>>& graph, int start, int end, map<pair<int, int>, string> transistors) {
    vector<bool> visited(graph.size(), false);
    vector<int> path;
    vector<vector<int>> paths;

    // Find all paths using DFS
    findAllPaths(graph, visited, start, end, path, paths, transistors);

    return paths;
}

vector<vector<string>> getSimpleTransistorPaths(vector<vector<int>>& graph, int start, int end, map<int, string>& transistors) {
    vector<bool> visited(graph.size(), false);
    vector<int> path;
    vector<vector<string>> paths;

    // Find all paths using DFS
    findAllPaths(graph, visited, start, end, path, paths, transistors);

    return paths;
}

vector<vector<int>> getSimpleTransistorIDPaths(vector<vector<int>>& graph, int start, int end, map<int, string>& transistors) {
    vector<bool> visited(graph.size(), false);
    vector<int> path;
    vector<vector<int>> paths;

    // Find all paths using DFS
    findAllPathIDs(graph, visited, start, end, path, paths, transistors);

    return paths;
}



void getLongerKPaths(vector<vector<int>>& paths, vector<vector<int>>& Kpaths, int K, map<pair<int, int>, string>& transistors) {
    for (auto path : paths)
        if (CountPathTransistors(path, transistors) > K)
            Kpaths.push_back(path);
}

vector<vector<int>> IncrementalAddLimitedPathConstraints(map<pair<int, int>, int> AllEdges, map<pair<int, int>, string> transistors, map<pair<int, int>, int>& transistors_cnfvar, int nNodes, vector<int> Ks, vector<int> OutClusters) {
    vector<vector<int>> LimitedPathClauses;
    vector<vector<int>> graph(nNodes);
    for (auto Edge : AllEdges) {
        graph[Edge.first.first].push_back(Edge.first.second);
        graph[Edge.first.second].push_back(Edge.first.first);
    }
    /*for (int i = 0; i < AllEdges.size(); i++) {
        graph[AllEdges[i].first].push_back(AllEdges[i].second);
        graph[AllEdges[i].second].push_back(AllEdges[i].first);
    }*/
    for (auto item : transistors) {
        graph[item.first.first].push_back(item.first.second);
        graph[item.first.second].push_back(item.first.first);
    }

    // Find all simple paths between two nodes
    int i = 0;
    vector<int> transistorClause;
    for (auto item : transistors_cnfvar)
        transistorClause.push_back(-item.second);
    for (auto id : OutClusters) {
        vector<vector<int>> paths = getSimplePaths(graph, 0, id, transistors);
        for (auto path : paths) {
            if (CountPathTransistors(path, transistors) > Ks[i]) {
                vector<int> InternalEdges = GetPathInternalEdges(path, AllEdges);
                // vector<int> TransistorEdges = GetPathTransistorEdges(path, transistors_cnfvar);
                vector<int> clause;
                /*for (auto eid : TransistorEdges)
                    LimitedPathClauses.push_back({ eid });*/
                for (auto cnfvar : transistorClause)
                    clause.push_back(cnfvar);
                for (auto eid : InternalEdges)
                    clause.push_back(-eid);
                for (auto literal : clause)
                    cout << literal << " ";
                cout << "(";
                for (int j = 0; j < path.size() - 1; j++) {
                    pair<int, int> edge = (path[j] > path[j + 1]) ? make_pair(path[j + 1], path[j]) : make_pair(path[j], path[j + 1]);
                    if (transistors.count(edge)) {
                        cout << transistors[edge] << " ";
                    }
                }
                cout << ")";
                cout << endl;
                LimitedPathClauses.push_back(clause);

            }
            
        }
        
        i++;
    }
    return LimitedPathClauses;
}