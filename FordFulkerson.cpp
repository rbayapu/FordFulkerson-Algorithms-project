#include<iostream>
#include<vector>
#include<fstream>
#include<queue>
#include<list>
#include<limits>
#include<math.h>
#include <termios.h>
#include"Graph.cpp"

Vertex s,t; 

//Define initial graph
//input : input file
//Output : Graph
Graph* createInitGraph(){

	string line;
	int nV,nE, capacity;
	Vertex source, destination;
	string filename;
	const char *f;
	cout<<"Enter the input file(Ex:sample-input01):";
	getline(cin,filename);
        f = (char *)filename.c_str();
	//cin>>filename;
	
	ifstream buffer;
        buffer.open(f);
	if( !buffer ){
	cout<<"Input a valid file";
	return 0;
       }


	if(buffer.is_open())
	{
		buffer>>nV;
		buffer>>nE;
		buffer>>s;
		buffer>>t;

		Graph* graph = new Graph(nV);
		while(true)
		{
			buffer>>source;
			buffer>>destination;
			buffer>>capacity;

			Edge e(source, destination, capacity , 0);
			graph->insertEdge(e);

			if(buffer.eof())
			{
				break;
			}
		}

		return graph;
	}
	return NULL;
}


//Define residual graph
//input : graph
//output : residual graph

Graph* createResidualGraph(Graph* graph){
	int nV = graph->getNumOfNodes();
	Graph* residualGraph = new Graph(nV);
	vector<Edge> edges = graph->getAllEdges();
	vector<Edge>::iterator it;
	for(it=edges.begin();it!=edges.end(); it++){

		Edge e = *it;
		int capacity = e.getCapacity();
		int flow = e.getFlow();
		int resCapacity = capacity - flow;

		Vertex src = e.getSource();
		Vertex dest = e.getDestination();

		//Insert forward edge with capacity = residual capacity
		if(resCapacity > 0){
			Edge fwdEdge(src, dest,resCapacity,0 /*Flow not required */);
			residualGraph->insertEdge(fwdEdge);
		}

		if(flow>0){
			//Insert reverse Edge with capacity = flow
			Edge revEdge(dest ,src ,flow , 0 );
			residualGraph->insertEdge(revEdge);
		}

	}
	return residualGraph;
}

//Create (Delta) Residual Graph from a given residual graph

void createResidualGraphDelta(Graph* resGraph , int delta){
	vector<Edge> edges = resGraph->getAllEdges();
	vector<Edge>::iterator it;
	for(it=edges.begin();it!=edges.end(); it++){

		Edge e = *it;
		int capacity = e.getCapacity();


		Vertex src = e.getSource();
		Vertex dest = e.getDestination();

		//Remove all edges of capacity less than delta
		if(capacity < delta){
			resGraph->removeEdge(e);	
		}	
	}	
}

//Augment path
void augment(Graph* graph , vector<Edge*> path)
{
	vector<Edge*>::iterator it;
	int minCap = INT_MAX;    //bottleneck
	for(it = path.begin(); it!=path.end(); it++){
		int capacity = (*it)->getCapacity(); 
		if (capacity < minCap)
		{
			minCap = capacity;
		}
	}

	for(it = path.begin(); it!=path.end(); it++){
		Vertex src = (*it)->getSource();
		Vertex dest = (*it)->getDestination();

		Edge* fwdEdge = graph->getEdge(src, dest);
		Edge* revEdge = graph->getEdge(dest, src);


		if(fwdEdge!=NULL ){
			int flow = fwdEdge->getFlow();
			flow = flow + minCap;

			int capacity = fwdEdge->getCapacity();

			Edge e(src , dest , capacity, flow);

			graph->modifyEdge(e);
		} 

		if(revEdge!=NULL){

			int flow = revEdge->getFlow();
			flow = flow - minCap;

			int capacity = revEdge->getCapacity();

			Edge e(dest , src , capacity, flow);

			graph->modifyEdge(e);
		}
	}
}

void displayGraph(Graph* graph)
{
	vector<Edge> allEdges = graph->getAllEdges();
	vector<Edge>::iterator it;
	cout<<"\n";
	for(it = allEdges.begin();it!=allEdges.end();it++ )
	{
		Edge e =*it;
		if(e.getFlow()!=0){
			cout << "  " << e.getSource() << "  " << e.getDestination() <<"  "<<e.getFlow()<<endl;
		}
	}
	cout<<endl;
}

int mygetch( ) {
  struct termios oldt,
                 newt;
  int            ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}


int main()
{
	Graph* graph = createInitGraph();
	Graph* residualGraph;
	int maxFlow;

	//Capacity Scaling
	int delta = 1 ;
	int maxCap = graph->getMaxCapFromSource(s);

	while(delta < maxCap)
	{
		delta = delta*2;
	}
	if(delta > maxCap)
	{
		delta = delta /2;
	}

	while(delta >=1){	
		residualGraph = createResidualGraph(graph);
		createResidualGraphDelta(residualGraph, delta);

		vector<Edge*> path = residualGraph->findPathByBFS(s,t);
		while(path.size()!=0 )
		{
			augment(graph , path);
			residualGraph = createResidualGraph(graph);
			createResidualGraphDelta(residualGraph, delta);
			path = residualGraph->findPathByBFS(s,t);
		}

		maxFlow = graph->getTotalFlowFromSource(s);

		cout<<"\nWith DELTA = "<<delta<<", Flow Value = "<<maxFlow;
		delta = delta/2;
	}

	cout<<endl<<"Maximum-FLow value : "<<maxFlow;
	cout<<endl<<endl<<"The max-flow:";

	displayGraph(graph);

	//Find Min-Cut
	vector< vector<int> > minCut = residualGraph->findCut(s);
	vector<int> setS = minCut[0];
	vector<int> setT = minCut[1];
	
	//Calculate Min-Cut Capacity
	
	int minCutCapacity = 0;
	
	vector<int>::iterator sit;
	vector<int>::iterator tit;

	for(sit=setS.begin();sit!=setS.end();sit++)
	{
		for(tit=setT.begin();tit!=setT.end();tit++)
		{
			Edge* e = graph->getEdge(*sit,*tit);
			if(e!=NULL){

				minCutCapacity += e->getCapacity();
			}
		}

	}

	cout<<endl<<"Min-Cut Capacity = "<<minCutCapacity;

	vector<int>::iterator it;
	cout<<endl<<"The min cut :"<<endl;
	cout<<"Set S :";
	for(it=setS.begin();it!=setS.end();it++)
	{
		cout<<*it<<", ";
	}

	cout<<endl;
	cout<<"Set T :";
	for(it=setT.begin();it!=setT.end();it++)
	{
		cout<<*it<<", ";
	}


	
	return 0;
}
