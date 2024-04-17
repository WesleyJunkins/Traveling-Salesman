#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>

using namespace std;

class node
{
private:
    double xCoord;
    double yCoord;
    bool used = false;
    bool startingNode = false;

public:
    node()
    {
        xCoord = rand() % 100;
        yCoord = rand() % 100;
    }

    node(double xCoord, double yCoord)
    {
        this->xCoord = xCoord;
        this->yCoord = yCoord;
    }

    void sayCoord()
    {
        cout << "X:" << xCoord << ", Y:" << yCoord;
    }

    double getX()
    {
        return xCoord;
    }

    double getY()
    {
        return yCoord;
    }

    void setUsedTrue()
    {
        this->used = true;
    }

    void setUsedFalse()
    {
        this->used = false;
    }

    void setStartingNodeTrue()
    {
        this->startingNode = true;
    }

    void setStartingNodeFalse()
    {
        this->startingNode = false;
    }

    bool isUsed()
    {
        if(this->used == true)
        {
            return true;
        }

        return false;
    }

    bool isStartingNode()
    {
        if(this->startingNode == true)
        {
            return true;
        }

        return false;
    }
};

double getDistance(node fromNode, node toNode)
{
    return (sqrt(((toNode.getX() - fromNode.getX()) * (toNode.getX() - fromNode.getX())) + ((toNode.getY() - fromNode.getY()) * (toNode.getY() - fromNode.getY()))));
};

vector<node*> insertNodes()
{
    bool goAgain = true;
    vector<node *> nodeBank;
    do
    {
        char answer;
        double newNodeX;
        double newNodeY;
        cout << "New Node Creation" << endl
             << "X: ";
        cin >> newNodeX;
        cout << endl
             << "Y: ";
        cin >> newNodeY;
        node *newNode = new node(newNodeX, newNodeY);
        nodeBank.push_back(newNode);
        cout << endl
             << endl
             << "New node created at (" << newNodeX << "," << newNodeY << ")." << endl
             << "Would you like to create a new node? (Y/N) ";
        cin >> answer;
        if (toupper(answer) == 'Y')
        {
            goAgain = true;
        }
        else
        {
            goAgain = false;
        }
    } while (goAgain);

    return nodeBank;
};

//A function that finds a path through the graph using the nearest neighbor algorithm. Starting at the startingPosition and wrapping back around to it.
double nearestNeighbor(int startingPosition, vector<node*> nodeBank)
{
    node* current = nodeBank.at(startingPosition);
    double minDistance = INT_MAX;
    int targetPosition = 0;
    //Iterate through each node in the nodeBank. See which one has the smallest distance to currentNode.
    for(int i = 0; i < nodeBank.size(); i++)
    {
        if((i != startingPosition) && (nodeBank.at(i)->isUsed() == false))
        {
            node* next = nodeBank.at(i);
            if(getDistance(*current, *next) < minDistance)
            {
                minDistance = getDistance(*current, *next);
                targetPosition = i;
            }
        }
    } //Now, target is the nearest neighbor to current. We go again with target being the new startingPosition
    nodeBank.at(startingPosition)->setUsedTrue();

    if(minDistance == INT_MAX) //No nodes left
    {
        cout<<"That was our last node. Now connecting back to the starting node.\n";
        node* startingNode = nullptr;
        for(int i = 0; i < nodeBank.size(); i++) //Find the starting node
        {
            if(nodeBank.at(i)->isStartingNode() == true)
            {
                startingNode = nodeBank.at(i);
            }
        }
        //Add on the distance from the last node to the starting node to complete the circuit
        cout<<"Distance back to starting node: "<<getDistance(*current, *startingNode)<<endl;
        return getDistance(*current, *startingNode);
    }
    cout<<"Went from ";
    nodeBank.at(startingPosition)->sayCoord();
    cout<<" to ";
    nodeBank.at(targetPosition)->sayCoord();
    cout<<" with distance: "<<getDistance(*nodeBank.at(startingPosition), *nodeBank.at(targetPosition))<<endl;
    double previousRound = nearestNeighbor(targetPosition, nodeBank);
    return (previousRound + getDistance(*nodeBank.at(startingPosition), *nodeBank.at(targetPosition)));
};

int main()
{
    vector<node*> nodeBank = insertNodes();
    int myStart = 0; //Have the user enter this later.
    nodeBank.at(myStart)->setStartingNodeTrue();
    cout<<nearestNeighbor(myStart, nodeBank);

    return 0;
}

