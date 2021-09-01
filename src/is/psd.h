#ifndef DISCRIMINATIVE_H_
#define DISCRIMINATIVE_H_

//
// Clase Prototype Selection Discriminative
//
class PSD
{
	private:
		double alpha;
		double tnode;
		double tedge;
		double w_substedge;
		bool print_details;
		std::string dbname;
		std::map<std::string,double> distances;
		std::vector<Instance> training;
		time_t start;
		time_t end;
		bool calculateGED;
		std::string fileout;
		std::vector<Graph*> lcps;
		std::vector<Graph*> lbps;
		std::vector<Graph*> lrps;
		std::vector<Graph*> lsps;
		std::vector<Graph*> ltps;

	public:
		PSD(std::map<std::string,double> distances, std::string fileout);
		PSD(std::string dbname, double alpha, double tnode, double tedge, double w_substedge, bool print_details, std::string fileout);
		~PSD(){}
		void execute(std::vector<Graph*> graphs_tr, float *, int);
		std::map<std::string, std::vector<Graph*> > graphsByClass(std::vector<Graph*> graphs);
		double Distance(Graph* g1, Graph* g2);
		std::map<std::string, std::map<std::string, Graph*> > CPS(std::map<std::string, std::vector<Graph*> > , float, float, int);
		std::map<std::string, std::map<std::string, Graph*> > BPS(std::map<std::string, std::vector<Graph*> > , float, float, int);
		std::map<std::string, std::map<std::string, Graph*> > RPS(std::map<std::string, std::vector<Graph*> > , float, float, int, std::map<std::string, std::map<std::string, Graph*> >);
		std::map<std::string, std::map<std::string, Graph*> > SPS(std::map<std::string, std::vector<Graph*> > graphsByCls, float Wc, float Ws, int n, std::map<std::string, std::map<std::string, Graph*> > prot_center);
		std::map<std::string, std::map<std::string, Graph*> > TPS(std::map<std::string, std::vector<Graph*> > graphsByCls, float Wc, float Ws, int n, std::map<std::string, std::map<std::string, Graph*> > prot_center, std::map<std::string, std::map<std::string, Graph*> > prot_border);
		void writeResult(std::vector<Graph*> indexes, std::string output, int size, float ret, time_t ends);
		std::vector<Graph*> getListProtFromMap(std::map<std::string, std::map<std::string, Graph*> > mspd);
		std::vector<Graph*> getCPS();
		std::vector<Graph*> getBPS();
		std::vector<Graph*> getRPS();
		std::vector<Graph*> getSPS();
		std::vector<Graph*> getTPS();
};

/**
 * Constructor utilizado cuando se leen las distancias de edición desde archivo.
 *
 */
PSD::PSD(std::map<std::string,double> distances, std::string fileout)
{
	this->distances = distances;
	this->alpha = 0;
	this->tnode = 0;
	this->tedge = 0;
	this->w_substedge = 0;
	this->print_details = false;
	this->dbname = "";
	this->start = this->end = 0;
	this->calculateGED = false;
	this->fileout = fileout;

}


/**
 * Constructor utilizado cuando se calculan las distancias de edición.
 *
 */
PSD::PSD(std::string dbname, double alpha, double tnode, double tedge, double w_substedge, bool print_details, std::string fileout)
{
	this->alpha = alpha;
	this->tnode = tnode;
	this->tedge = tedge;
	this->w_substedge = w_substedge;
	this->print_details = print_details;
	this->dbname = dbname;
	this->start = this->end = 0;
	this->calculateGED = true;
	this->fileout = fileout;
}

/**
 * Función para obtener el listado de grafos por Clase
 *
 */

std::map<std::string, std::vector<Graph*> > PSD::graphsByClass(std::vector<Graph*> graphs)
{
	int i;
	std::map<std::string, std::vector<Graph*> > graphsByCls;

    for(i = 0; i < (int)graphs.size(); i++)
		graphsByCls[graphs[i]->GetCls()].push_back(graphs[i]);

    return graphsByCls;
}

void PSD::writeResult(std::vector<Graph*> indexes, std::string output, int size, float ret, time_t ends)
{
	std::ofstream fileOut;
	fileOut.open( output.c_str() );

    int i;
    bool b = false;

    for(i = 0; i < size; i++)
    {
        fileOut << (b? ",":"") << indexes[i]->GetId();
        b = true;
    }
    fileOut << "\nTiempo: " << difftime(ends, start) << std::endl;
    fileOut << "\nRetención: " << ret << std::endl;

    fileOut.close();
}


double PSD::Distance(Graph* g1, Graph* g2)
{
    if(this->calculateGED)
        return compare(this->dbname, g1, g2, this->alpha, this->tnode, this->tedge, this->w_substedge, this->print_details);
    else if(this->distances.count(g1->GetId() + "-" + g2->GetId()))
        return this->distances[g1->GetId() + "-" + g2->GetId()];
    else if(this->distances.count(g2->GetId() + "-" + g1->GetId()))
        return this->distances[g2->GetId() + "-" + g1->GetId()];
    else
    {
        std::cout << "PSR: No se encuentra la distancia de edición con clave " << g1->GetId() + "-" + g2->GetId() << std::endl;
        return -1;
    }
}

//
// Funcion que ejecuta el algoritmo de selección de instancias
// Center Prototype Selection Discriminativo
//
std::map<std::string, std::map<std::string, Graph*> > PSD::CPS(std::map<std::string, std::vector<Graph*> > graphsByCls, float Wc, float Ws, int n)
{
	//Obtener el numero de clases
	std::map<std::string, std::map<std::string, Graph*> > prototypes;
	int i, j, argMin;
	double disMin;
	std::string idgraph;
	double a, b, d;

	//Obtenemos los grafos por clase
	std::map<std::string, std::vector<Graph*> >::iterator Cn, Cnn;

	//Iteramos para cada clase
	for(Cn = graphsByCls.begin(); Cn != graphsByCls.end(); Cn++)
	{
		//Seleccionamos n prototipos por clase
		for(int l = 0; l < n ; l++)
		{
			argMin = -1;
			disMin = 3.40e38;

			//Iteramos sobre el conjunto de grafos que pertenecen a la clase para obtener el prototipo minimo
			for(j = 0; j < (int)graphsByCls[Cn->first].size() ; j++)
			{
				//Si el prototipo no se ha seleccionado
				if( !prototypes[Cn->first].count( graphsByCls[Cn->first][j]->GetId() ) )
				{
					//Calculamos la sumatoria del centro con respecto a los elementos de la clase
					a = 0;
					for(i = 0; i < (int)graphsByCls[Cn->first].size(); i++)
						if(i != j)
							a += Distance(graphsByCls[Cn->first][j], graphsByCls[Cn->first][i]);

					//Calculamos la sumatoria con respecto a los grafos de las clases restantes
                    b = 0;
                    for(Cnn = graphsByCls.begin(); Cnn != graphsByCls.end(); Cnn++)
                    	if( Cn != Cnn)
                    		for(i = 0; i < (int)graphsByCls[Cnn->first].size(); i++)
                    			b += Distance(graphsByCls[Cn->first][j], graphsByCls[Cnn->first][i]);

                    //Distancia final
                    d = Wc * a - Ws * b;
                    //Actualizamos distancia, grafo minimo a seleccionar
                    if(d < disMin)
                    {
                    	argMin = j;
                    	disMin = d;
                    	idgraph = graphsByCls[Cn->first][j]->GetId();
                    }
				}
			}

			//Agregamos prototipo seleccionado
			prototypes[Cn->first][idgraph] = graphsByCls[Cn->first][argMin];
		}
	}

	return prototypes;
}

//
// Funcion que ejecuta el algoritmo de selección de instancias
// Border Prototype Selection Discriminativo
//
std::map<std::string, std::map<std::string, Graph*> > PSD::BPS(std::map<std::string, std::vector<Graph*> > graphsByCls, float Wc, float Ws, int n)
{
	//Obtener el numero de clases
	std::map<std::string, std::map<std::string, Graph*> > prototypes;
	int i, j, argMin;
	std::string idgraph;
	double a, b, d, disMin;

	//Obtenemos los grafos por clase
	std::map<std::string, std::vector<Graph*> >::iterator Cn, Cnn;

	//Iteramos para cada clase
	for(Cn = graphsByCls.begin(); Cn != graphsByCls.end(); Cn++)
	{
		//Seleccionamos n prototipos por clase
		for(int l = 0; l < n ; l++)
		{
			argMin = -1;
			disMin = 3.40e38;

			//Iteramos sobre el conjunto de grafos que pertenecen a la clase para obtener el prototipo minimo
			for(j = 0; j < (int)graphsByCls[Cn->first].size() ; j++)
			{
				//Si el prototipo no se ha seleccionado
				if( !prototypes[Cn->first].count( graphsByCls[Cn->first][j]->GetId() ) )
				{
					//Calculamos la sumatoria del centro con respecto a los elementos de la clase
					a = 0;
					for(i = 0; i < (int)graphsByCls[Cn->first].size(); i++)
						if(i != j)
							a += Distance(graphsByCls[Cn->first][j], graphsByCls[Cn->first][i]);

					//Calculamos la sumatoria con respecto a los grafos de las clases restantes
                    b = 0;
                    for(Cnn = graphsByCls.begin(); Cnn != graphsByCls.end(); Cnn++)
                    	if(Cn != Cnn)
                    		for(i = 0; i < (int)graphsByCls[Cnn->first].size(); i++)
                    			b += Distance(graphsByCls[Cn->first][j], graphsByCls[Cnn->first][i]);

                    //Distancia final
                    d = Wc * a + Ws * b;
                    //Actualizamos distancia, grafo minimo a seleccionar
                    if(d < disMin)
                    {
                    	argMin = j;
                    	disMin = d;
                    	idgraph = graphsByCls[Cn->first][j]->GetId();
                    }
				}
			}

			//Agregamos prototipo seleccionado
			prototypes[Cn->first][idgraph] = graphsByCls[Cn->first][argMin];

		}
	}

	return prototypes;
}


// *
// * Funcion que ejecuta el algoritmo de selección de instancias
// * Repelling Prototype Selection Discriminativo
// *
std::map<std::string, std::map<std::string, Graph*> > PSD::RPS(std::map<std::string, std::vector<Graph*> > graphsByCls, float Wc, float Ws, int n, std::map<std::string, std::map<std::string, Graph*> > cps)
{
	//Obtener el numero de clases

	std::map<std::string, std::map<std::string, Graph*> > prototypes;
	int i, j, argMin;
	std::string idgraph;
	double a, b, c, d, disMin;

	//Obtenemos los grafos por clase
	std::map<std::string, std::vector<Graph*> >::iterator Cn, Cnn;
	std::map<std::string, Graph*>::iterator x;
	std::map<std::string, Graph*>::iterator it;

	//Iteramos para cada clase
	for(Cn = graphsByCls.begin(); Cn != graphsByCls.end(); Cn++)
	{
		//Apuntamos al primer elemento en la clase Cn->first de los prototipos extraidos por CPS
		it = cps[Cn->first].begin();
		//Guardamos en prototypes[Clase][idgrafo] = "posicion del grafo en graphsByCls = it->second"
		prototypes[Cn->first][it->first] = it->second;

		//Seleccionamos n prototipos por clase
		for(int l = 0; l < (n-1) ; l++)
		{
			argMin = -1;
			disMin = 3.40e38;

			//Iteramos sobre el conjunto de grafos que pertenecen a la clase para obtener el prototipo minimo
			for(j = 0; j < (int)graphsByCls[Cn->first].size() ; j++)
			{
				//Si el prototipo no se ha seleccionado
				if( !prototypes[Cn->first].count( graphsByCls[Cn->first][j]->GetId() ) )
				{
					//Calculamos la sumatoria del centro con respecto a los elementos de la clase
					a = 0;
					for(i = 0; i < (int)graphsByCls[Cn->first].size(); i++)
						if(i != j)
							a += Distance(graphsByCls[Cn->first][j], graphsByCls[Cn->first][i]);


					//Calculamos la sumatoria con respecto a los grafos de las clases restantes
                    b = 0;
                    for(Cnn = graphsByCls.begin(); Cnn != graphsByCls.end(); Cnn++)
                    	if(Cn != Cnn)
                    		for(i = 0; i < (int)graphsByCls[Cnn->first].size(); i++)
                    			b += Distance(graphsByCls[Cn->first][j], graphsByCls[Cnn->first][i]);

                    //Calculamos la sumatoria con respecto a los grafos de las clases restantes
					c = 0;
					for(x = prototypes[Cn->first].begin(); x != prototypes[Cn->first].end(); x++)
						c += Distance(prototypes[Cn->first][x->first], graphsByCls[Cn->first][j]);

                    //Distancia final
                    d = Wc * a - Ws * (b * c);

                    //Actualizamos distancia, grafo minimo a seleccionar
                    if(d < disMin)
                    {
                    	argMin = j;
                    	disMin = d;
                    	idgraph = graphsByCls[Cn->first][j]->GetId();
                    }
				}
			}

			//Agregamos prototipo seleccionado
			prototypes[Cn->first][idgraph] = graphsByCls[Cn->first][argMin];
		}
	}

	return prototypes;
}


// *
// * Funcion que ejecuta el algoritmo de selección de instancias
// * Spanning Prototype Selection Discriminative
// *
std::map<std::string, std::map<std::string, Graph*> > PSD::SPS(std::map<std::string, std::vector<Graph*> > graphsByCls, float Wc, float Ws, int n, std::map<std::string, std::map<std::string, Graph*> > cps)
{
	//Obtener el numero de clases

	std::map<std::string, std::map<std::string, Graph*> > prototypes;
	int i, j, argMin;
	std::string idgraph;
	double a, b, d, disMin;


	//Obtenemos los grafos por clase
	std::map<std::string, std::vector<Graph*> >::iterator Cn, Cnn;
	std::map<std::string, Graph*>::iterator x;
	std::map<std::string, Graph*>::iterator it;

	//Iteramos para cada clase
	for(Cn = graphsByCls.begin(); Cn != graphsByCls.end(); Cn++)
	{
		//Apuntamos al primer elemento en la clase Cn->first de los prototipos extraidos por CPS
		it = cps[Cn->first].begin();
		//Guardamos en prototypes[Clase][idgrafo] = "posicion del grafo en graphsByCls = it->second"
		prototypes[Cn->first][it->first] = it->second;

		//Seleccionamos n prototipos por clase
		for(int l = 0; l < (n-1) ; l++)
		{
			argMin = -1;
			disMin = 3.40e38;

			//Iteramos sobre el conjunto de grafos que pertenecen a la clase para obtener el prototipo minimo
			for(j = 0; j < (int)graphsByCls[Cn->first].size() ; j++)
			{
				//Si el prototipo no se ha seleccionado
				if( !prototypes[Cn->first].count( graphsByCls[Cn->first][j]->GetId() ) )
				{
					//Calculamos la sumatoria del centro con respecto a los elementos de la clase
					a = 0;
					for(x = prototypes[Cn->first].begin(); x != prototypes[Cn->first].end(); x++)
						a += Distance(prototypes[Cn->first][x->first], graphsByCls[Cn->first][j]);

					//Calculamos la sumatoria con respecto a los grafos de las clases restantes
                    b = 0;
                    for(Cnn = graphsByCls.begin(); Cnn != graphsByCls.end(); Cnn++)
                    	if(Cn != Cnn)
                    		for(i = 0; i < (int)graphsByCls[Cnn->first].size(); i++)
                    			b += Distance(graphsByCls[Cn->first][j], graphsByCls[Cnn->first][i]);

                    //Distancia final
                    d = Wc * a + Ws * b;

                    //Actualizamos distancia, grafo minimo a seleccionar
                    if(d < disMin)
                    {
                    	argMin = j;
                    	disMin = d;
                    	idgraph = graphsByCls[Cn->first][j]->GetId();
                    }
				}
			}

			//Agregamos prototipo seleccionado
			prototypes[Cn->first][idgraph] = graphsByCls[Cn->first][argMin];
		}
	}

	return prototypes;
}


// *
// * Funcion que ejecuta el algoritmo de selección de instancias
// * Spanning Prototype Selection Discriminative
// *
std::map<std::string, std::map<std::string, Graph*> > PSD::TPS(std::map<std::string, std::vector<Graph*> > graphsByCls, float Wc, float Ws, int n, std::map<std::string, std::map<std::string, Graph*> > cps, std::map<std::string, std::map<std::string, Graph*> > bps)
{
	//Obtener el numero de clases

	std::map<std::string, std::map<std::string, Graph*> > prototypes;
	int i, j, argMin;
	std::string idgraph;
	double a, d, disMin, dgp, dMax;

	//Obtenemos los grafos por clase
	std::map<std::string, std::vector<Graph* > >::iterator Cn, Cnn;
	std::map<std::string, Graph*>::iterator x;

	//Copiamos los prototipos bordes de prot_border a prototypes
	std::map<std::string, Graph* >::iterator it_b, it_c;

	//Iteramos para cada clase
	for(Cn = graphsByCls.begin(); Cn != graphsByCls.end(); Cn++)
	{
		//Apuntamos al primer elemento en la clase Cn->first de los prototipos extraidos por CPS
		it_c = cps[Cn->first].begin();
		it_b = bps[Cn->first].begin();

		//Guardamos en prototypes[Clase][idgrafo] = "posicion del grafo en graphsByCls = it->second"
		prototypes[Cn->first][it_c->first] = it_c->second;
		prototypes[Cn->first][it_b->first] = it_b->second;

		//calculamos la distancia que existe entre estos dos prototipos
		dMax = Distance(prototypes[Cn->first][it_c->first], prototypes[Cn->first][it_b->first]);

		//Seleccionamos n prototipos por clase
		for(int l = 0; l < (n - 2) ; l++)
		{
			argMin = -1;
			disMin = 3.40e38;

			//Iteramos sobre el conjunto de grafos que pertenecen a la clase para obtener el prototipo minimo
			for(j = 0; j < (int)graphsByCls[Cn->first].size() ; j++)
			{
				//Si el prototipo no se ha seleccionado
				if( !prototypes[Cn->first].count( graphsByCls[Cn->first][j]->GetId() ) )
				{
					//Calculamos la sumatoria con respecto a los grafos de las clases restantes
                    a = 0;
                    for(Cnn = graphsByCls.begin(); Cnn != graphsByCls.end(); Cnn++)
                    	if(Cn != Cnn)
                    		for(i = 0; i < (int)graphsByCls[Cnn->first].size(); i++)
                    			a += Distance(graphsByCls[Cn->first][j], graphsByCls[Cnn->first][i]);

                    dgp = Distance(graphsByCls[Cn->first][j], prototypes[Cn->first][it_c->first]);

                    //Distancia final
                    d = Wc * std::abs( dgp - ((n+1) * (dMax/(n+2))) ) - Ws * a;

                    //Actualizamos distancia, grafo minimo a seleccionar
                    if(d < disMin)
                    {
                    	argMin = j;
                    	disMin = d;
                    	idgraph = graphsByCls[Cn->first][j]->GetId();
                    }
				}
			}

			//Agregamos prototipo seleccionado
			prototypes[Cn->first][idgraph] = graphsByCls[Cn->first][argMin];
		}
	}

	return prototypes;
}


std::vector<Graph*> PSD::getListProtFromMap(std::map<std::string, std::map<std::string, Graph*> > spd)
{
	std::vector<Graph*> lspd;
	std::map<std::string, std::map<std::string, Graph*> >::iterator i;
	std::map<std::string, Graph*>::iterator j;

	for(i = spd.begin(); i != spd.end(); i++)
	{
		for(j = spd[i->first].begin(); j != spd[i->first].end() ; j++)
		{
			lspd.push_back( spd[i->first][j->first] );
		}
	}

	return lspd;
}

//Métodos Get para obtener listado de prototipos discriminativos
std::vector<Graph*> PSD::getCPS()
{
	return this->lcps;
}

std::vector<Graph*> PSD::getBPS()
{
	return this->lbps;
}

std::vector<Graph*> PSD::getRPS()
{
	return this->lrps;
}

std::vector<Graph*> PSD::getSPS()
{
	return this->lsps;
}

std::vector<Graph*> PSD::getTPS()
{
	return this->ltps;
}


void PSD::execute(std::vector<Graph*> graphs_tr, float *Ws, int n)
{
	std::map<std::string, std::vector<Graph*> > graphsByCls = this->graphsByClass(graphs_tr);

	//Iniciamos midiendo tiempo
	this->start = time(NULL);

	//Generamos Center Prototype Selection
	//std::map<std::string, std::map<std::string, Graph*> > cps = CPS(graphsByCls, 1 - Ws[0], Ws[0], n);
	//Generamos Border Prototype Selection
	//std::map<std::string, std::map<std::string, Graph*> > bps = BPS(graphsByCls, 1 - Ws[1], Ws[1], n);

	//Volvemos a generar Center Prototype Selection pero con solo 1 elemento
	//Es algo repetitivo pero servirá para medir el tiempo de selección junto a RPS,SPS y TPS
	std::map<std::string, std::map<std::string, Graph*> > inicps = CPS(graphsByCls, 1 - Ws[0], Ws[0], 1);

	//Calculamos Repelliing Prototype Selection
	//std::map<std::string, std::map<std::string, Graph*> > rps = RPS(graphsByCls, 1 - Ws[2], Ws[2], n, inicps);

	//Calculamos Selection Prototype Selection
	std::map<std::string, std::map<std::string, Graph*> > sps = SPS(graphsByCls, 1 - Ws[3], Ws[3], n, inicps);

	//Generamos Border Prototype Selection calculando 1 solo elemento por clase
	//Es algo repetitivo pero servirá para medir el tiempo de selección de TPS
	//std::map<std::string, std::map<std::string, Graph*> > inibps = BPS(graphsByCls, 1 - Ws[1], Ws[1], 1);

	//Calculamos Target Prototype Selection
	//std::map<std::string, std::map<std::string, Graph*> > tps = TPS(graphsByCls, 1 - Ws[4], Ws[4], n, inicps, inibps);

	//Obtenemos el listado de grafos seleccionados
	//this->lcps = this->getListProtFromMap(cps);
	//this->lbps = this->getListProtFromMap(bps);
	//this->lrps = this->getListProtFromMap(rps);
	this->lsps = this->getListProtFromMap(sps);
	//this->ltps = this->getListProtFromMap(tps);

    //Tiempo final del calculo
    this->end = time(NULL);
    //Calculamos el porcentaje de retencion
    //float percentRetCPS = (float)(((float)this->lcps.size()/(float)(graphs_tr.size()))*100);
    //float percentRetBPS = (float)(((float)this->lbps.size()/(float)(graphs_tr.size()))*100);
    //float percentRetRPS = (float)(((float)this->lrps.size()/(float)(graphs_tr.size()))*100);
    float percentRetSPS = (float)(((float)this->lsps.size()/(float)(graphs_tr.size()))*100);
    //float percentRetTPS = (float)(((float)this->ltps.size()/(float)(graphs_tr.size()))*100);

    //Escribimos en archivo los prototipos seleccionados
    //this->writeResult(this->lcps, this->fileout + "-cps.txt", this->lcps.size(), percentRetCPS, this->end);
    //this->writeResult(this->lbps, this->fileout + "-bps.txt", this->lbps.size(), percentRetBPS, this->end);
    //this->writeResult(this->lrps, this->fileout + "-rps.txt", this->lrps.size(), percentRetRPS, this->end);
    this->writeResult(this->lsps, this->fileout + "-sps.txt", this->lsps.size(), percentRetSPS, this->end);
    //this->writeResult(this->ltps, this->fileout + "-tps.txt", this->ltps.size(), percentRetTPS, this->end);

}

#endif
