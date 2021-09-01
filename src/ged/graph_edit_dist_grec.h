#ifndef GRAPH_EDIT_DIST_GREC_H_
#define GRAPH_EDIT_DIST_GREC_H_

#include "abstract_graph_edit_dist.h"
#include <sstream>
#include <math.h>


class GraphEditDistanceGrec: public AbstractGraphEditDistance
{
	public:
		GraphEditDistanceGrec(Graph *g_source, Graph *g_dest, double _alpha, double _weigth_insdel_node, double _weigth_insdel_edge, double _weigth_subst_edge): AbstractGraphEditDistance(g_source, g_dest, _alpha, _weigth_insdel_node, _weigth_insdel_edge, _weigth_subst_edge) {}
		~GraphEditDistanceGrec(){}

	protected:
		double substitute_cost(Node* node1, Node* node2);
		double insert_cost(int i, int j);
		double delete_cost(int i, int j);
		double relabel_cost(Node* node1, Node* node2);
		double edge_diff(Node *node1, Node *node2);
		double insert_edge_cost(int i, int j);
		double delete_edge_cost(int i, int j);
		double substitute_edge_cost(std::string, std::string);
		double calculate_edge_distance(Node*, Node*);
};

double GraphEditDistanceGrec::substitute_cost(Node* node1, Node* node2)
{
	double cost_node = relabel_cost(node1, node2);
    double cost_edge = edge_diff(node1, node2);
    return cost_node + cost_edge;
}

double GraphEditDistanceGrec::relabel_cost(Node *node1, Node *node2)
{
	std::list<double> a, b;

	if(node1->GetAttr("type").AsString == node2->GetAttr("type").AsString){
		a.push_front(node1->GetAttr("x").AsNumeric);
		a.push_front(node1->GetAttr("y").AsNumeric);
		b.push_front(node2->GetAttr("x").AsNumeric);
		b.push_front(node2->GetAttr("y").AsNumeric);

		double d = euclideanDistance(a, b);
		a.clear();
		b.clear();

		return d;
	}

	return 2* this->weigth_insdel_node;
}

double GraphEditDistanceGrec::delete_cost(int i, int j)
{
    if(i == j)
        return weigth_insdel_node * alpha;
    return DBL_MAX;
}

double GraphEditDistanceGrec::insert_cost(int i, int j)
{
	if(i == j)
        return weigth_insdel_node * alpha;
	return DBL_MAX;
}

double GraphEditDistanceGrec::substitute_edge_cost(std::string type1, std::string type2)
{
	if(type1 == type2)
		return 0;
	return 2 * weigth_subst_edge * (1 - alpha);
}

double GraphEditDistanceGrec::insert_edge_cost(int i, int j)
{
    if(i == j)
		return this->weigth_insdel_edge * (1 - alpha);
	return DBL_MAX;
}

double GraphEditDistanceGrec::delete_edge_cost(int i, int j)
{
    if(i == j)
		return this->weigth_insdel_edge * (1 - alpha);
	return DBL_MAX;
}

double GraphEditDistanceGrec::edge_diff(Node *node1, Node *node2)
{
	if(node1->GetListAdj().size() == 0 || node2->GetListAdj().size() == 0)
		return (double)std::max(node1->GetListAdj().size(), node2->GetListAdj().size()) * weigth_insdel_edge;

	return calculate_edge_distance(node1, node2);
}

double GraphEditDistanceGrec::calculate_edge_distance(Node* node1, Node* node2)
{
	std::string id_source_node1 = node1->GetId();
	std::string id_source_node2 = node2->GetId();
	std::list<Node*> Nodes1 = node1->GetListAdj();
	std::list<Node*> Nodes2 = node2->GetListAdj();

	int n = Nodes1.size();
	int m = Nodes2.size();
	int i,j;

	int dim = n + m;
	double d = 0;

	//Creamos matriz de costos
	double** cost_matrix;
	cost_matrix = (double**) calloc(dim, sizeof(double * ));

	for (i = 0; i < dim; i++)
	{
		cost_matrix[i] = (double*) calloc(dim,sizeof(double));
		for (j = 0; j < dim; j++)
			cost_matrix[i][j] = 0;
	}

	std::list<Node*>::iterator iter_nodes1, iter_nodes2;

	//Costos de sustitución
	i = 0;
	for (iter_nodes1 = Nodes1.begin(); iter_nodes1 != Nodes1.end(); iter_nodes1++)
	{
		j = 0;
		for (iter_nodes2 = Nodes2.begin(); iter_nodes2 != Nodes2.end(); iter_nodes2++)
		{
			cost_matrix[i][j] = substitute_edge_cost(this->graph_source->GetEdge(id_source_node1+"-"+(*iter_nodes1)->GetId())->GetAttr("type0").AsString, this->graph_dest->GetEdge(id_source_node2+"-"+(*iter_nodes2)->GetId())->GetAttr("type0").AsString);
			j++;
		}
		i++;
	}

	//Costos de inserción en el Grafo destino
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			cost_matrix[i+n][j] = insert_edge_cost(i, j);

	//Costos de eliminacion en el Grafo origen
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			cost_matrix[j][i+m] = delete_edge_cost(i, j);

	col *rowsol;
	row *colsol;
	cost *u,*v;
	rowsol = (col *)malloc(sizeof(col)*dim);
	colsol = (row *)malloc(sizeof(row)*dim);
	u = (cost *)malloc(sizeof(cost)*dim);
	v = (cost *)malloc(sizeof(cost)*dim);

	d = lap(dim, cost_matrix,rowsol,colsol,u,v);
	d /= ((double)dim/2);

	// free the memory of calloc
	for (i=0; i < dim; i++)
		delete cost_matrix[i];
	delete cost_matrix;

	return d;

}

#endif
