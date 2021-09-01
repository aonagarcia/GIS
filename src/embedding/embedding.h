#ifndef EMBEDDING_H_
#define EMBEDDING_H_

class GraphEmbedding
{
	private:
		std::vector<Graph*> graphs_tr;
		std::vector<int> prototypes;
		std::vector<std::vector<double> > X;
		std::vector<std::string> Y;
		std::map<std::string,double> distances;
	public:
		GraphEmbedding() {}
		void execute(std::vector<Graph*> graphs_tr, std::vector<int> prototypes);
		double Distance(std::string idgr1, std::string idgr2);
};

double GraphEmbedding::Distance(std::string idgr1, std::string idgr2)
{
	return this->distances[idgr1 + "-" + idgr2];
}


void GraphEmbedding::execute(std::vector<Graph*> graphs_tr, std::vector<int> prototypes)
{
	this->X.clear();
	this->Y.clear();

    for(int i = 0; i < (int)this->graphs_tr.size() ; i++)
    {
    	//Creamos una instancia para almacenar la disimilaridad
    	std::vector<double> instance;
    	//Calculamos las disimilaridades o atributos
    	for(int j = 0; j < (int)this->prototypes.size() ; j++)
    		instance.push_back(Distance(this->graphs_tr[i]->GetId(), this->graphs_tr[this->prototypes[j]]->GetId()));

    	//almacenamos instancia y clase de la instancia
    	this->X.push_back(instance);
		this->Y.push_back(this->graphs_tr[i]->GetCls());
	}
}

#endif
