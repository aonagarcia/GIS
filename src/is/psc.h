#ifndef PSC_H_
#define PSC_H_

#include <unistd.h>

typedef struct{
	std::string id;
    int index;
    std::string cls;
    int idcluster;
    float r1;
}InstanceGraph;

typedef struct{
    int size;
    InstanceGraph representative;
    int *instances;
}Cluster;


class PSC {
	private:
		double alpha;
		double tnode;
		double tedge;
		double w_substedge;
		bool print_details;
		std::string dbname;
		std::vector<Graph*> graphs_tr;
		std::map<std::string, double> distances;
		std::vector<InstanceGraph> training;
		std::vector<int> PSC1;
		std::vector<Cluster> clusters;
		int tcls;
		int *classes, *maximize;
		int tSelected, nInst;
		float percentRet;
		time_t start, end;
		std::string fileout;
		bool calculateGED;
                int numClusters;

		std::vector<Cluster> randomRepresentatives(int k);
		void calculateR(int k);
		void assign(int k);
		float R1(int index_cluster, int k);
		float R2(int index_cluster);
		float R3(int index, float r2);
		double Distance(Graph* g1, Graph* g2);
		float similarity(Graph* g1, Graph* g2);
		float R4(int index, int k);
		int* maximizeInst(int k);
		int isHomogeneous(Cluster* c);
		int occurCluster(Cluster* c, std::string clss);
		int Oc(Cluster* clus, std::string mcls, InstanceGraph* gr);
		int Om(Cluster* clus, InstanceGraph* c);
		int isInto(int dat, std::vector<int> array);

	public:
		PSC(std::map<std::string, double> distances, std::string fileout);
		PSC(std::string dbname, double alpha, double tnode, double tedge, double w_substedge, bool print_details,std::string fileout);
		~PSC() {}
		std::map<std::string,int> occurclasses();
		void execute(std::vector<Graph*> graphs, int k);
		void KMSF(int k);
		void writeResult(std::vector<int> indexes, std::string output, int size, float ret, time_t ends);
		std::vector<int> getListPSC();
                float getRetPSC();
};


PSC::PSC(std::map<std::string, double> distances, std::string fileout)
{
	this->distances = distances;
	this->alpha = 0;
	this->tnode = 0;
	this->tedge = 0;
	this->w_substedge = 0;
	this->print_details = false;
	this->dbname = "";
	this->tcls = 0;
	this->tSelected = 0;
	this->percentRet = 0;
	this->classes = NULL;
	this->maximize = NULL;
	this->start = this->end = 0;
	this->calculateGED = false;
	this->fileout = fileout;

}

PSC::PSC(std::string dbname, double alpha, double tnode, double tedge, double w_substedge, bool print_details,std::string fileout)
{
	this->graphs_tr = graphs_tr;
	this->alpha = alpha;
	this->tnode = tnode;
	this->tedge = tedge;
	this->w_substedge = w_substedge;
	this->print_details = print_details;
	this->dbname = dbname;
	this->nInst = graphs_tr.size();
	this->tcls = 0;
	this->tSelected = 0;
	this->percentRet = 0;
	this->classes = NULL;
	this->maximize = NULL;
	this->start = this->end = 0;
	this->calculateGED = true;
	this->fileout = fileout;
}

///*
// * Calcula las ocurrencias de cada una de las clases
// */
std::map<std::string,int> PSC::occurclasses()
{
	int i;
	std::map<std::string,int> occur_classes;

    for(i = 0; i < (int)this->graphs_tr.size(); i++)
    {
    	if(!occur_classes.count(this->graphs_tr[i]->GetCls()))
    		occur_classes[this->graphs_tr[i]->GetCls()] = 0;
    	occur_classes[this->graphs_tr[i]->GetCls()]++;
        
    	InstanceGraph gr;
    	gr.id = this->graphs_tr[i]->GetId();
    	gr.index = i;
    	gr.cls = this->graphs_tr[i]->GetCls();

    	this->training.push_back(gr);
    }

    return occur_classes;
}

void PSC::writeResult(std::vector<int> indexes, std::string output, int size, float ret, time_t ends)
{
	std::ofstream fileOut;
	fileOut.open( output.c_str() );

    int i;
    bool b = false;

    for(i = 0; i < size; i++)
    {
        fileOut << (b? ",":"") << indexes[i];
        b = true;
    }
    fileOut << "\nTiempo: " << difftime(ends, start) << std::endl;
    fileOut << "\nRetención: " << ret << std::endl;

    fileOut.close();
}

int PSC::isInto(int dat, std::vector<int> array)
{
    int i;
    for(i = 0; i < (int)array.size(); i++)
    {
        if(array[i] == dat)
            return 1;
    }
    return 0;
}


std::vector<int> PSC::getListPSC()
{
	return this->PSC1;
}

float PSC::getRetPSC()
{
	return this->percentRet;
}


int isIntoCluster(InstanceGraph* index, int cluster)
{
    if(cluster == index->idcluster)
        return 1;    
    return 0;
}

int PSC::isHomogeneous(Cluster* c)
{
    int i;
    for(i = 1; i < c->size; i++)
        if(this->training[c->instances[i-1]].cls != this->training[c->instances[i]].cls)
            return 0;
    return 1;
}

int PSC::occurCluster(Cluster* c, std::string cls)
{
    int count = 0, i;
    for(i = 0; i < c->size; i++)
        if(this->training[c->instances[i]].cls == cls)
            count++;
    return count;
}

/*
 * Distancia de edición en Grafos
 */
double PSC::Distance(Graph* g1, Graph* g2)
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

float PSC::similarity(Graph* g1, Graph* g2)
{
    //Número de nodos máximo
    //float m = 9; //Letter
    return 1 - ((float)Distance(g1, g2)/((float)this->numClusters));
	//return Distance(g1, g2);
}

///*----------------------------------*/
///*K-MEANS SIMILARITY FUNCTIONS Clustering Algorithm*/
float PSC::R4(int index, int k)
{
    int i;
    float aux = 0;
    for(i = 0; i < k; i++)
    {
        if(clusters[i].representative.index != index) //revisar el index y el i
            aux += 1 - similarity(this->graphs_tr[index], this->graphs_tr[clusters[i].representative.index]);
    }
    return aux;
}

/*
 * Calcula la segunda función de la formula de KMSF
 * misma que es el promedio de similaridad de Oj con los otros objetos
 * en el mismo cluster
 */
float PSC::R2(int index)
{
    int i, idcluster;
    float aux = 0;

    idcluster = training[index].idcluster;

    if(clusters[idcluster].size == 0)
		return 0;

    for(i = 0; i < clusters[idcluster].size; i++)
    {
    	//De la formula Oj != Oq
        if(index != clusters[idcluster].instances[i])
            aux += similarity(this->graphs_tr[index], this->graphs_tr[clusters[idcluster].instances[i]]);
    }

    aux = aux/(clusters[idcluster].size - 1);

    return aux;
}

float PSC::R3(int index, float r2)
{
    int i, idcluster;
    float aux = 0;
    idcluster = training[index].idcluster;

    if(clusters[idcluster].size == 0)
		return 0;

    for(i = 0; i < clusters[idcluster].size; i++)
    {
        if(index != clusters[idcluster].instances[i])
            aux += std::abs(r2 - similarity(this->graphs_tr[index], this->graphs_tr[clusters[idcluster].instances[i]]));
    }
    aux = aux/(clusters[idcluster].size - 1);
    return aux;
}

float PSC::R1(int index, int k)
{
    float r2 = R2(index);
    return (r2 / (R3(index, r2) + (1 - r2))) + R4(index, k);
}

void PSC::calculateR(int k)
{
    int i;
    for(i = 0; i < this->nInst; i++)
        this->training[i].r1 = R1(i, k);
}

int* PSC::maximizeInst(int k)
{
    int i, j, aux;
    int* maximize;
    
    //reserva memoria para los clusters
    maximize = (int*)calloc(k, sizeof(int)); //array con el numero de clases

    for(i = 0; i < k; i++)
    {   
        aux = this->clusters[i].representative.index;//seleccionamos el primer elemento que contiene la posicion de algún elemento en training o graphs_tr
        for(j = 0; j < this->clusters[i].size; j++)//busca el grafo con mayor R
            if(training[aux].r1 < training[clusters[i].instances[j]].r1)
                aux = clusters[i].instances[j];
       
        maximize[i] = aux;
        
    }

    return maximize;
}

/*
 * Función que asigna los grafos a cada uno de los clusters
 */
void PSC::assign(int k)
{
    int i, j, index = 0;
    int **temp;
    std::vector<int> aux (k);
    float dis1, dis2;
    void *tmp_ptr;
    
    temp = (int**)calloc(k, sizeof(int*));

    for(i = 0; i < k; i++){
        temp[i] = (int*) calloc(this->nInst, sizeof(int));
    }

    //Este ciclo asocia los grafos a cada cluster
    for(i = 0; i < nInst; i++)
    {
    	//distancia entre el grafo mas representativo y el grafo i
        dis1 = Distance(this->graphs_tr[i], this->graphs_tr[clusters[index].representative.index]);
        for(j = 1; j < k; j++) // A partir del siguiente grafo representativo
        {
        	//Distancia entre el grafo mas representativo
            dis2 = Distance(this->graphs_tr[i], this->graphs_tr[clusters[j].representative.index]);
            if(dis2 < dis1) //buscamos la minima distancia
            {
                dis1 = dis2;
                index = j; //Guardamos el número de cluster mínimo
            }
        }
        //index tiene el numero de cluster
        //aux en la posicion index tiene el numero de elementos del cluster
        //En Temp en la posicion de cluster index y posicion aux[index] asigna el elemento(o grafo) i al cluster numero "index"
        temp[index][aux[index]++] = i;
        index = 0;
    }
    
    //Este ciclo lo unico que hace es copiar los indices de instancias
    //a la variable instance que se encuentra en la estructura cluster
    for(i = 0; i < k; i++)
    {
    	//copia los tamaños de los clusters o bien el numero de elementos
        clusters[i].size = aux[i];
        
        //reserva memoria para guardar las instancias asignadas en cada cluster
        clusters[i].instances = (int*)calloc(clusters[i].size, sizeof(int)); 
        
        for(j = 0; j < clusters[i].size; j++)
        {
            clusters[i].instances[j] = temp[i][j];
            //Asociamos el numero de cluster a la instancia
            this->clusters[i].representative.idcluster = i;
            this->training[temp[i][j]].idcluster = i;
        }

    }

}


/*
 * Función que genera los prototipos representativos para los elementos representativos
 */
std::vector<Cluster> PSC::randomRepresentatives(int k)
{
    srand( getpid() );

    int i, random;
    std::vector<int> indexes (k);
    std::vector<Cluster> clusters (k);
    
    //inicializamos vector de indices
    for(i = 0; i < k; i++)
        indexes[i] = -1;
    
    //para cada posicion en indexes generamos un numero aleatorio
    for(i = 0; i < k; i++)
    {
        random = rand() % this->nInst;
        while(this->isInto(random, indexes) == 1)
            random = rand() % this->nInst;
        indexes[i] = random;
    }
    
    //De acuerdo a los indices aleatorios elegidos como grafos representativos
    //le asignamos la informacion del grafo a los elementos representativos
    for(i = 0; i < k; i++)
    {
    	clusters[i].representative.id = this->graphs_tr[indexes[i]]->GetId();
    	clusters[i].representative.index = indexes[i];
    	clusters[i].representative.cls = this->graphs_tr[indexes[i]]->GetCls();
    }

    return clusters;
}

void PSC::KMSF(int k)
{
    int i, band = 0, count = 0, z = 0;
    int maxiter = this->nInst/2;
    std::vector<InstanceGraph> temp (k);
    std::vector<InstanceGraph> temp2 (k);

    this->clusters = randomRepresentatives(k);

    
    
    while(band == 0 && z < maxiter)
    {
        if(z > 0) //bandera para copiar a temporal a partir del segundo elemento
            for(i = 0; i < k; i++)//Copiamos elementos de temporal2 a temporal
            {
            	temp2[i].id = temp[i].id;
                temp2[i].cls = temp[i].cls;
                temp2[i].index = temp[i].index;
                temp2[i].idcluster = temp[i].idcluster;
                temp2[i].r1 = temp[i].r1;
            }
        
        //Copiamos los grafos representativos a la variable temp
        for(i = 0; i < k; i++)
        {
        	temp[i].id = this->clusters[i].representative.id;
        	temp[i].cls = this->clusters[i].representative.cls;
        	temp[i].index = this->clusters[i].representative.index;
        	temp[i].idcluster = this->clusters[i].representative.idcluster;
        	temp[i].r1 = this->clusters[i].representative.r1;
        }
        
        //Asignamos los elementos al cluster
        this->assign(k);
        
        //calculamos los objetos representativos
        this->calculateR(k);
        
        //Buscamos el mas representativo en cada cluster
        this->maximize = maximizeInst(k);
        
        
        for(i = 0; i < k; i++) //Copiamos los mas representativos
        {
            this->clusters[i].representative.id = this->training[maximize[i]].id;
            this->clusters[i].representative.cls = this->training[maximize[i]].cls;
            this->clusters[i].representative.index = this->training[maximize[i]].index;
            this->clusters[i].representative.idcluster = this->training[maximize[i]].idcluster;
            this->clusters[i].representative.r1 = this->training[maximize[i]].r1;
        }
        
        for(i = 0; i < k; i++) //En este ciclo lo unico que se evalua es la convergencia
        {
        	//es decir si no hay cambios en los elementos mas representativos quiere decir que termina el KMEANS
            if(this->clusters[i].representative.id == temp[i].id)
                count++;
        }
        std::cout << (z+1) << ". similars:" << count << " == "  << k << " max:" << maxiter << std::endl;

        if(count == k) //si count igual a k entonces ya hay mas cambios por lo tanto se activa la bandera
            band = 1;
        else
        {
            count = 0;
            for(i = 0; i < k; i++)
            {
                if(this->clusters[i].representative.id == temp2[i].id)
                    count++;
            }

            if(count == k)
            band = 1;            
        }
        
        count = 0;
        z++;
    }
}


///*----------------------------------*/

/*
 * Funcion que calcula los prototipos frontera con respecto a la clase mayoritaria
 */

int PSC::Oc(Cluster* clus, std::string mcls, InstanceGraph* gr)
{
    int i, aux;
    float distance, disMin = 3.40e38;
    for(i = 0; i < clus->size; i++) //recorremos los elementos del cluster
    {
        if(this->training[clus->instances[i]].cls == mcls) //si la clase es la misma a la mayoritaria
        {
            distance = Distance(this->graphs_tr[this->training[clus->instances[i]].index], this->graphs_tr[gr->index]); //calculamos la distancia
            if(distance < disMin) //Asignamos la minima distancia
            {
                disMin = distance;
                aux = training[clus->instances[i]].index;
            }
        }
    }
    return aux;
}

/*
 * Buscamos el prototipo enemigo mas cercano con el fin de encontrar su prototipo frontera
 *
 */
int PSC::Om(Cluster* clus, InstanceGraph* gr) //Parametros son el cluster actual y el prototipo frontera seleccionado
{
    int i, aux;
    float distance, disMin = 3.40e38;

    for(i  = 0; i < clus->size; i++) //Recorremos los elementos del cluster
    {
        if(this->training[clus->instances[i]].cls != gr->cls) //buscamos el prototipo enemigo mas cercano
        {
            distance = this->Distance(this->graphs_tr[clus->instances[i]], this->graphs_tr[gr->index]);
            if(distance < disMin){
                disMin = distance;
                aux = training[clus->instances[i]].index;
            }
        }
    }
    return aux;
}

/*
 * Función que ejecuta el método de Selección de instancias PSC
 */
void PSC::execute(std::vector<Graph*> graphs, int k)
{
    float maxClass;
    int i, j, count = 0, aux, kc;
    std::string mcls;
    
    this->graphs_tr = graphs;
    this->nInst = graphs_tr.size();
    this->PSC1.clear();
    
    //Obtenemos el número de ocurrencias por clase
    std::map<std::string,int> occurcls = this->occurclasses();
    this->tcls = occurcls.size();

	//Numero de clusters totales
    kc = k * this->tcls;
    this->numClusters = this->graphs_tr.size();
    std::vector<int> temp (this->graphs_tr.size());

    for(i = 0; i < (int) temp.size(); i++)
        temp[i] = -1;
    
    //std::cout << "N clusters: " << kc << std::endl;

    this->start = time(NULL);

    //Calcula los clusters, toda la informacion se almacena en this->clusters
	KMSF(kc);

	//std::cout << "Si pasa el cluster." << std::endl;
    
    for(i = 0; i < kc; i++)
    {
        if(isHomogeneous(&clusters[i]) == 1)
        {
        	//Copiamos unicamente el centroide representativo
			temp[count++] = this->clusters[i].representative.index;
        }
        else{
        	//Si el cluster no es homogeneo
        	//buscamos los prototipos frontera
            maxClass = -1;
            mcls = "";
            aux = -1;
            std::map<std::string,int>::iterator it;
            //Obtenemos la clase mayoritaria
            for(it = occurcls.begin(); it != occurcls.end(); it++)
            {
            	//obtenemos las ocurrencias en el cluster
                aux = occurCluster(&this->clusters[i], it->first);
                if(aux > maxClass)
                {
                    mcls = it->first;
                    maxClass = aux;
                }
            }

            aux = -1;
            for(j = 0; j < this->clusters[i].size; j++) //recorremos los elementos del cluster actual
                //si el elemento es diferente que la clase mayoritaria
            	if(this->training[this->clusters[i].instances[j]].cls != mcls)
                {
            		//calculamos el prototipo frontera con respecto al enemigo mas cercano de la clase diferente
                    aux = Oc(&this->clusters[i], mcls, &this->training[this->clusters[i].instances[j]]);
                    if(aux != -1 && isInto(aux, temp) == 0) //agregamos al listado de prototipos si es que no existe
                    {

                        temp[count++] = aux; // agregamos prototipo
                        aux = Om(&clusters[i], &training[aux]); //calculamos su frontera
                        if(aux != -1 && isInto(aux, temp) == 0)
                            temp[count++] = aux;
                    }
                }
        }
    }

    //std::cout << "Calcula prototipos centroides y frontera..." << std::endl;
    //std::cin.get();

    //Tiempo final del calculo
    this->end = time(NULL);
    //Numero de prototipos seleccionados
    this->tSelected = count;
    //Calculamos el porcentaje de retencion
    this->percentRet = (float)(((float)this->tSelected/(float)(this->nInst))*100);

    for(i = 0; i < tSelected; i++)
        this->PSC1.push_back(temp[i]);

    this->writeResult(this->PSC1, this->fileout + "-psc.txt", this->PSC1.size(), this->percentRet, this->end);

}

#endif
