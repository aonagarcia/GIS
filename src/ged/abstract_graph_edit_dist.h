
#ifndef ABSTRACT_GRAPH_EDIT_DIST_H_
#define ABSTRACT_GRAPH_EDIT_DIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include "../graph/graph.h"
//#include "hungarian_2/asp.h"
#include "lapjv/lap.h"
#define DBL_MAX 1000

 
class AbstractGraphEditDistance {
	protected:
		//Propiedades
		Graph *graph_source;
		Graph *graph_dest;
		double alpha;
		double weigth_insdel_node;
		double weigth_insdel_edge;
		double weigth_subst_edge;
		int tam_cost_matrix;
		//Métodos
		double distance();
		double** create_cost_matrix();
		virtual double insert_cost(int i, int j) = 0;
		virtual double delete_cost(int i, int j) = 0;
		virtual double substitute_cost(Node* node1, Node* node2) = 0;
	public:
		AbstractGraphEditDistance(Graph*, Graph*,double , double, double, double);
		virtual ~AbstractGraphEditDistance(){}
		void print_matrix();
		double normalized_distance();
};

AbstractGraphEditDistance::AbstractGraphEditDistance(Graph *g_source, Graph *g_dest, double _alpha, double _weigth_insdel_node, double _weigth_insdel_edge, double _weigth_subst_edge) 
{
	this->graph_source = g_source;
	this->graph_dest = g_dest;
	this->alpha = _alpha;
	this->weigth_insdel_node = _weigth_insdel_node;
	this->weigth_insdel_edge = _weigth_insdel_edge;
	this->weigth_subst_edge = _weigth_subst_edge;
	this->tam_cost_matrix = 0;
}

double AbstractGraphEditDistance::normalized_distance()
{
	double avg_graph_len = (double)(graph_source->GetNumNodes() + graph_dest->GetNumNodes())/2;
	double dist = distance();

	if(avg_graph_len != 0)
		return dist/avg_graph_len;
	return 0.0;
}

double AbstractGraphEditDistance::distance()
{
	double **cost_matrix = create_cost_matrix();

	if(tam_cost_matrix > 0)
	{

		//long *rowsol;
		//long *colsol;
		//cost *u,*v;
		//rowsol = (long *)calloc(tam_cost_matrix, sizeof(long));
		//colsol = (long *)calloc(tam_cost_matrix, sizeof(long));
		//u = (cost *)malloc(sizeof(cost)*tam_cost_matrix);
		//v = (cost *)malloc(sizeof(cost)*tam_cost_matrix);
		//aplicar Hungarian
		//double x = asp(tam_cost_matrix, cost_matrix, colsol, rowsol);
		//delete cost_matrix;
		//free(cost_matrix);
		//return x;

		col *rowsol;
		row *colsol;
		cost *u,*v;
		rowsol = (col *)malloc(sizeof(col)*tam_cost_matrix);
		colsol = (row *)malloc(sizeof(row)*tam_cost_matrix);
		u = (cost *)malloc(sizeof(cost)*tam_cost_matrix);
		v = (cost *)malloc(sizeof(cost)*tam_cost_matrix);

		double x = lap(tam_cost_matrix, cost_matrix,rowsol,colsol,u,v);
		// free the memory of calloc
	  	for (int i=0; i<tam_cost_matrix; i++)
			delete cost_matrix[i];
		delete cost_matrix;

		return x;

	}

	return 0;
}

double** AbstractGraphEditDistance::create_cost_matrix()
{    
	// Crea una |N+M| X |N+M| matriz de costo entre todos los nodos de los grafos g1 y g2
	// Cada costo representa el costo de sustituir, eliminar o insertar un nodo
	// La matriz de costo consiste en 4 regiones:

	// sustituciones 	    |     costos por insertar
	// -------------------------------
	// costo por eliminar 	|     eliminar -> eliminar

	// La region eliminar -> eliminar es rellenada con ceros
        
	int n = graph_source->GetNumNodes();
	int m = graph_dest->GetNumNodes();
	int i,j;

	//std::cout<<"Inicio:"<<std::endl;
	tam_cost_matrix = n + m;
	
	//Creamos matriz de costos
	double** cost_matrix;
    cost_matrix = (double**) calloc(tam_cost_matrix, sizeof(double * ));
	
    for (i = 0; i < tam_cost_matrix; i++)
	{
        cost_matrix[i] = (double*) calloc(tam_cost_matrix,sizeof(double));
		for (j = 0; j < tam_cost_matrix; j++)
			cost_matrix[i][j] = 0;
	}

	std::map<std::string, Node*> nodes1 = graph_source-> GetNodes();
	std::map<std::string, Node*> nodes2 = graph_dest-> GetNodes();
	std::map<std::string, Node*>::iterator iter_nodes1, iter_nodes2;

	//Costos de sustitución
	i = 0;
	for (iter_nodes1 = nodes1.begin(); iter_nodes1 != nodes1.end(); iter_nodes1++)
	{
		j = 0;
		for (iter_nodes2 = nodes2.begin(); iter_nodes2 != nodes2.end(); iter_nodes2++)
		{
			cost_matrix[i][j] = substitute_cost(nodes1[iter_nodes1->first], nodes2[iter_nodes2->first]);
			j++;
		}
		i++;
	}


	//Costos de inserción en el Grafo destino
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			cost_matrix[i+n][j] = insert_cost(i, j);

	//Costos de eliminacion en el Grafo origen
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			cost_matrix[j][i+m] = delete_cost(i, j);


	//std::cout<<"finish calculate subst"<<std::endl;

	return cost_matrix;
}


void AbstractGraphEditDistance::print_matrix()
{
	std::cout << "Matriz de costo:" << std::endl;
	double **cost_matrix = create_cost_matrix();
	//cout << setprecision(2) << fixed;
	for(int i=0; i < tam_cost_matrix; i++)
	{
		for(int j = 0; j < tam_cost_matrix; j++)
			if (cost_matrix[i][j] >= DBL_MAX)
				std::cout << "inf\t";
			else
				printf("%.2f\t",cost_matrix[i][j]);
		std::cout << std::endl;
	}

	free(cost_matrix);
}

#endif
