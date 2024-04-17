// Traveling Salesman Problem Approximation Algorithm, by Wesley Junkins.
// This algorithm takes as input the method to use (original, brute, nearest, check), the input graph, and optionally, the graph to check.
// Original mode uses my original algorithm, which will be described below.
// Brute mode runs the brute force algorithm for finding an exact answer to TSP.
// Nearest mode runs the nearest neighbor algorithm to find an approximation for TSP.

// Original algorithm description:
/*
    Say you have a fully connected non-directional graph. You input this graph as a file of edge weights. Each row and column corresponds to a node-connection.
    This algorithm reads-in the graph file and builds a graph class containing nodes, connections, and weights.
    After reading-in the file and determining the overall structure (arbitrarily) of the graph, all the edge weights are sorted.
    Imagine an empty graph. We first start with the smallest edge-weight in our sorted list. We find the nodes that are connected by that edge weight and make that connection.
    Then, we move on to the next smallest edge weight.
    We continue this process, but we must follow some rules before adding a connection to the graph.
    Each node has a node type: untouched, leader, inside. The state of the node, along with its group number, determine if a connection will be made.
    Say the next smallest edge weight in the list is X. So, we find the nodes that X would connect and look at their type and group.
    Cases:
        Both nodes are untouched:
            Make the connection and label it with a new group number. The untouched nodes are now leader nodes.
        Both nodes are leaders:
            Check if they are in the same group. If they are, do nothing. If not, make the connection and set the group of all the nodes connected to the two leader nodes to be the lower of the two group numbers. The leader nodes are now inside nodes.
        One is a leader and one is untouched:
            Make the connection. The untouched becomes a leader and the leader becomes an insider. The untouched node receives the group number of the leader's group.

        There are other cases, but we do nothing in those scenarios.

    When we have exhausted the edge weights, we are done constructing the graph. We can then retrace our steps to print what path we took.
    The output is printed to the console as well as a file. The total distance is printed to the console and will appear in the filename.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

int globalGroupNumber = 1;
int globalTotalWeight = 0;

class node
{
public:
    int nodeName;    // The integer name of a node, starting with 0.
    int nodeType;    // The type of node it is: 0 = untouched, 1 = leader, 2 = inside.
    int nodeGroup;   // The group that the node is in. Starts with 0, but changes to the global group number as soon as it is touched.
    bool wasTouched; // Only used in nearest neighbor algorithm.

    // Default constructor
    node(int nodeName)
    {
        this->nodeName = nodeName;
        this->nodeType = 0;
        this->nodeGroup = 0;
        this->wasTouched = false;
    };
};

class connectedNode
{
public:
    node *connectedLeftNode;
    node *connectedRightNode;
    bool checked;

    // Default constructor
    connectedNode(node *connectedLeftNode, node *connectedRightNode)
    {
        this->connectedLeftNode = connectedLeftNode;
        this->connectedRightNode = connectedRightNode;
        this->checked = false;
    }
};

class weight
{
public:
    int value;
    int fromNode;
    int toNode;

    // Default constructor.
    weight(int value, int fromNode, int toNode)
    {
        this->value = value;
        this->fromNode = fromNode;
        this->toNode = toNode;
    };
};

class graph
{
public:
    int numNodes;

    // The Adjacency Matrix.
    vector<vector<int>> matrix;

    // The vector of weights to sort.
    vector<weight *> weights;

    // The vector of nodes. This will be useful in keeping up with the current state of each node (e.g. which group they are in.).
    vector<node *> nodes;

    // The vector of node connectioins we make. This will be useful when we need to retrace our path.
    vector<connectedNode *> connectedNodes;

    // The vector of integers showing what path we took.
    vector<int> pathTaken;

    // Add a weight value to the weight matrix. Also add it to the weight vector and specify which nodes it connects (which corresponds to the row and column it lies in the matrix).
    void addWeight(int row, int item)
    {
        if (matrix.size() <= row)
        {
            matrix.resize(row + 1); // Resize the matrix to accommodate the new index.
        };
        matrix.at(row).push_back(item);

        // Add the weight (item) to the weights vector. If the weight is 0, do not add it, this will mean nothing; this assumes that no weights, other than ones that connect the same node, will be 0. Also specify which nodes the weight is connecting.
        if (item != 0)
        {
            weight *newWeight = new weight(item, row, matrix.at(row).size() - 1);
            this->weights.push_back(newWeight);
        };
    };

    // Prints the weight-matrix that was read-in from the file.
    void printMatrix()
    {
        for (int i = 0; i < this->matrix.size(); i++)
        {
            for (int j = 0; j < this->matrix.at(i).size(); j++)
            {
                cout << this->matrix.at(i).at(j) << "\t";
            };
            cout << endl;
        };
    };

    // Looks-up the distance from a node to a node.
    double distance(int from, int to)
    {
        if (from < to)
        {
            return this->matrix.at(to).at(from);
        }
        return this->matrix.at(from).at(to);
    };

    // A one-time function that sorts the edge-weights.
    void sortWeghts()
    {
        sort(weights.begin(), weights.end(), [](const weight *a, const weight *b)
             { return a->value < b->value; });
    };
};

int main(int argc, char *argv[])
{
    // Decide what to do.
    if (strcmp(argv[1], "original") == 0)
    {
        cout << "Running ORIGINAL algorithm" << endl;
    }
    else if (strcmp(argv[1], "nearest") == 0)
    {
        // Run nearest neighbor.
        // Read-in the file.
        cout << "Running NEAREST NEIGHBOR algorithm" << endl;
        ifstream file(argv[2]);
        if (!file.is_open())
        {
            cerr << "Failed to open file" << endl;
            return 1;
        };
        graph *myGraph = new graph();
        string textLine;
        int index = 0;
        while (getline(file, textLine))
        {
            istringstream iss(textLine);
            int weightToAdd;
            while (iss >> weightToAdd)
            {
                myGraph->addWeight(index, weightToAdd);
            };
            node *newNode = new node(index);
            myGraph->nodes.push_back(newNode);
            index++;
        };
        file.close();
        myGraph->numNodes = index;

        // Starting with node 0, perform nearest neighbor.
        // Cycle through each node and its connected nodes to find the shortest path. Then move to that node.
        int currentNodeIndex = 0;
        int smallestNodeIndex;
        int totalWeight = 0;
        int smallestWeight;
        node *currentNode;
        currentNode = myGraph->nodes.at(currentNodeIndex);
        currentNode->wasTouched = true;
        for (int x = 0; x < myGraph->nodes.size() - 1; x++)
        {
            smallestWeight = INT_MAX;
            for (int i = 0; i < myGraph->nodes.size(); i++)
            {
                if ((currentNodeIndex != i) && (myGraph->nodes.at(i)->wasTouched == false))
                {
                    if (myGraph->distance(currentNodeIndex, i) < smallestWeight)
                    {
                        smallestWeight = myGraph->distance(currentNodeIndex, i);
                        smallestNodeIndex = i;
                    };
                };
            };
            cout << currentNode->nodeName << "---" << myGraph->distance(currentNodeIndex, smallestNodeIndex) << "-->" << myGraph->nodes.at(smallestNodeIndex)->nodeName << endl;
            myGraph->pathTaken.push_back(currentNodeIndex);
            totalWeight += smallestWeight;
            currentNode = myGraph->nodes.at(smallestNodeIndex);
            currentNodeIndex = smallestNodeIndex;
            currentNode->wasTouched = true;
        };

        // Add the distance from the starting node to the ending node.
        cout << currentNode->nodeName << "---" << myGraph->distance(currentNodeIndex, 0) << "-->" << myGraph->nodes.at(0)->nodeName << endl;
        myGraph->pathTaken.push_back(currentNodeIndex);
        totalWeight += myGraph->distance(currentNodeIndex, 0);

        // Write the output to a file
        cout << "Writing path to file" << endl;
        string fileName = "S[NEAREST]" + to_string(totalWeight) + "_wcjunkins.sol";
        ofstream outFile(fileName);
        if (!outFile.is_open())
        {
            cerr << "Failed to open the file for writing" << endl;
            return 1;
        };
        for (int i = 0; i < myGraph->pathTaken.size(); i++)
        {
            outFile << myGraph->pathTaken.at(i) << " ";
        };
        outFile << myGraph->pathTaken.at(0) << " ";
        outFile.close();

        cout << "Total Distance: " << totalWeight << endl;

        cout << "The shortest path has been successfully generated" << endl
             << "A copy of the complete path has been saved to the file " << fileName << endl
             << "Closing program..." << endl;

        return 0;
    }
    else if (strcmp(argv[1], "brute") == 0)
    {
        // Run brute force. Inspiration was taken from GeeksforGeeks.com.
        cout << "Running BRUTE FORCE algorithm" << endl;

        // Read-in the file.
        ifstream file(argv[2]);
        if (!file.is_open())
        {
            cerr << "Failed to open file" << endl;
            return 1;
        };
        graph *myGraph = new graph();
        string textLine;
        int index = 0;
        while (getline(file, textLine))
        {
            istringstream iss(textLine);
            int weightToAdd;
            while (iss >> weightToAdd)
            {
                myGraph->addWeight(index, weightToAdd);
            };
            node *newNode = new node(index);
            myGraph->nodes.push_back(newNode);
            index++;
        };
        file.close();
        myGraph->numNodes = index;

        vector<int> nodeIndex(myGraph->numNodes);
        vector<int> pathTaken(myGraph->numNodes);
        for (int i = 0; i < myGraph->numNodes; i++)
        {
            nodeIndex.at(i) = i;
        };

        int shortestDistance = INT_MAX;
        do
        {
            int currentPathDistance = 0;
            for (int i = 0; i < myGraph->numNodes - 1; i++)
            {
                currentPathDistance += myGraph->distance(nodeIndex.at(i), nodeIndex.at(i + 1));
            };
            currentPathDistance += myGraph->distance(nodeIndex.back(), nodeIndex.at(0));
            if (currentPathDistance < shortestDistance)
            {
                shortestDistance = currentPathDistance;
                pathTaken = nodeIndex;
            }
        } while (next_permutation(nodeIndex.begin() + 1, nodeIndex.end()));

        //  Write the output to a file
        cout << "Writing path to file" << endl;
        string fileName = "S[BRUTE]" + to_string(shortestDistance) + "_wcjunkins.sol";
        ofstream outFile(fileName);
        if (!outFile.is_open())
        {
            cerr << "Failed to open the file for writing" << endl;
            return 1;
        };
        for (int i = 0; i < pathTaken.size(); i++)
        {
            outFile << pathTaken.at(i) << " ";
        };
        outFile << pathTaken.at(0) << " ";
        outFile.close();

        cout << "Total Distance: " << shortestDistance << endl;

        cout << "The shortest path has been successfully generated" << endl
             << "A copy of the complete path has been saved to the file " << fileName << endl
             << "Closing program..." << endl;

        return 0;
    }
    else if (strcmp(argv[1], "check") == 0)
    {
        cout << "Checking the total distance of the path in the provided file" << endl;

        // Read in the file.
        ifstream file(argv[2]);
        if (!file.is_open())
        {
            cerr << "File cannot be opened" << endl;
            return 1;
        };
        graph *myGraph = new graph();
        string textLine;
        int index = 0;
        while (getline(file, textLine))
        {
            istringstream iss(textLine);
            int weightToAdd;
            while (iss >> weightToAdd)
            {
                myGraph->addWeight(index, weightToAdd);
            };
            index++;
        };
        file.close();

        // Go through the path file.
        ifstream pathFile(argv[3]);
        if (!pathFile.is_open())
        {
            cerr << "File cannot be opened" << endl;
            return 1;
        };
        int totalWeight = 0;
        int pathValue;
        int initialPathValue;
        pathFile >> initialPathValue;
        while (pathFile >> pathValue)
        {
            totalWeight += myGraph->distance(initialPathValue, pathValue);
            cout << initialPathValue << "---" << myGraph->distance(initialPathValue, pathValue) << "-->" << pathValue << endl;
            initialPathValue = pathValue;
        };
        cout << "Total path distance: " << totalWeight << endl;
        pathFile.close();

        return 0;
    }
    else
    {
        // Nothing of meaning was typed, or there was a typo.
        cout << "Incorrect arguments. Type something like ./a.out programMode inputFile.ext pathToCheck.ext(if applicable)" << endl
             << "Examples of programModes: {original, nearest, brute, check}." << endl
             << "Try running the program again with those arguments." << endl;
        return 0;
    };

    // Start the Original algorithm here:
    //  Read in the file.
    cout << "Reading in the graph" << endl;
    ifstream file(argv[2]);
    if (!file.is_open())
    {
        cerr << "File cannot be opened." << endl;
        return 1;
    };
    cout << "Successfully opened the file" << endl;
    graph *myGraph = new graph();
    string textLine;
    int index = 0;
    cout << "Parsing graph file" << endl;
    while (getline(file, textLine))
    {
        istringstream iss(textLine);
        int weightToAdd;
        while (iss >> weightToAdd)
        {
            myGraph->addWeight(index, weightToAdd);
        };
        node *newNode = new node(index);
        myGraph->nodes.push_back(newNode);
        index++;
    };
    file.close();
    myGraph->numNodes = index;
    cout << "Finished reading in the graph" << endl;
    cout << "Sorting weight values" << endl;
    myGraph->sortWeghts();
    cout << "Successfully sorted weight values" << endl;

    // Start with the smallest weight in the weights vector. Iterate through each weight in the vector.
    for (int i = 0; i < myGraph->weights.size(); i++)
    {
        weight *currentWeight = myGraph->weights.at(i);
        int currentLeftNodeNumber = myGraph->weights.at(i)->fromNode;
        int currentRightNodeNumber = myGraph->weights.at(i)->toNode;
        node *currentLeftNode = myGraph->nodes.at(currentLeftNodeNumber);
        node *currentRightNode = myGraph->nodes.at(currentRightNodeNumber);

        if ((currentLeftNode->nodeType == 0) && (currentRightNode->nodeType == 0))
        {
            // Both nodes are untouched.
            // We will use this weight to connect those nodes. The nodes each become leader nodes. They are added to a group together.
            cout << currentLeftNode->nodeName << "---" << currentWeight->value << "-->" << currentRightNode->nodeName << endl;
            connectedNode *newConnectedNode = new connectedNode(currentLeftNode, currentRightNode);
            myGraph->connectedNodes.push_back(newConnectedNode);
            currentLeftNode->nodeType = 1;
            currentRightNode->nodeType = 1;
            currentLeftNode->nodeGroup = globalGroupNumber;
            currentRightNode->nodeGroup = globalGroupNumber;
            globalGroupNumber++;
            globalTotalWeight += currentWeight->value;
        }
        else if ((currentLeftNode->nodeType == 2) && (currentRightNode->nodeType == 2))
        {
            // Both are inside nodes.
            // We don't want to do anything.
            continue;
        }
        else if ((currentLeftNode->nodeType == 1) && (currentRightNode->nodeType == 1))
        {
            // Both are leader nodes.
            // We first need to check if they are in the same group.
            if (currentLeftNode->nodeGroup != currentRightNode->nodeGroup)
            {
                // If they are not in the same group, connect the two nodes with the weight, set them as inside nodes, see which group between the two is the lowest integer, then go through the nodes vector and change all nodes with the higher-integer group into that of the lower-integer.
                cout << currentLeftNode->nodeName << "---" << currentWeight->value << "-->" << currentRightNode->nodeName << endl;
                connectedNode *newConnectedNode = new connectedNode(currentLeftNode, currentRightNode);
                myGraph->connectedNodes.push_back(newConnectedNode);
                globalTotalWeight += currentWeight->value;
                currentLeftNode->nodeType = 2;
                currentRightNode->nodeType = 2;
                int smallestIntegerGroup;
                int oldIntegerGroup;
                if (currentLeftNode->nodeGroup < currentRightNode->nodeGroup)
                {
                    smallestIntegerGroup = currentLeftNode->nodeGroup;
                    oldIntegerGroup = currentRightNode->nodeGroup;
                }
                else
                {
                    smallestIntegerGroup = currentRightNode->nodeGroup;
                    oldIntegerGroup = currentLeftNode->nodeGroup;
                };
                for (int i = 0; i < myGraph->nodes.size(); i++)
                {
                    if (myGraph->nodes.at(i)->nodeGroup == oldIntegerGroup)
                    {
                        myGraph->nodes.at(i)->nodeGroup = smallestIntegerGroup;
                    };
                };
            };
        }
        else if (((currentLeftNode->nodeType == 1) && (currentRightNode->nodeType == 2)) || ((currentLeftNode->nodeType == 2) && (currentRightNode->nodeType == 1)))
        {
            // One node is a leader and one is an inside node.
            // We don't want to do anything.
            continue;
        }
        else if (((currentLeftNode->nodeType == 2) && (currentRightNode->nodeType == 0)) || ((currentLeftNode->nodeType == 0) && (currentRightNode->nodeType == 2)))
        {
            // One node is an inside node and one is untouched.
            // We don't want to do anything.
            continue;
        }
        else
        {
            // One is a leader and one is untouched.
            // We connect them. The leader becomes an inside node. The untouched node becomes a leader. The untouched node gets the group number of the leader.
            cout << currentLeftNode->nodeName << "---" << currentWeight->value << "-->" << currentRightNode->nodeName << endl;
            connectedNode *newConnectedNode = new connectedNode(currentLeftNode, currentRightNode);
            myGraph->connectedNodes.push_back(newConnectedNode);
            globalTotalWeight += currentWeight->value;
            if (currentLeftNode->nodeType == 1)
            {
                // The left node was the leader.
                currentLeftNode->nodeType = 2;
                currentRightNode->nodeType = 1;
                currentRightNode->nodeGroup = currentLeftNode->nodeGroup;
            }
            else
            {
                // The right node was the leader.
                currentRightNode->nodeType = 2;
                currentLeftNode->nodeType = 1;
                currentLeftNode->nodeGroup = currentRightNode->nodeGroup;
            };
        };
    };

    // Connect the two end nodes. These will be the only leader nodes left.
    bool oneUsed = false;
    int nodeOne;
    int nodeTwo;
    for (int i = 0; i < myGraph->nodes.size(); i++)
    {
        if (myGraph->nodes.at(i)->nodeType == 1)
        {
            if (!oneUsed)
            {
                nodeOne = myGraph->nodes.at(i)->nodeName;
                oneUsed = true;
            };
            nodeTwo = myGraph->nodes.at(i)->nodeName;
        };
    };
    node *currentLeftNode = myGraph->nodes.at(nodeOne);
    node *currentRightNode = myGraph->nodes.at(nodeTwo);
    connectedNode *newConnectedNode = new connectedNode(currentLeftNode, currentRightNode);
    myGraph->connectedNodes.push_back(newConnectedNode);
    globalTotalWeight += myGraph->distance(nodeOne, nodeTwo);

    // Starting with nodeOne (the first remaining leader node), traverse through the connected nodes to retrace our path.
    node *currentNode = currentLeftNode;
    for (int i = 0; i < myGraph->numNodes; i++)
    {
        for (int j = 0; j < myGraph->connectedNodes.size(); j++)
        {
            if (((myGraph->connectedNodes.at(j)->connectedLeftNode == currentNode) || (myGraph->connectedNodes.at(j)->connectedRightNode == currentNode)) && (myGraph->connectedNodes.at(j)->checked == false))
            {
                if (myGraph->connectedNodes.at(j)->connectedLeftNode == currentNode)
                {
                    myGraph->pathTaken.push_back(currentNode->nodeName);
                    currentNode = myGraph->connectedNodes.at(j)->connectedRightNode;
                }
                else
                {
                    myGraph->pathTaken.push_back(currentNode->nodeName);
                    currentNode = myGraph->connectedNodes.at(j)->connectedLeftNode;
                };
                myGraph->connectedNodes.at(j)->checked = true;
            };
        };
    };
    myGraph->pathTaken.push_back(currentLeftNode->nodeName);

    // Write the output to a file
    cout << "Writing path to file" << endl;
    string fileName = "S" + to_string(globalTotalWeight) + "_wcjunkins.sol";
    ofstream outFile(fileName);
    if (!outFile.is_open())
    {
        cerr << "Failed to open the file for writing" << endl;
        return 1;
    };
    for (int i = 0; i <= myGraph->numNodes; i++)
    {
        outFile << myGraph->pathTaken.at(i) << " ";
        cout << myGraph->pathTaken.at(i) << " ";
    };
    outFile.close();

    // Ending total.
    cout << endl
         << "Total Distance: " << globalTotalWeight << endl;

    cout << "The shortest path has been successfully generated" << endl
         << "A copy of the complete path has been saved to the file " << fileName << endl
         << "Closing program..." << endl;

    return 0;
};