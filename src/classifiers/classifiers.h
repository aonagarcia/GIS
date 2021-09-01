#ifndef CLASSIFIERS_H_
#define CLASSIFIERS_H_

#include <algorithm>
#include <vector>
#include "../ged/ged.h"

struct Pair
{
	double d;
	std::string cls;
	Pair(double d, std::string cls)
	{
		this->d = d;
		this->cls = cls;
	}
};

struct Pair_compare
{
	bool operator()(Pair const &left, Pair const &right) {
		return left.d < right.d;
    }
};

class KNN {
	private:
            double alpha;
            double tnode;
            double tedge;
            double w_substedge;
            bool print_details;
            int errors;
            std::string dbname;
            std::map<std::string, double> distances;
            bool classifyWithDistances;
            double accuracy;

	public:
            KNN(double alpha, double tnode, double tedge, double w_substedge, bool print_details, std::string dbname);
            KNN(std::map<std::string, double> distances);
            ~KNN() {}
            double Distance(std::string database, Graph *g1, Graph *g2);
            std::string classify(std::string, std::vector<Graph*> Graphs_Tr, Graph *newgraph, int K);
            void test(std::string,std::vector<Graph*>, std::vector<Graph*>, int);
            double getAccuracy();
            double getErrors();
};

//Constructor
KNN::KNN(double alpha, double tnode, double tedge, double w_substedge, bool print_details, std::string dbname)
{
    this->alpha = alpha;
    this->tnode = tnode;
    this->tedge = tedge;
    this->w_substedge = w_substedge;
    this->print_details = print_details;
    this->dbname = dbname;
    this->errors = 0;
    this->classifyWithDistances = false;
    this->accuracy = 0;
}

KNN::KNN(std::map<std::string, double> distances)
{
    this->alpha = 0;
    this->tnode = 0;
    this->tedge = 0;
    this->w_substedge = 0;
    this->print_details = false;
    this->dbname = "none";
    this->distances = distances;
    this->errors = 0;
    this->classifyWithDistances = true;
    this->accuracy = 0;
}

double KNN::Distance(std::string database, Graph *g1, Graph *g2)
{
    if(this->classifyWithDistances)
    {
        if(this->distances.count(g1->GetId() + "-" + g2->GetId()))
            return this->distances[g1->GetId() + "-" + g2->GetId()];
        else if(this->distances.count(g2->GetId() + "-" + g1->GetId()))
            return this->distances[g2->GetId() + "-" + g1->GetId()];
        else
        {
            std::cout << "La distancia no ha sido encontrada con clave: " << g1->GetId() + "-" + g2->GetId() << std::endl;
            return -1;
        }
    }
    else
        return compare(database, g1, g2, this->alpha, this->tnode, this->tedge, this->w_substedge, false);
}


double KNN::getErrors()
{
    return this->errors;
}


double KNN::getAccuracy()
{
    return this->accuracy;
}

std::string KNN::classify(std::string database, std::vector<Graph*> Graphs_Tr, Graph *newgraph, int K)
{
    std::vector<Pair> U;
    std::vector<Graph*>::iterator i;
    int c = 1;
    double d;
    std::string cls;

    //Calculamos la distancia mas cercana de cada instancia con respecto a ejemplo a clasificar
    for(i = Graphs_Tr.begin(); i != Graphs_Tr.end(); i++)
    {
        d = this->Distance(database, (*i), newgraph);
        U.push_back(Pair(d, (*i)->GetCls()));
        //std::cout << "\t" << c << ". d(" + (*i)->GetId() << "," << newgraph->GetId() << ") = " << d << std::endl;
        c++;
    }

    std::map<std::string,int> ir;
    //ordenamos y obtenemos los K vecinos mas cercanos
    std::sort(U.begin(),U.end(),Pair_compare());
    int n = U.size();
    std::map<std::string,int>::iterator x;

    for(int j = 0; j < K && j < n; j++)
    {
            x = ir.find(U[j].cls);
            if(x == ir.end())
                    ir[U[j].cls] = 0;
            ir[U[j].cls]++;
    }
    //Elemento maximo
    std::map<std::string,int>::iterator it;
    double max = -DBL_MAX;
    for (it = ir.begin(); it != ir.end(); it++)
    {
        if(ir[it->first] > max)
        {
            max = ir[it->first];
            cls = it->first;
        }
    }

    ir.clear();
    U.clear();
	
    return  cls;
}

void KNN::test(std::string database, std::vector<Graph*> Graphs_Tr, std::vector<Graph*> Graphs_Ts, int K)
{
	errors = 0;
	std::vector<Graph*>::iterator i;
	int c = 1;
	std::string cls;
        
        if(this->print_details)
            std::cout << "K-NN sobre " << database << " con K=" <<K<<std::endl;
        
	for(i = Graphs_Ts.begin(); i != Graphs_Ts.end(); i++, c++)
	{
            cls = classify(database, Graphs_Tr, (*i), K);
            if((*i)->GetCls() != cls)
                errors++;
            
            if(this->print_details)
                std::cout << c << ". Graph(" << (*i)->GetId() << ") = " << (*i)->GetCls() << "  result k-NN =" << cls << " Error:" << errors << " No. nodes: " << (*i)->GetNumNodes() << " No. edges: " << (*i)->GetNumEdges() << std::endl;
	}
        
        //Calculamos accuracy
        this->accuracy = (1 - (errors/(double)Graphs_Ts.size()))*100;
        
        if(this->print_details)
            //Imprimimos accuracy y errores.
            printf("Errores: %d Acc.: %.2f\n", errors, accuracy);
}

#endif
