#ifndef PSR_H_
#define PSR_H_

typedef struct{
	std::string id;
    int index;
    std::string cls;
    float relevance;
}Instance;

bool sortByRelevance(const Instance &lhs, const Instance &rhs) { return lhs.relevance < rhs.relevance; }

class PSR {
	private:
            int percent_relevance;
            int percent_not_relevance;
            double alpha;
            double tnode;
            double tedge;
            double w_substedge;
            bool print_details;
            bool calculateGED;
            std::string dbname;
            std::vector<Graph*> graphs_tr;
            std::vector<int> PSR1, PSR2, PSR3, PSR4, PSR5, PSR6; //Guarda prototipos
            int nInst;
            int tcls;
            float percentRet1, percentRet2, percentRet3, percentRet4, percentRet5, percentRet6;
            time_t start, end1, end2, end3, end4, end5, end6;
            std::map<std::string,double> distances;
            std::string fileout;

            std::vector<int> simplify(std::vector<int> array);
            void writeResult(std::vector<int> indexes, std::string output, int size, float ret, time_t ends);
            int isInto(int dat, int *array, int size);
            int searchIndex(int ind);
            std::map<std::string,int> occurclasses();
            std::vector<Instance> calcRelevance(std::map<std::string,int> occur_classes);
            bool order_instances(Instance, Instance);
            double Distance(Graph* g1, Graph* g2);

	public:
            PSR(std::string, int , int, double, double, double, double, bool, std::string);
            PSR(int percent_relevance, int percent_not_relevance, std::map<std::string,double> distances, std::string fileout);
            ~PSR() {}
            std::vector<int> getPSR1();
            std::vector<int> getPSR2();
            std::vector<int> getPSR3();
            std::vector<int> getPSR4();
            std::vector<int> getPSR5();
            std::vector<int> getPSR6();
            float getRetPSR1();
            float getRetPSR2();
            float getRetPSR3();
            float getRetPSR4();
            float getRetPSR5();
            float getRetPSR6();
            void execute(std::vector<Graph*> graphs);
};

/**
 * Constructor utilizado para ejecutar el algoritmo PSR con distancias leidas desde archivo
 *
 */
PSR::PSR(int percent_relevance, int percent_not_relevance, std::map<std::string,double> distances, std::string fileout)
{
	this->percent_relevance = percent_relevance;
	this->percent_not_relevance = percent_not_relevance;
	this->distances = distances;
	this->alpha = 0;
	this->tnode = 0;
	this->tedge = 0;
	this->w_substedge = 0;
	this->print_details = false;
	this->dbname = "";
	this->tcls = 0;
	this->percentRet1 = this->percentRet2 = this->percentRet3 = this->percentRet4 = this->percentRet5 = this->percentRet6 = 0;
	this->start = this->end1 = this->end2 = this->end3 = this->end4 = this->end5 = this->end6 = 0;
	this->calculateGED = false;
	this->fileout = fileout;
}

/**
 * Constructor utilizado para ejecutar el algoritmo PSR de manera normal, es decir, se calculan
 * las distancias con el algoritmo de Distancia de Edición en Grafos.
 *
 */
PSR::PSR(std::string dbname, int percent_relevance, int percent_not_relevance, double alpha, double tnode, double tedge, double w_substedge, bool print_details, std::string fileout)
{
	this->percent_relevance = percent_relevance;
	this->percent_not_relevance = percent_not_relevance;
	this->alpha = alpha;
	this->tnode = tnode;
	this->tedge = tedge;
	this->w_substedge = w_substedge;
	this->print_details = print_details;
	this->dbname = dbname;
	this->tcls = 0;
	this->percentRet1 = this->percentRet2 = this->percentRet3 = this->percentRet4 = this->percentRet5 = this->percentRet6 = 0;
	this->start = this->end1 = this->end2 = this->end3 = this->end4 = this->end5 = this->end6 = 0;
	this->calculateGED = true;
	this->fileout = fileout;
}


std::vector<int> PSR::getPSR1()
{
	return this->PSR1;
}

std::vector<int> PSR::getPSR2()
{
	return this->PSR2;
}

std::vector<int> PSR::getPSR3()
{
	return this->PSR3;
}

std::vector<int> PSR::getPSR4()
{
	return this->PSR4;
}

std::vector<int> PSR::getPSR5()
{
	return this->PSR5;
}

std::vector<int> PSR::getPSR6()
{
	return this->PSR6;
}

//---


float PSR::getRetPSR1()
{
    return this->percentRet1;
}

float PSR::getRetPSR2()
{
    return this->percentRet2;
}

float PSR::getRetPSR3()
{
    return this->percentRet3;
}

float PSR::getRetPSR4()
{
	return this->percentRet4;
}

float PSR::getRetPSR5()
{
	return this->percentRet5;
}

float PSR::getRetPSR6()
{
	return this->percentRet6;
}

//Functions

/*
 * Función utilizada para eliminar los prototipos repetidos
 */

std::vector<int> PSR::simplify(std::vector<int> array)
{
	std::vector<int> aux_v;
	std::vector<int> final_v;
    int count, num, i, k;
    
    for (i = 0; i < (int)array.size(); i++){
        if(array[i] == -1)
            break;
        
        count = 0;
        num = array[i];
        aux_v.push_back(num);

        for (k = 0; k < (int)aux_v.size(); k++)
            if (aux_v[k] == num)
                count++;

        if (count == 1)
            final_v.push_back(num);
    }

    return final_v;
}

void PSR::writeResult(std::vector<int> indexes, std::string output, int size, float ret, time_t ends)
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

/*
 * Calcula las ocurrencias de cada una de las clases
 */
std::map<std::string,int> PSR::occurclasses()
{

	std::vector<Graph*>::iterator i;
	std::map<std::string,int> occur_classes;

    for(i = this->graphs_tr.begin(); i != this->graphs_tr.end(); i++)
    {
    	if(!occur_classes.count((*i)->GetCls()))
    		occur_classes[(*i)->GetCls()] = 0;
    	occur_classes[(*i)->GetCls()]++;
    }

    return occur_classes;
}
/*
 * Distancia de edición en Grafos
 */
double PSR::Distance(Graph* g1, Graph* g2)
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

/*
 * Funcion que calcula la relevancia de cada instancia
 * con respecto al conjunto total de instancias
 */
std::vector<Instance> PSR::calcRelevance(std::map<std::string,int> occur_cls)
{
    int i, j;
    float aux;
    std::vector<Instance> graphs_relevance(this->nInst);

    //Inicializamos
    for(i = 0; i < this->nInst; i++)
    {
    	graphs_relevance[i].index = i;
    	graphs_relevance[i].relevance = 0;
    	graphs_relevance[i].cls = this->graphs_tr[i]->GetCls();
    }

    //Calculamos las distancias
    for(i = 0; i < this->nInst - 1; i++){ //Para todas las instancias excepto la ultima

    	for(j = i + 1; j < this->nInst; j++){ //iniciamos a partir de la siguiente instancia puesto que las anteriores ya se calcularon
        	//Si la instancia en i es igual en clase a la instancia en j
            if(this->graphs_tr[i]->GetCls() == this->graphs_tr[j]->GetCls())
            {
                aux = this->Distance(this->graphs_tr[i], this->graphs_tr[j]);
                graphs_relevance[i].relevance += aux; //agregamos la distancia a la relevancia en i y j
                graphs_relevance[j].relevance += aux;
            }
        }
        //calculamos la relevancia final diviendolo entre el numero de ocurrencias por clase
    	graphs_relevance[i].relevance /= (float)(occur_cls[graphs_relevance[i].cls] - 1);
    }

    //calculamos la relevancia final diviendolo entre el numero de ocurrencias por clase
    //esto para la ultima instancia
    graphs_relevance[i].relevance /= (float)(occur_cls[graphs_relevance[i].cls] - 1);

    return graphs_relevance;
}

/*
 * Algoritmo de Seleccion de Instancias PSR1-6
 */

void PSR::execute(std::vector<Graph*> graphs)
{
    float disMin =  3.40e38;
    int *retC, i, j, count1 = 0, count2 = 0, aux;
    int *retNC;
    std::map<std::string,int>::iterator it;
    double distance;
    
    //Copiamos los grafos
    this->graphs_tr = graphs;
    this->nInst = graphs_tr.size();
    //Limpiamos variables
    this->PSR1.clear();
    this->PSR2.clear();
    this->PSR3.clear();
    this->PSR4.clear();
    this->PSR5.clear();
    this->PSR6.clear();
              
    this->start = time(NULL);

    std::map<std::string,int> occur_cls = occurclasses(); //calculamos las ocurrencias de las clases
    this->tcls = occur_cls.size();

    //tcls tiene el numero de clases
    retC = (int*)calloc(this->tcls, sizeof(int)); //array con el numero de clases
    retNC = (int*)calloc(this->tcls, sizeof(int)); //array con el numero de clases

    //recorremos cada una de las clases
    //para calcular el numero de prototipos relevantes por clase
    for(it = occur_cls.begin(), i = 0; it != occur_cls.end(); it++, i++){
        retC[i] = (int)(occur_cls[it->first]*(this->percent_relevance/(float)100)); //Guarda el numero de instancias a seleccionar
        count1 += retC[i];
    }

    //PSR1
    //Calculamos la relevancia de cada instancia
    std::vector<Instance> graphs_relevance;
    graphs_relevance = this->calcRelevance(occur_cls);


    //ordenamos grafos de acuerdo a su relevancia
    std::sort(graphs_relevance.begin(), graphs_relevance.end(), sortByRelevance);


    j = 0;
    count2 = 0;
    for(it = occur_cls.begin(), i = 0; it != occur_cls.end(); it++, i++)//recorremos todas las clases
    {
    	while(j < retC[i])//obtenemos las instancias relevantes hasta que se tenga el porcentaje requerido
        {
            if(graphs_relevance[count2].cls == it->first) //si es igual a la clase se incrementa count2 para recorrer el array del conjunto de entrenamiento
            {
                this->PSR1.push_back(graphs_relevance[count2].index);
                j++; //j lleva el contador del numero de instancias relevantes
            }
            count2++;
        }
        count2 = 0;
        j = 0;
    }

    //tiempo que le toma a PSR1
    this->end1 = time(NULL);

    //Generamos PSR2
    aux = -1;

    for(i = 0; i < (int)PSR1.size(); i++)//recorremos el numero de instancias relevantes seleccionadas
    {
        for(j = 0; j < this->nInst; j++) //recorremos las instancias del conjunto de entrenamiento
        {
        	//añadimos la instancia borde de diferente clase mas cercana a la clase relevante
            if(this->graphs_tr[this->PSR1[i]]->GetCls() != this->graphs_tr[j]->GetCls()){
            	//calculamos distancia entre estas dos instancias
                distance = Distance(this->graphs_tr[this->PSR1[i]], this->graphs_tr[j]);
                //si esta distancia es menor que la distancia establecida
                if(distance < disMin){
                    disMin = distance;//actualizamos distancia minima
                    aux = j; //guardamos la posicion de la distancia
                }
            }
        }

        //Guardamos instancia borde
        this->PSR2.push_back(aux);
        //borramos datos para el calculo de la siguiente distancia minima para la instancia relevante
        disMin =  3.40e38;
        aux = -1;
    }

    //PSR2: Tiempo que le toma calcular los bordes
    end2 = time(NULL);    
    end3 = end2;

    //Select the non-relevant instances for each class
    count1 = 0;
    for(it = occur_cls.begin(), i = 0; it != occur_cls.end(); it++, i++)
    {
    	retNC[i] = (int)(occur_cls[it->first] * (this->percent_not_relevance/(float)100)); //Guarda el numero de instancias a seleccionar
        count1 += retNC[i];
    }

    std::vector<int> temp;
    j = 0;
    count2 = nInst-1;

    for(it = occur_cls.begin(), i = 0; it != occur_cls.end(); it++, i++){
        while(j < retNC[i]){

            if(graphs_relevance[count2].cls == it->first){
                temp.push_back(graphs_relevance[count2].index);
                j++;
            }
            count2--;
        }
        count2 = this->nInst-1;
        j = 0;
    }

    end4 = end1;
    
    //Formamos PSR4: prototupos relevantes y no relevantes
    for(i = 0; i < (int)this->PSR1.size(); i++)
        this->PSR4.push_back(this->PSR1[i]);
    for(i = 0; i < (int)temp.size(); i++)
    	this->PSR4.push_back(temp[i]);


    //Border between relevant and non-relevant -- PSR5
    aux = -1;

    for(i = 0; i < (int)this->PSR4.size(); i++)
    {
        for(j = 0; j < this->nInst; j++)
        {
            if(this->graphs_tr[PSR4[i]]->GetCls() != this->graphs_tr[j]->GetCls())
            {
                distance = Distance(this->graphs_tr[PSR4[i]], this->graphs_tr[j]);
                if(distance < disMin)
                {
                    disMin = distance;
                    aux = j;
                }
            }
        }
        this->PSR5.push_back(aux);
        disMin =  3.40e38;
        aux = -1;
    }
    
    this->end5 = time(NULL);
    this->end6 = this->end5;

    //Elminamos elementos repetidos en PSR2 y PSR5
    this->PSR2 = simplify(this->PSR2);
    this->PSR5 = simplify(this->PSR5);
    
    //PSR3: Union between the border objects PSR2 and the most relevant objects PSR1
    for(i = 0; i < (int)this->PSR1.size(); i++)
        this->PSR3.push_back(PSR1[i]);

    for(i = 0; i < (int)this->PSR2.size(); i++)
        this->PSR3.push_back(PSR2[i]);
    

    //PSR6: Union between the border objects of PSR4, the relevant objects PSR1 and the non-relevant objects
    for(i = 0; i < (int)this->PSR4.size(); i++)
        this->PSR6.push_back(this->PSR4[i]);
    for(i = 0; i < (int)this->PSR5.size(); i++)
        PSR6.push_back(PSR5[i]);

    //Clean array PSR3 and PSR6
    this->PSR3 = simplify(this->PSR3);
    this->PSR6 = simplify(this->PSR6);
    
    this->percentRet1 = (float)(((float)this->PSR1.size()/(float)(this->nInst))*100);
    this->percentRet2 = (float)(((float)this->PSR2.size()/(float)(this->nInst))*100);
    this->percentRet3 = (float)(((float)this->PSR3.size()/(float)(this->nInst))*100);
    this->percentRet4 = (float)(((float)this->PSR4.size()/(float)(this->nInst))*100);
    this->percentRet5 = (float)(((float)this->PSR5.size()/(float)(this->nInst))*100);
    this->percentRet6 = (float)(((float)this->PSR6.size()/(float)(this->nInst))*100);

    this->writeResult(this->PSR1, this->fileout + "-psr1.txt", this->PSR1.size(), this->percentRet1, this->end1);
    this->writeResult(this->PSR2, this->fileout + "-psr2.txt", this->PSR2.size(), this->percentRet2, this->end2);
    this->writeResult(this->PSR3, this->fileout + "-psr3.txt", this->PSR3.size(), this->percentRet3, this->end3);
    this->writeResult(this->PSR4, this->fileout + "-psr4.txt", this->PSR4.size(), this->percentRet4, this->end4);
    this->writeResult(this->PSR5, this->fileout + "-psr5.txt", this->PSR5.size(), this->percentRet5, this->end5);
    this->writeResult(this->PSR6, this->fileout + "-psr6.txt", this->PSR6.size(), this->percentRet6, this->end6);

}

#endif
