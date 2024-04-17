#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

const int numNodes = 4;

// A function to go through the graph and find the shortest path using multiple iterations of the naive brute force approach.
// Inspiration from GeeksForGeeks.com
int findMinimumPath(int myGraph[numNodes][numNodes], int startingNode)
{
    // Other than the node that we are starting with, put all the other nodes into a vector nodeBank.
    vector<int> nodeBank;
    for (int i = 0; i < numNodes; i++)
    {
        if (i != startingNode)
        {
            nodeBank.push_back(i);
        }
    }

    // Make our minimum path the largest integer we can have. This will reduce the liklihood of having a path greater than the initial minimumPath variable, which would screw up the algorithm.
    int minimumPath = INT_MAX;

    // Go through one combination of paths to see if it is smaller than the previous minimumPath.
    do
    {
        // Start fresh with a new totalEdgeWeight value.
        int totalEdgeWeight = 0;

        // Starting with the first node (the startingNode), which corresponds to a row in our graph matrix, go through every node in our nodeBank. The nodeBank holds each node OTHER THAN the starting node.
        int row = startingNode;
        for (int i = 0; i < nodeBank.size(); i++)
        {
            // To totalEdgeWeight, add the value of the current row's (current node's) first connection to another node. This may not necessarily be the nearest neighbor to the current node. We are simply trying all combinations of nodes in a brute force manner. At the end of this iteration, we will find a new permutation of the nodeBank which will cause us to try different node paths on the next iteration.
            totalEdgeWeight += myGraph[row][nodeBank[i]];
            // Our new row will be the row to which we traveled.
            row = nodeBank[i];
        }
        // After going through each node in our path, find the weight of the edge between the last node we landed on to the original startingNode.
        totalEdgeWeight += myGraph[row][startingNode];

        // If the path we just forged is less than any previously recorded minimum path, then make that the new minimum path.
        if (totalEdgeWeight < minimumPath)
        {
            minimumPath = totalEdgeWeight;
        }
    } while (next_permutation(nodeBank.begin(), nodeBank.end())); //Keep iterating through this as long as we can keep finding new permutations of nodeBank. When we no longer have any viable permutations, it returns false and stops the algorithm because we have searched all possible paths. O(n!)

    return minimumPath;
}

int main()
{
    int myGraph[numNodes][numNodes] = {{0, 10, 15, 20},
                                       {10, 0, 35, 25},
                                       {15, 35, 0, 30},
                                       {20, 25, 30, 0}};
    int startingNode = 0;
    cout << findMinimumPath(myGraph, startingNode) << endl;
    return 0;
}