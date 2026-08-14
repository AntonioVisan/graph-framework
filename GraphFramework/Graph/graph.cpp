#include "../Graph/graph.h"
#include <fstream>

void Graph::printAdjacencyMatrix()
{
    std::ofstream fout("adjacencyMatrix.txt");
    fout<<m_nodes.size()<<std::endl;
    for(int i=0;i<m_adjacencyMatrix.size();i++)
    {
        for(int j=0;j<m_adjacencyMatrix[i].size();j++)
            fout<<m_adjacencyMatrix[i][j]<<" ";
        fout<<std::endl;
    }
    fout.close();
}

Graph::Graph() {}

Graph::Graph(const Graph &other)
{
    for(auto node : other.m_nodes)
    {
        int index=node->getIndex();
        QPoint p = node->getCoord();
        m_nodes.push_back(new Node(index,p));
    }
    for(auto &edge : other.m_edges)
    {
        Node *firstNode=nullptr, *secondNode=nullptr;
        for(auto n : m_nodes)
        {
            if(n->getIndex()==edge.getFirst()->getIndex()) firstNode=n;
            else if(n->getIndex()==edge.getSecond()->getIndex()) secondNode=n;
        }
        m_edges.push_back(Edge(firstNode,secondNode));
    }
    m_oriented=other.m_oriented;
    m_adjacencyMatrix=other.m_adjacencyMatrix;
}

Graph& Graph::operator=(const Graph &other)
{
    if(this==&other) return *this;

    for(auto node : m_nodes) delete node;
    m_nodes.clear();
    m_edges.clear();

    for(auto node : other.m_nodes)
    {
        int index=node->getIndex();
        QPoint p = node->getCoord();
        m_nodes.push_back(new Node(index,p));
    }

    for(auto &edge : other.m_edges)
    {
        Node* firstNode=nullptr, *secondNode=nullptr;
        for(auto n : m_nodes)
        {
            if(n->getIndex()==edge.getFirst()->getIndex()) firstNode=n;
            else if(n->getIndex()==edge.getSecond()->getIndex()) secondNode=n;
        }
        m_edges.push_back(Edge(firstNode,secondNode));
    }
    m_oriented=other.m_oriented;
    m_adjacencyMatrix=other.m_adjacencyMatrix;

    return *this;
}

Graph::~Graph()
{
    for(auto node : m_nodes) delete node;
    m_nodes.clear();
    m_edges.clear();
    m_oriented=false;
    m_adjacencyMatrix.clear();

}

void Graph::addNode(QPoint p)
{
    Node* n = new Node;
    n->setCoord(p);
    int nrNodes=m_nodes.size()+1;
    n->setIndex(nrNodes);
    m_adjacencyMatrix.resize(nrNodes);
    for(auto& row : m_adjacencyMatrix)
        row.resize(nrNodes,0);
    m_nodes.push_back(n);
    printAdjacencyMatrix();
}

void Graph::addEdge(Node* f, Node* s)
{
    Edge e(f,s);
    int i=f->getIndex();
    int j=s->getIndex();
    if(m_oriented)
        m_adjacencyMatrix[i-1][j-1]=1;
    else m_adjacencyMatrix[i-1][j-1]=m_adjacencyMatrix[j-1][i-1]=1;
    m_edges.push_back(e);
    printAdjacencyMatrix();
}

void Graph::changeState()
{
    m_oriented= !m_oriented;
    if(!m_oriented)
    {
        std::vector<Edge> uniqueEdges;
        for(auto &edge : m_edges)
        {
            int a=edge.getFirst()->getIndex();
            int b=edge.getSecond()->getIndex();
            bool exists=false;
            for(auto &uEdge : uniqueEdges)
            {
                int ua=uEdge.getFirst()->getIndex();
                int ub=uEdge.getSecond()->getIndex();
                if(a==ua && b==ub || a==ub && b==ua)
                {
                    exists=true;
                    break;
                }
            }
            if(!exists)
                uniqueEdges.push_back(edge);
        }
        m_edges=uniqueEdges;
        m_adjacencyMatrix.clear();
        m_adjacencyMatrix.assign(m_nodes.size(),std::vector<int>(m_nodes.size(),0));
        for(auto &edge : m_edges)
        {
            int i=edge.getFirst()->getIndex();
            int j=edge.getSecond()->getIndex();
            m_adjacencyMatrix[i-1][j-1]=m_adjacencyMatrix[j-1][i-1]=1;
        }
        printAdjacencyMatrix();
    }
    else
    {
        m_adjacencyMatrix.clear();
        m_adjacencyMatrix.assign(m_nodes.size(),std::vector<int>(m_nodes.size(),0));
        for(auto &edge : m_edges)
        {
            int i=edge.getFirst()->getIndex();
            int j=edge.getSecond()->getIndex();
            m_adjacencyMatrix[i-1][j-1]=1;
        }
        printAdjacencyMatrix();
    }
}

std::vector<Node*> Graph::getNodes()
{
    return m_nodes;
}

std::vector<Edge> Graph::getEdges()
{
    return m_edges;
}

std::vector<std::vector<int>> Graph::getAdjacencyMatrix()
{
    return m_adjacencyMatrix;
}

bool Graph::isOriented()
{
    return m_oriented;
}
