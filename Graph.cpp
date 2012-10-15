#include<iostream>
#include<vector>
#include<fstream>
#include<queue>
#include<list>

using namespace std;

typedef int Vertex;

//A Node in the Adjacency List of a graph
class Node
{
public:
	Vertex endVertex;
	int capacity;
	int flow;
	Node* next;

	Node(Vertex endVertex, int capacity, int flow, Node* next)
	{
		this->endVertex = endVertex;
		this->capacity = capacity;
		this->flow = flow;
		this->next = next;		
	}

};

//Edge of Graph
class Edge{

	Vertex source;
	Vertex destination;
	int flow;
	int capacity;

public:

	Edge(){
	}

	Edge(Vertex source, Vertex destination){
		this->source = source;
		this->destination = destination;
		capacity = 0;
		flow = 0;
	}


	Edge(Vertex source, Vertex destination, int capacity , int flow){
		this->source = source;
		this->destination = destination;
		this->capacity = capacity;
		this->flow = flow;
	}

	Vertex getSource(){
		return source;
	}
	Vertex getDestination(){
		return destination;
	}
	int getCapacity(){
		return capacity;
	}
	int getFlow(){
		return flow;
	}

};


//Class for Graph
class Graph
{
	int numofNodes;
	int numofEdges;

	Node** adjList; //Adjacency List

public:
	//Constructor
	Graph(int numofNodes)
	{
		this->numofNodes = numofNodes;
		this->numofEdges = 0;
		adjList = new Node*[numofNodes];
		for(int i=0 ; i < numofNodes ; i++)
		{
			adjList[i] = NULL;
		}
	}

	int getNumOfNodes(){
		return numofNodes;
	}

	int getNumOfEdges(){
		return numofEdges;
	}

	//Get an edge between the given vertices u->v
	Edge* getEdge(Vertex u, Vertex v)
	{

		Node* p = adjList[u-1];
		while(p!=NULL){
			if(p->endVertex == v){
				Edge* e = new Edge(u , v , p->capacity , p->flow);
				return e;
			}
			p = p->next;
		}
		return NULL;
	}

	//Insert an edge into the graph
	void insertEdge(Edge e)
	{
		Vertex source = e.getSource();
		Vertex dest = e.getDestination();

		int flow = e.getFlow();
		int capacity = e.getCapacity();
		Node * newNode = new Node(dest,capacity,flow, NULL);


		Node* p = adjList[source - 1];
		if(p == NULL){
			adjList[source - 1] = newNode;
		}else{

			while(p->next !=NULL){
				p = p->next;
			}

			p->next = newNode;
		}

		++numofEdges;
	}


	//Remove an edge from the graph
	void removeEdge(Edge e)
	{
		Vertex source = e.getSource();
		Vertex dest = e.getDestination();
		bool edgeFound = false;

		//delete node from adj List of the vertex

		if(adjList[source-1]==NULL){
			cout<<"Edge does not exist";
		}
		else if(adjList[source-1]->endVertex == dest)
		{
			Node* temp = adjList[source-1];
			adjList[source-1] = adjList[source-1]->next;
			delete temp;
		}
		else
		{
			Node* p = adjList[source-1]->next;
			Node*q = adjList[source-1];
			bool edgeFound = false;
			while(p!=NULL)
			{
				if(p->endVertex == dest){
					q->next = p->next;
					delete p;
					edgeFound = true;
					break;
				}else{
					q = p;
					p = p->next;
				}
			}
			if(edgeFound == false)
			{
				cout<<"\nEdge not found";
			}else{
				//cout<<"\nEdge deleted";
				--numofEdges;
			}
		}		
	}


	// Modify the capacity and flow of edge represented by argument e
	void modifyEdge(Edge e)
	{
		Vertex source = e.getSource();
		Vertex destination = e.getDestination();
		Node* p= adjList[source -1];

		while(p!=NULL){
			if(p->endVertex == destination)
			{
				break;
			}
			p = p->next;
		}

		if(p==NULL){
			cout<< "Edge does not exist";
		}else{

			p->capacity = e.getCapacity();
			p->flow = e.getFlow();
		}
	}


	// Calculate the total flow from the source
	
	int getTotalFlowFromSource(Vertex src)
	{
		Node* p = adjList[src -1];
		int flow = 0;
		while(p!=NULL){
			flow = flow + p->flow;
			p = p->next;
		}
		return flow;
	}

	//Calculate Total capacity from source
	int getMaxCapFromSource(Vertex src)
	{
		Node* p = adjList[src -1];
		int maxCap = INT_MIN;
		while(p!=NULL){
			if(p->capacity > maxCap)
			{
				maxCap = p->capacity;
			}
			p = p->next;
		}
		return maxCap;
	}


	// Get All the Edges of the Graph
	vector<Edge> getAllEdges(){

		vector<Edge> edges;
		for(int i=0; i<numofNodes;i++)
		{
			Node* p = adjList[i];
			Vertex src = i+1;
			while(p!=NULL){
				Vertex dest = p->endVertex;
				int capacity = p->capacity;
				int flow = p->flow;
				Edge e(src,dest,capacity,flow);
				edges.push_back(e);
				p = p->next;
			}
		}
		return edges;
	}



	//Find path between start and end vertex using BFS
	vector<Edge*> findPathByBFS(Vertex s , Vertex t)
	{
		bool* visited = new bool[numofNodes];
		queue<int> nodeQueue;
		nodeQueue.push(s);
		visited[s-1] = true;
		int* parent = new int[numofNodes];
		bool pathExist = false;
		vector<Edge*> edgeList;
		while(!nodeQueue.empty())
		{
			Vertex u = nodeQueue.front();
			nodeQueue.pop();
			Node *p = adjList[u-1];
			while(p!=NULL)
			{
				Vertex v = p->endVertex;
				if(visited[v-1] != true)
				{
					visited[v-1] = true;
					parent[v-1] = u;
					nodeQueue.push(v);

					if(v == t)	//destination vertex found
					{ 
						pathExist = true;
						break;
					}
				}
				p=p->next;
			}

			if(pathExist==true){
				break;
			}
		}

		if(pathExist==false)
		{
			//cout<<"Path does not exist";
		}
		else
		{
			list<int> path;
			int p = t;
			path.push_front(p);

			while(p!=s){

				p = parent[p-1];
				path.push_front(p);

			}

			list<int>::iterator it;
			it = path.begin();
			Vertex src = *it;
			it++;
			//cout<<"s-t path : ";
			//cout<<src<<"->";
			while(it!=path.end()){

				
				Vertex dest = *it;
				Edge* e = getEdge(src, dest);
				edgeList.push_back(e);
				src = dest;
				//cout<<src<<"->";
				it++;
			}
			//cout<<endl;
		}

		return edgeList;
	}


	//To Find the sets of vertices in the 
	//residual graph reachable from the source vertex and  those that 
	//are not

	vector< vector<int> > findCut(Vertex s){
		
		vector< vector<int> > cut;
		vector<int> setS;
		vector<int> setT;
		
		setS.push_back(s);
		for(int i=1; i<=numofNodes ; i++){
			if(s==i) continue;
			vector<Edge*> path = findPathByBFS(s , i);
			
			if (path.size()!=0){
				setS.push_back(i);
			}else{
				setT.push_back(i);
			}
		}
		cut.push_back(setS);
		cut.push_back(setT);
		return cut;

	}

};


