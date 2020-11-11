#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <cstring>
#include <climits>
using namespace std;
// N is number of total nodes on the graph or the cities in the map
#define N 11
// Sentinal value for representing infinity
#define INF INT_MAX
// State Space Tree nodes
vector<int>patt;
struct Node
{
	// stores edges of state space tree
	// helps in tracing path when answer is found
	vector<pair<int, int>> path;
	// stores the reduced matrix
	int reducedMatrix[N][N];
	// stores the lower bound
	int cost;
	//stores current city number
	int vertex;
	// stores number of cities visited so far
	int level;
};
// Function to allocate a new node (i, j) corresponds to visiting
// city j from city i
Node* newNode(int parentMatrix[N][N], vector<pair<int, int>> const &path,
			int level, int i, int j)
{
	Node* node = new Node;
	// stores ancestors edges of state space tree
	node->path = path;
	// skip for root node
	if (level != 0)
		// add current edge to path
		node->path.push_back(make_pair(i, j));
	// copy data from parent node to current node
	memcpy(node->reducedMatrix, parentMatrix,
		sizeof node->reducedMatrix);
	// Change all entries of row i and column j to infinity
	// skip for root node
	for (int k = 0; level != 0 && k < N; k++)
	{
		// set outgoing edges for city i to infinity
		node->reducedMatrix[i][k] = INF;
		// set incoming edges to city j to infinity
		node->reducedMatrix[k][j] = INF;
	}
	// Set (j, 0) to infinity
	// here start node is 0
	node->reducedMatrix[j][0] = INF;
	// set number of cities visited so far
	node->level = level;
	// assign current city number
	node->vertex = j;
	// return node
	return node;
}
// Function to reduce each row in such a way that
// there must be at least one zero in each row
int rowReduction(int reducedMatrix[N][N], int row[N])
{
	// initialize row array to INF
	fill_n(row, N, INF);
	// row[i] contains minimum in row i
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (reducedMatrix[i][j] < row[i])
				row[i] = reducedMatrix[i][j];
	// reduce the minimum value from each element in each row
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (reducedMatrix[i][j] != INF && row[i] != INF)
				reducedMatrix[i][j] -= row[i];
	return 0;
}
// Function to reduce each column in such a way that
// there must be at least one zero in each column
int columnReduction(int reducedMatrix[N][N], int col[N])
{
	// initialize col array to INF
	fill_n(col, N, INF);
	// col[j] contains minimum in col j
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (reducedMatrix[i][j] < col[j])
				col[j] = reducedMatrix[i][j];
	// reduce the minimum value from each element in each column
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (reducedMatrix[i][j] != INF && col[j] != INF)
				reducedMatrix[i][j] -= col[j];
	return 0;
}
// Function to get the lower bound on
// on the path starting at current min node
int calculateCost(int reducedMatrix[N][N])
{
	// initialize cost to 0
	int cost = 0;
	// Row Reduction
	int row[N];
	rowReduction(reducedMatrix, row);
	// Column Reduction
	int col[N];
	columnReduction(reducedMatrix, col);
	// the total expected cost
	// is the sum of all reductions
	for (int i = 0; i < N; i++)
		cost += (row[i] != INT_MAX) ? row[i] : 0,
			cost += (col[i] != INT_MAX) ? col[i] : 0;
	return cost;
}
// print list of cities visited following least cost
void printPath(vector<pair<int, int>> const &list)
{
	for (int i = 0; i < list.size(); i++)
		cout << list[i].first + 1 << " -> "
			<< list[i].second + 1 << endl;
}
void path(vector<pair<int, int>> const &list)
{
	for (int i = 0; i < list.size(); i++)
	{
		if(patt.size()==0)
		{
			patt.push_back(list[i].first+1);
		}
		else if (patt.back() != list[i].first + 1)
		{
			patt.push_back(list[i].first + 1);
		}
		patt.push_back(list[i].second + 1);
	}
}
	// Comparison object to be used to order the heap
struct comp
{
	bool operator()(const Node* lhs, const Node* rhs) const
	{
		return lhs->cost > rhs->cost;
	}
};
// Function to solve Traveling Salesman Problem using Branch and Bound
int solve(int costMatrix[N][N])
{
	// Create a priority queue to store live nodes of search tree;
	priority_queue<Node*, std::vector<Node*>, comp> pq;
	vector<pair<int, int>> v;
	// create a root node and calculate its cost
	// The TSP starts from first city i.e. node 0
	Node* root = newNode(costMatrix, v, 0, -1, 0);
	// get the lower bound of the path starting at node 0
	root->cost = calculateCost(root->reducedMatrix);
	// Add root to list of live nodes;
	pq.push(root);
	// Finds a live node with least cost, add its children to list of
	// live nodes and finally deletes it from the list
	while (!pq.empty())
	{
		// Find a live node with least estimated cost
		Node* min = pq.top();
		// The found node is deleted from the list of live nodes
		pq.pop();
		// i stores current city number
		int i = min->vertex;
		// if all cities are visited
		if (min->level == N - 1)
		{
			// return to starting city
			min->path.push_back(make_pair(i, 0));
			// print list of cities visited;
			printPath(min->path);
			path(min->path);
			// return optimal cost
			return min->cost;
		}
		// do for each child of min
		// (i, j) forms an edge in space tree
		for (int j = 0; j < N; j++)
		{
			if (min->reducedMatrix[i][j] != INF)
			{
				// create a child node and calculate its cost
				Node* child = newNode(min->reducedMatrix, min->path,
					min->level + 1, i, j);
				/* Cost of the child =

					cost of parent node +

					cost of the edge(i, j) +

					lower bound of the path starting at node j

				*/
				child->cost = min->cost + min->reducedMatrix[i][j]
							+ calculateCost(child->reducedMatrix);
				// Add child to list of live nodes
				pq.push(child);
			}
		}
		// free node as we have already stored edges (i, j) in vector.
		// So no need for parent node while printing solution.
		delete min;
	}
	return 0;
}
int main()
{
	char colour[11][30] = {"Amsterdam", "New York", "Hudson", "Middletown", "Mechanicville", "Rochester", "Kingston", "Norwich", "Niagara Falls", "Jamestown", "Cortland"};
	// cost matrix for traveling salesman problem.
	int costMatrix[N][N] =
	{

		{INF, 29, 20, 21, 16, 31, 100, 12, 4, 31, 18},

                      {29, INF, 15, 29, 28, 40, 72, 21, 29, 41, 12},

                      {20, 15, INF, 15, 14, 25, 81, 9, 23, 27, 13},

                      {21, 29, 15, INF, 4, 12, 92, 12, 25, 13, 25},

                      {16, 28, 14, 4, INF, 16, 94, 9, 20, 16, 22},

                      {31, 40, 25, 12, 16, INF, 95, 24, 36, 3, 37},

                      {100, 72, 81, 92, 94, 95, INF, 90, 101, 99, 84},

                      {12, 21, 9, 12, 9, 24, 90, INF, 15, 25, 13},

                      {4, 29, 23, 25, 20, 36, 101, 15, INF, 35, 18},

                      {31, 41, 27, 13, 16, 3, 99, 25, 35, INF, 38},

                      {18, 12, 13, 25, 22, 37, 84, 13, 18, 38, INF}

	};
	cout << "\n\nTotal Cost is " << solve(costMatrix)<<endl;


	for(int i=0;i<patt.size()-1;i++)
	{
		cout<<patt[i]<<"->";
	}
	cout<<patt[patt.size()-1]<<endl;
	for (int i = 0; i < patt.size() - 1; i++)
	{
		cout << colour[patt[i]-1] << " ---> ";
	}
	cout << colour[patt[patt.size() - 1]-1];
	cout<<endl;
	return 0;

}
