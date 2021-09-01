#ifndef GRAPH_EDIT_DIST_PROTEIN_H_
#define GRAPH_EDIT_DIST_PROTEIN_H_

#include "abstract_graph_edit_dist.h"
#include <sstream>
#include <string.h>
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

class GraphEditDistanceProtein: public AbstractGraphEditDistance
{
	public:
		GraphEditDistanceProtein(Graph *g_source, Graph *g_dest, double _alpha, double _weigth_insdel_node, double _weigth_insdel_edge, double _weigth_subst_edge): AbstractGraphEditDistance(g_source, g_dest, _alpha, _weigth_insdel_node, _weigth_insdel_edge, _weigth_subst_edge) {}
		~GraphEditDistanceProtein(){}

	protected:
		double substitute_cost(Node* node1, Node* node2);
		double insert_cost(int i, int j);
		double delete_cost(int i, int j);
		double relabel_cost(Node* node1, Node* node2);
		double edge_diff(Node *node1, Node *node2);
		double insert_edge_cost(int i, int j);
		double delete_edge_cost(int i, int j);
		double substitute_edge_cost(Edge*,Edge*);
		double calculate_edge_distance(Node*, Node*);
		int levenshtein(const char *s1, const char *s2);
};

double GraphEditDistanceProtein::substitute_cost(Node* node1, Node* node2)
{
	double cost_node = relabel_cost(node1, node2);// * alpha;
    double cost_edge = edge_diff(node1, node2);// * (1 - alpha);
    return cost_node + cost_edge;
}

double GraphEditDistanceProtein::relabel_cost(Node *node1, Node *node2)
{
    if (node1->GetAttr("type").AsString == node2->GetAttr("type").AsString)
    {
    	double d = (double)this->levenshtein(node1->GetAttr("sequence").AsString.c_str(), node2->GetAttr("sequence").AsString.c_str());
    	//std::cout<<d<<std::endl;
        return d;
    }
	return 2 * this->weigth_insdel_node;
}

double GraphEditDistanceProtein::delete_cost(int i, int j)
{
    if(i == j)
        return weigth_insdel_node * alpha;
    return DBL_MAX;
}

double GraphEditDistanceProtein::insert_cost(int i, int j)
{
	if(i == j)
        return weigth_insdel_node * alpha;
	return DBL_MAX;
}

double GraphEditDistanceProtein::substitute_edge_cost(Edge* edge1, Edge* edge2)
{
	if(edge1->GetAttr("type0").AsString == edge2->GetAttr("type0").AsString)
		return 0;
	return 2 * this->weigth_subst_edge * (1 - this->alpha);

}

double GraphEditDistanceProtein::insert_edge_cost(int i, int j)
{
    if(i == j)
		return weigth_insdel_edge * (1 - alpha);
	return DBL_MAX;
}

double GraphEditDistanceProtein::delete_edge_cost(int i, int j)
{
    if(i == j)
		return weigth_insdel_edge * (1 - alpha);
	return DBL_MAX;
}

double GraphEditDistanceProtein::edge_diff(Node *node1, Node *node2)
{
	if(node1->GetListAdj().size() == 0 || node2->GetListAdj().size() == 0)
		return (double)std::max(node1->GetListAdj().size(), node2->GetListAdj().size()) * weigth_insdel_edge;

	return calculate_edge_distance(node1, node2);
}

double GraphEditDistanceProtein::calculate_edge_distance(Node* node1, Node* node2)
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
			cost_matrix[i][j] = substitute_edge_cost(this->graph_source->GetEdge(id_source_node1+"-"+(*iter_nodes1)->GetId()), this->graph_dest->GetEdge(id_source_node2+"-"+(*iter_nodes2)->GetId()));
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
	//std::cout << "d: " << d <<std::endl;
	return d;

}

int GraphEditDistanceProtein::levenshtein(const char *s1, const char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

#endif
