#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <cmath>
#include <stdlib.h>

typedef struct ATTRIBUTE{
    double AsNumeric;
    std::string AsString;
}Attribute;

/*
*	Clase Vertices para la creación de Grafos
*/
class Node {
	private:
		std::string id;
		std::map<std::string,Attribute> attrs;
		std::list<Node *> adj; //Lista de vertices adyacentes
	public:
		Node() {}
		Node(std::string k);
		~Node() {}
		void AddNodeAdj(Node*);
		void AddAttr(std::string, Attribute);
		std::string GetId();
		Attribute GetAttr(std::string name);
		std::map<std::string,Attribute> GetAttrs();
		std::list<Node*> GetListAdj();
};

//Constructor
Node::Node(std::string k)
{
	id = k;
}

void Node::AddNodeAdj(Node *v)
{
    adj.push_back(v);
}

void Node::AddAttr(std::string name, Attribute value)
{
    attrs[name] = value;
}

std::string Node::GetId()
{
    return id;
}

Attribute Node::GetAttr(std::string name)
{
    return this->attrs[name];
}

std::map<std::string,Attribute> Node::GetAttrs()
{
    return this->attrs;
}

std::list<Node*> Node::GetListAdj()
{
	return adj;
}

/*
*	Clase aristas para la creación de Grafos
*/

class Edge {
	private:
		Node *node_source;
		Node *node_dest;
		std::map<std::string,Attribute> attrs;

	public:
		Edge() {node_source = NULL; node_dest = NULL;}
		~Edge() {}
		Edge(Node *, Node *);
		Node *GetNodeSource();
		void AddAttr(std::string, Attribute);
		Node *GetNodeDest();
		Attribute GetAttr(std::string name);
		std::map<std::string, Attribute> GetAttrs();
};

Edge::Edge(Node *n_source, Node *n_dest)
{
	node_source = n_source;
    node_dest = n_dest;
}

void Edge::AddAttr(std::string name, Attribute value)
{
    attrs[name] = value;
}


Node *Edge::GetNodeSource()
{
	return node_source;
}

Node *Edge::GetNodeDest()
{
    return node_dest;
}

Attribute Edge::GetAttr(std::string name)
{
    return attrs[name];
}

std::map<std::string, Attribute> Edge::GetAttrs()
{
    return attrs;
}


/*
*	Clase Grafos
*/
class Graph {
	private:
		std::string id;
		std::string cls;
		std::map<std::string, Node *> nodes;
		std::map<std::string, Edge *> edges;
	public:
		Graph() {}
		~Graph() {}
		Graph(std::string, std::string);
		Graph(std::string, std::string, std::map<std::string, Node *>, std::map<std::string,Edge*>);
		void SetId(std::string);
		void SetCls(std::string);
		void AddNode(std::string, Node*);
		void AddEdge(Edge*, std::string);
		std::string GetId();
		std::string GetCls();
		std::map<std::string, Node *> GetNodes();
		int GetNumNodes();
		int GetNumEdges();
		std::map<std::string, Edge *> GetEdges();
		Node* GetNode(std::string id);
		Edge* GetEdge(std::string id);
                int size;
};

Graph::Graph(std::string _id, std::string _cls)
{
	id = _id;
	cls = _cls;
}

Graph::Graph(std::string _id, std::string _cls, std::map<std::string, Node *> _nodes, std::map<std::string, Edge *> _edges)
{
	id = _id;
	cls = _cls;
	nodes = _nodes;
    edges = _edges;
}

void Graph::SetId(std::string id)
{
	this->id = id;
}

void Graph::SetCls(std::string cls)
{
	this->cls = cls;
}

void Graph::AddNode(std::string key, Node *node)
{
	this->nodes[key] = node;
}

void Graph::AddEdge(Edge * edge, std::string edgemode)
{
	//añadimos arista
	edges[edge->GetNodeSource()->GetId()+"-"+edge->GetNodeDest()->GetId()] = edge;
	//añadimos adyacencia al nodo origen
	if(nodes.count(edge->GetNodeSource()->GetId()))
		nodes[edge->GetNodeSource()->GetId()]->AddNodeAdj(edge->GetNodeDest());

	//añadimos adyacencia al nodo destino
	if(nodes.count(edge->GetNodeDest()->GetId()) && edgemode == "undirected")
	{
		nodes[edge->GetNodeDest()->GetId()]->AddNodeAdj(edge->GetNodeSource());
		edges[edge->GetNodeDest()->GetId()+"-"+edge->GetNodeSource()->GetId()] = edge;
	}
}

std::string Graph::GetId()
{
	return id;
}

std::string Graph::GetCls()
{
	return cls;
}

std::map<std::string, Node *> Graph::GetNodes()
{
	return nodes;
}

int Graph::GetNumNodes()
{
	return nodes.size();
}

int Graph::GetNumEdges()
{
	return edges.size();
}

Node * Graph::GetNode(std::string id)
{
	return nodes[id];
}

Edge* Graph::GetEdge(std::string id)
{
	return edges[id];
}

std::map<std::string, Edge *> Graph::GetEdges()
{
	return edges;
}

/*
* Método que imprime un Grafo
*/
void print_graph_example(Graph *g)
{
	//Imprimos lista de nodos
	std::map<std::string,Node*> lnodes = g->GetNodes();
	std::map<std::string,Node*>::iterator iter_nodes;
	std::list<Node *>::iterator iter_adj;
	std::list<Node *> list_adj;

	std::cout << "Grafo ID:" << g->GetId() << std::endl;

	for (iter_nodes = lnodes.begin(); iter_nodes != lnodes.end(); iter_nodes++)
	{
		std::cout << "Nodo ID: "<< lnodes[iter_nodes->first]->GetId() << "\t Nodos adyacentes: "; //Imprimimos nodos
		//Imprimimos nodos adyacentes al nodo
		list_adj = lnodes[iter_nodes->first]->GetListAdj();
		for(iter_adj = list_adj.begin(); iter_adj != list_adj.end(); iter_adj++)
			std::cout << (*iter_adj)->GetId() << " ";
		
		//Imprimimos atributos de un nodo
		std::map<std::string, Attribute> attrs = lnodes[iter_nodes->first]->GetAttrs();
		std::map<std::string, Attribute>::iterator iter_attr;
		std::cout << "\nAtributos: ";
		for(iter_attr = attrs.begin(); iter_attr != attrs.end(); iter_attr++)
			std::cout << "[" << iter_attr->first << ": " << attrs[iter_attr->first].AsString << "] ";
		std::cout << std::endl;
	}

	//Imprimimos lista de aristas
	/*
	std::list<Edge*> ledges = g->GetEdges();
	std::list<Edge*>::iterator iterl;

	for (iterl = ledges.begin(); iterl != ledges.end(); iterl++)
	{
		std::cout << "Arista: " << (*iterl)->GetNodeSource()->GetId() << " <-> " << (*iterl)->GetNodeDest()->GetId() << std::endl;
		//Imprimimos atributos de una arista
		std::map<std::string, std::string> attrs = (*iterl)->GetAttrs();
		std::map<std::string, std::string>::iterator iter_attr;
		std::cout << "Atributos: ";
		for(iter_attr = attrs.begin(); iter_attr != attrs.end(); iter_attr++)
			std::cout << "[" << iter_attr->first << ": " << attrs[iter_attr->first] << "] ";
		std::cout << std::endl;
	}
	*/
	std::cout << std::endl;
}

/*
*	Método que genera un grafo de Ejemplo mediante las clases Graph, Node, Edge.
*/
void create_graph_example()
{
	//Creamos nodos

	Node *a = new Node("A");
	Attribute v1;
	v1.AsNumeric = 10;
	a->AddAttr("x", v1);
    Node *b = new Node("B");
	b->AddAttr("x", v1);
    Edge *e = new Edge(a,b);
	Graph *g = new Graph("ID", "A");

	//Node *v1 = e->GetNodeSource();
	//Node *v2 = e->GetNodeDest();
	g->AddNode("A", a);
	g->AddNode("B", b);
	g->AddEdge(e, "undirected");

	//imprimimos grafo
	print_graph_example(g);
	//Liberamos memoria
    //delete a,b,e,g;

}

double euclideanDistance(std::list<double> x, std::list<double> y)
{
	if(x.size() != y.size()) return -1;
	double s = 0;
	std::list<double>::iterator i,j;
	for(i = x.begin(), j = y.begin(); i != x.end(); i++,j++)
		s += pow((*i) - (*j), 2);

	return sqrt(s);
}

/**
 * readGraphFileXML. Método que lee un archivo GXL que contiene la información 
 * de un Grafo.
 * @param path: ruta en el que se encuentra el nombre del archivo
 * @param filename: nombre del archivo CXL.
 * @param cls: clase del grafo.
 * @param concatid: bandera para determinar si se concatena o no la variable id.
 * @param id: id a concantenar con el Id de grafo.
 * @return 
 */
Graph* readGraphFileXML(std::string path, std::string filename, std::string cls, bool concatid, std::string idconcat)
{
	Graph* g = NULL;
        std::string idgraph, edgemode, idnode;

	//Lectura de Grafo del archivo GXL
	pugi::xml_document doc;

	if (!doc.load_file((path + filename).c_str()))//Verificamos error de lectura
	{
		std::cout << "Error al leer el archivo" + filename << std::endl;
		return g;
	}

	//Lectura de grafos
	pugi::xml_node root_node = doc.child("gxl").child("graph");

	//Propiedades
	idgraph = root_node.attribute("id").as_string();
	edgemode = root_node.attribute("edgemode").as_string();

	//Concatenamos el nombre del archivo al id del grafo, esto para datasets como GREC
	if(concatid)
            idgraph += "-" + idconcat;
        
	//Creamos Grafo
	g = new Graph(idgraph, cls);

	//Recorremos los nodos y aristas del grafo
	for (pugi::xml_node node = root_node.first_child(); node; node = node.next_sibling())
	{
		//Extraemos nombre de la etiqueta para verificar si es nodo o arista
		std::string node_name = node.name();

		//Es un nodo
		if(node_name == "node")
		{
			idnode = node.attribute("id").value();

			Node *new_node = new Node(idnode);
			//Leemos los atributos del nodo
			for (pugi::xml_node attrib = node.first_child(); attrib; attrib = attrib.next_sibling())
			{
				//Identificamos el tipo de variable
				Attribute v;
				std::string name = attrib.first_child().name();

				if( name == "Integer" || name == "float")
				{
					v.AsNumeric = atof(attrib.first_child().child_value());
					v.AsString = attrib.first_child().child_value();
				}
				else
				{
					v.AsNumeric = 0;
					v.AsString = attrib.first_child().child_value();
				}
				//añadimos atributos al nodo
				new_node->AddAttr(attrib.attribute("name").value(), v);
			}
			//Añadimos nodo al grafo
			g->AddNode(idnode, new_node);
		}

		//Es una Arista
		if(node_name == "edge")
		{
			std::string node_source, node_dest;
			node_source = node.attribute("from").value();
			node_dest = node.attribute("to").value();
			//Creamos nueva arista
			Edge *new_edge = new Edge(g->GetNode(node_source),g->GetNode(node_dest));
			//Leemos las aristas
			for (pugi::xml_node attrib = node.first_child(); attrib; attrib = attrib.next_sibling())
			{
				Attribute v;
				std::string name = attrib.first_child().name();

				if(name == "Integer" || name == "float")
				{
					v.AsNumeric = atof(attrib.first_child().child_value());
					v.AsString = attrib.first_child().child_value();
				}
				else
				{
					v.AsNumeric = 0;
					v.AsString = attrib.first_child().child_value();
				}
				new_edge->AddAttr(attrib.attribute("name").value(), v);
			}
			g->AddEdge(new_edge, edgemode);
		}
	}

	return g;
}

/*
*	Método que lee el listado de grafos
*/
std::vector<Graph *> readlistgraphs(std::string path, std::string filename, std::string label, bool concatid)
{
	pugi::xml_document doc;
	std::string filegraph, cls, f;
	std::vector<Graph *> graphs;


	f = path + filename;

	//Leemos archivo XML
	if (!doc.load_file(f.c_str()))
	{
		std::cout<<"Error al leer el listado de grafos."<<std::endl;
		return graphs;
	}
	//Lectura de grafos
	pugi::xml_node root_node = doc.child("GraphCollection").child(label.c_str());
        int count = 1;
        
        

	for (pugi::xml_node node = root_node.first_child(); node; node = node.next_sibling())
	{
		filegraph = node.attribute("file").value();
		cls = node.attribute("class").value();
                //añadimos identificador si es necesario
                std::ostringstream convertid;
                convertid << count;
		//Leemos grafo de archivo GXL
		Graph *g = readGraphFileXML(path, filegraph, cls, concatid, convertid.str());
		graphs.push_back(g);
                count++;
	}

	return graphs;
}


#endif
