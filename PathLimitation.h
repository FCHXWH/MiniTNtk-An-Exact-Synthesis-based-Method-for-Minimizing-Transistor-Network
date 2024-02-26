#pragma once
#ifndef __PATH_LIMITATION_H__
#define __PATH_LIMITATION_H__
#include <vector>
#include <map>
#include <iostream>
using namespace std;

int CheckPath(vector<int> path, map<pair<int, int>, string> transistors);
int CheckPath(vector<int> path, map<int, string> transistors);
vector<int> GetPathInternalEdges(vector<int> path, map<pair<int, int>, int> AllEdges);
// Recursive function to find all simple paths in the graph
void findAllPaths(vector<vector<int>>& graph, vector<bool>& visited, int start, int end, vector<int>& path, vector<vector<int>>& paths,
    map<pair<int, int>, string> transistors);
void findAllPaths(vector<vector<int>>& graph, vector<bool>& visited, int start, int end, vector<int>& path, vector<vector<string>>& paths,
    map<int, string> transistors);
// Function to find all simple paths between two nodes in a graph
vector<vector<int>> getSimplePaths(vector<vector<int>>& graph, int start, int end, map<pair<int, int>, string> transistors);
vector<vector<string>> getSimpleTransistorPaths(vector<vector<int>>& graph, int start, int end, map<int, string>& transistors);
void getLongerKPaths(vector<vector<int>>& paths, vector<vector<int>>& Kpaths, int K, map<pair<int, int>, string>& transistors);
vector<vector<int>> IncrementalAddLimitedPathConstraints(map<pair<int, int>, int> AllEdges, map<pair<int, int>, string> transistors, map<pair<int, int>, int>& transistors_cnfvar, int nNodes, vector<int> Ks, vector<int> OutClusters);
vector<pair<pair<int, int>, string>> bfs(vector<vector<int>>& graph, int start_node, map<int, string> transistors);
vector<vector<int>> getSimpleTransistorIDPaths(vector<vector<int>>& graph, int start, int end, map<int, string>& transistors);

#endif // !__PATH_LIMITATION_H__

