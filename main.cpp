/* 
 * GIS (Graphs Instance Selection) Métodos para la selección de instancias en el
 * espacio de Grafos y posterior a la aplicación del embedding de grafos.
 * File:   main.cpp
 * Author: Magdiel Jiménez Guarneros
 *
 * Created on 10 de septiembre de 2014, 03:28 AM
 */

#include <cstdlib>
#include <iostream>
#include <time.h>
#include <fstream>
#include "src/pugixml/pugiconfig.hpp"
#include "src/pugixml/pugixml.hpp"
#include "src/classifiers/classifiers.h"
#include "src/is/psr.h"
#include "src/is/psc.h"
#include "src/is/psd.h"
#include "src/embedding/embedding.h"

/*
 * Genera lista con las clases
 */
std::map<std::string,int> GetListClasses(std::vector<Graph*> graphs)
{
	std::map<std::string,int> occur_classes;

    for(int i = 0; i < (int) graphs.size(); i++)
    {
    	if(!occur_classes.count(graphs[i]->GetCls()))
    		occur_classes[graphs[i]->GetCls()] = 0;
    	occur_classes[graphs[i]->GetCls()]++;
    }

    return occur_classes;
}


/**
 * Método utilizado para leer los archivos CSV desde archivo.
 *
 */
std::vector<std::vector<std::string> > ReadCSV(std::string filename)
{
    //Lista de grafos
    std::vector<std::vector<std::string> > graphs;

    //Leemos archivo CSV
    std::ifstream csvFile(filename.c_str());
    std::string line;

    bool b = false;
    while (std::getline(csvFile, line))
    {
            //Evitamos leer el encabezado de los atributos
            if(!b)
            {
                    b = true;
                    continue;
            }
            //extraemos cada uno de los atributos
            std::vector<std::string> instance;
            std::stringstream  data(line);
            std::string field;
            while(std::getline(data,field,','))
                instance.push_back(field);
            //Agregamos instancia al grafo
            graphs.push_back(instance);
    }
    csvFile.close();

    return graphs;
}

/**
 * Método utlizado para generar un archivo ARFF a partir de un listado de grafos. Lo
 * unico que se almacena en estos archivos son el Id del grafo y su Clase.
 *
 */
void WriteARFFChangeID(std::vector<Graph *> graphs, std::map<std::string,int> listcls, std::string fileout)
{
    std::ofstream myFileArff;
    //Generamos archivo arff y txt
    myFileArff.open( (fileout + ".arff").c_str()  );

    myFileArff << "@relation " << fileout << std::endl;

    int i = 0;
    //Para no afectar el uso de listcls con otros metodos, sustituimos las ocurrencias de esta variable
    //y en lugar ponemos un indice que representa a la clase del grafo en versión de tipo entera
    for(std::map<std::string,int>::iterator l = listcls.begin(); l != listcls.end() ; l++, i++)
        listcls[l->first] = i;

    //Añadimos las instancias
    std::string idgraphs = "";
    std::string instances = "";
    bool b = false;


    for(i=0; i < (int)graphs.size(); i++)
    {
        //convertimos a string
        std::stringstream stri, strcls;
        stri << i;
        strcls << listcls[graphs[i]->GetCls()];
        //añadimos id y clase
        idgraphs += (b?",":"") + stri.str();
        instances += stri.str() + "," + strcls.str() + "\n";

        b = true;
    }

    //Grabamos los datos de los valores que puede tomar los ids de grafos
    myFileArff << "@attribute idgraph {" << idgraphs << "}" << std::endl;

    std::map<std::string,int>::iterator it;
    std::string lcls = "";
    b = false;
    for(it = listcls.begin(); it != listcls.end(); it++)
    {
        std::stringstream strcls;
        strcls << listcls[it->first];
        lcls += (b?",":"") + strcls.str();
        b = true;
    }

    //Grabamos los datos de las clases
    myFileArff << "@attribute  class {" << lcls << "}" << std::endl;

    //Grabamos los datos de las instancias
    myFileArff << "@data" << std::endl;
    myFileArff << instances << std::endl;

    myFileArff.close();
}

/**
 * Método utlizado para generar un archivo ARFF a partir de un listado de grafos. Lo
 * unico que se almacena en estos archivos son el Id del grafo y su Clase.
 *
 */
void WriteARFF(std::vector<Graph *> graphs, std::map<std::string,int> listcls, std::string fileout)
{
    std::ofstream myFileArff;
    //Generamos archivo arff y txt
    myFileArff.open( (fileout + ".arff").c_str()  );

    myFileArff << "@relation " << fileout << std::endl;

    int i = 0;

    //Añadimos las instancias
    std::string idgraphs = "";
    std::string instances = "";
    bool b = false;


    for(i=0; i < (int)graphs.size(); i++)
    {
        //añadimos id y clase
        idgraphs += (b?",":"") + graphs[i]->GetId();
        instances += graphs[i]->GetId() + "," + graphs[i]->GetCls() + "\n";

        b = true;
    }

    //Grabamos los datos de los valores que puede tomar los ids de grafos
    myFileArff << "@attribute idgraph {" << idgraphs << "}" << std::endl;

    std::map<std::string,int>::iterator it;
    std::string lcls = "";
    b = false;
    for(it = listcls.begin(); it != listcls.end(); it++)
    {
        lcls += (b?",":"") + it->first;
        b = true;
    }

    //Grabamos los datos de las clases
    myFileArff << "@attribute  class {" << lcls << "}" << std::endl;

    //Grabamos los datos de las instancias
    myFileArff << "@data" << std::endl;
    myFileArff << instances << std::endl;

    myFileArff.close();
}


/**
 * Método utlizado para generar un archivo CSV a partir de un listado de grafos. Lo
 * unico que se almacena en estos archivos son el Id en numerico y su Clase en numerico.
 *
 */
void WriteCSVChangeID(std::vector<Graph *> graphs, std::map<std::string,int> listcls, std::string fileout)
{
    std::ofstream myFileCsv;
    //Generamos archivo arff y txt
    myFileCsv.open( (fileout + ".csv").c_str() );
    myFileCsv << "idgraph,class" << std::endl;
    int i = 0;

    //Para no afectar el uso de listcls con otros metodos, sustituimos las ocurrencias de esta variable
    //y en lugar ponemos un indice que representa a la clase del grafo en versión de tipo entera
    for(std::map<std::string,int>::iterator l = listcls.begin(); l != listcls.end() ; l++, i++)
        listcls[l->first] = i;


    //Añadimos las instancias al archivo
    for(i =0; i < (int)graphs.size(); i++ )
    {
        std::stringstream strcls;
        strcls << listcls[graphs[i]->GetCls()];
        //escribimos en archivo
        myFileCsv << i << "," << strcls.str() << "\n";
    }

    myFileCsv.close();
}

/**
 * Método utlizado para generar un archivo CSV a partir de un listado de grafos. Lo
 * unico que se almacena en estos archivos son el Id en numerico y su Clase en numerico.
 * @param graphs
 * @param fileout
 */
void WriteCSV(std::vector<Graph *> graphs, std::string fileout)
{
    std::ofstream myFileCsv;
    //Generamos archivo arff y txt
    myFileCsv.open( (fileout + ".csv").c_str() );
    myFileCsv << "idgraph,class" << std::endl;
    int i = 0;

    //Añadimos las instancias al archivo
    for(i =0; i < (int)graphs.size(); i++)
        //escribimos en archivo
        myFileCsv << graphs[i]->GetId() << "," << graphs[i]->GetCls() << "\n";

    myFileCsv.close();
}

/**
 * WriteNum. Método utlizado para generar un archivo num a partir de un listado de grafos. Lo
 * unico que se almacena en estos archivos son el Id del grafo y su Clase.
 * @param graphs
 * @param listcls
 * @param fileout
 */
void WriteNum(std::vector<Graph *> graphs, std::map<std::string,int> listcls, std::string fileout)
{
    std::ofstream myFileNum;
    //Generamos archivo arff y txt
    myFileNum.open( (fileout + ".num").c_str() );
    //Colocamos numero de atributos y total de instancias
    myFileNum << "1 1 " << graphs.size() << std::endl;
    //Colocamos en archivo el tipo de atributos en nuestro caso nominal y el total de clases
    myFileNum << "5 " << listcls.size() << std::endl;
    int i = 0;

    //Para no afectar el uso de listcls con otros metodos, sustituimos las ocurrencias de esta variable
    //y en lugar ponemos un indice que representa a la clase del grafo en versión de tipo entera
    for(std::map<std::string,int>::iterator l = listcls.begin(); l != listcls.end() ; l++, i++)
        listcls[l->first] = i;

    //Añadimos las instancias al archivo num en versión de enteros
    for(i =0; i < (int)graphs.size(); i++ )
            myFileNum << i << " " << listcls[graphs[i]->GetCls()] << "\n";

    myFileNum.close();
}

/**
 * WriteCSVToARFF. Pasa un archivo en formato CSV a formato ARFF.
 * @param filecsv
 * @param filearff
 */
void WriteCSVToARFF(std::string filecsv, std::string filearff)
{
    //Leemos ids grafos y clases desde archivo
    std::vector<std::vector<std::string> > instances = ReadCSV(filecsv);

    //Copiamos a una lista de grafos unicamente ids y clases
    std::vector<Graph*> graphs;
    for(int i=0; i < (int)instances.size(); i++)
    {
        Graph *g = new Graph(instances[i][0], instances[i][1]);
        graphs.push_back(g);
    }
    
    //escribimos archivo arff
    WriteARFF(graphs, GetListClasses(graphs), filearff);
}

/**
 * WriteCSVToARFF_FilesKFold. Pasa varios archivos que tienen los KFold de CSV a
 * ARFF.
 * @param filecsv
 * @param filearff
 * @param nfolds
 * @param seed
 */
void WriteCSVToARFF_FilesKFold(std::string filecsv, std::string filearff, int nfolds, int seed)
{
    std::string fileaux, filename;
    std::vector<std::vector<std::string> > gr_tr;
    std::vector<Graph*> graphs;
    
    for(int s = 0; s < seed; s++)
    {
        for(int f = 0; f < nfolds; f++)
        {
            std::stringstream s_fold, s_seed;
            s_fold << (f+1);
            s_seed << s;
            
            
            fileaux = "-train-fold-" + s_fold.str() + "-seed-" + s_seed.str();
            filename = filecsv + fileaux + ".csv";
            
            std::cout << "Leyendo " << filename << "..." << std::endl;
            //Leemos los conjuntos de entrenamiento y prueba
            
            gr_tr = ReadCSV(filename);
            
            //Copiamos conjunto de entrenamiento
            for(int i = 0; i < gr_tr.size(); i++)
            {
                //Creamos grafo y añadimos a lista
                Graph *g = new Graph(gr_tr[i][0], gr_tr[i][1]);
                graphs.push_back(g);
            }
            
            WriteARFF(graphs, GetListClasses(graphs), filearff + fileaux);
            
            gr_tr.clear();
            graphs.clear();
        }
    }
}

/**
 * WriteDATToCSVAndARFF. Pasa un archivo en formato DAT(generado por DROP3-5) a 
 * formato CSV, nos ayudamos de la lectura de los archivos originales que contienen
 * el fold original.
 * @param filecsv
 * @param filearff
 */
void WriteDATtoCSVAndARFF(std::string filedat, std::string filefold, int nfold, int seed, std::string fileout)
{
    //Lista de grafos
    int f, s;
    std::string filename, fileaux;

    //Leemos archivo DAT
    std::ifstream datFile(filedat.c_str());
    std::string line;
    std::vector<Graph*> lstgr;
    f = 0;
    s = 0;
    
    while (std::getline(datFile, line)) //Cada linea es un Fold que contiene los ID's de cada grafo
    {
        std::stringstream s_fold, s_seed;
        //determinamos el fold
        s_fold << (f % nfold) + 1;
        s_seed << s;
        fileaux = "-train-fold-" + s_fold.str() + "-seed-" + s_seed.str();
        filename = filefold + fileaux + ".csv";
        std::cout << filename << " => ";
        
        //Leemos ids grafos y clases desde archivo
        std::vector<std::vector<std::string> > graphs_fold = ReadCSV(filename);
        
        //Creamos un hashmap para almacenar los ID's y mapearlos con la clase correspondiente
        std::map<std::string, std::string> gr_cls;
        for(int i = 0; i < graphs_fold.size(); i++)
            //Añadimos ID y Clase
            gr_cls[graphs_fold[i][0]] = graphs_fold[i][1];
        
        //extraemos listado
        std::stringstream data(line);
        std::string field;
        
        //mapeamos los IDs con sus clases y creamos una lista de grafos
        while(std::getline(data,field,',')) //cada campo es un id que contiene los ID's
        {
            if(gr_cls.count(field))
            {
                Graph *g = new Graph(field, gr_cls[field]);
                lstgr.push_back(g);
            }
            else
                std::cout << "No se encontró el ID: " << field << std::endl;
        }
        
        std::cout << fileout + fileaux << std::endl;
        //Escribimos en archivo CSV y ARFF
        WriteCSV(lstgr, fileout + fileaux);
        WriteARFF(lstgr, GetListClasses(lstgr), fileout + fileaux);
        
        //limpiamos vectores
        gr_cls.clear();
        lstgr.clear();
        
        //incrementamos fold y actualizamos contador de seed
        f++;
        s = (int)(f / nfold);
    }
    
    datFile.close();
}


/**
 * readEditDistances. Lectura de las distancias de edición desde archivo, mis-
 * mas en las que el algoritmo fue programado en C++.
 * @param filename
 * @return 
 */
std::map<std::string, double> ReadGEDs(std::string filename)
{
	std::map<std::string, double> distances;

	//Leemos archivo CSV
	std::ifstream csvFile(filename.c_str());
	std::string line;

	while (std::getline(csvFile, line)) // there is input overload classfile
	{
		//extraemos cada uno de los atributos
		std::stringstream  data(line);
		std::string idg1, idg2, d;

		//Leemos el ID del grafo
		std::getline(data,idg1,',');
		std::getline(data,idg2,',');
		std::getline(data,d,',');

		if (!distances.count( idg1 + "-" + idg2))
		{
			distances[idg1 + "-" + idg2] = atof(d.c_str());
		}
		else
			std::cout << "Esta distancia ya existe en el arreglo..." << std::endl;
	}
	csvFile.close();

	return distances;
}

/**
 * Preprocesamiento de Fingerprint. Se cambian clases de acuerdo a paper y tesis
 * publicada por riesen.
 * @param graphs
 * @return 
 */
std::vector<Graph *> PreprocessFingerPrint(std::vector<Graph *> graphs)
{
	std::string s;
    for(int i=0; i < (int)graphs.size();i++)
    {
    	s = graphs[i]->GetCls().substr(0,1);
    	graphs[i]->SetCls(s);

        if( graphs[i]->GetCls() == "T")
        	graphs[i]->SetCls("A");
    }

    return graphs;
}

/**
 * Prototype Selection Discriminative. Método de selección de instancias basado
 * en la relevancia de cada una de las instancias con respecto a las demás ins-
 * tancias de la clase.
 */
void PSDNormalExecute(std::string path, std::string database, double alpha, double tnode, double tedge, double w_substedge, std::string label, bool concatid, float *Ws, int nprot, std::string fileout)
{
	time_t time1, time2;
	//leemos los grafos de los archivos CXL
	std::vector<Graph *> graphs_tr = readlistgraphs(path,"train.cxl",label, concatid);
	
	time (&time1);
	std::cout << "The start psc time is: " << ctime (&time1) << std::endl;

	//Creamos objeto PSD y ejecutamos la seleccion de instancias
	PSD *psd = new PSD(database, alpha, tnode, tedge, w_substedge, false, fileout);
	psd->execute(graphs_tr, Ws, nprot);

        //escribimos en archivo los prototipos
	WriteCSV(psd->getCPS(), fileout + "-cps");
	WriteCSV(psd->getBPS(), fileout + "-bps");
	WriteCSV(psd->getRPS(), fileout + "-rps");
	WriteCSV(psd->getSPS(), fileout + "-sps");
	WriteCSV(psd->getTPS(), fileout + "-tps");

        WriteARFF(psd->getCPS(), GetListClasses(psd->getCPS()), fileout + "-cps");
        WriteARFF(psd->getBPS(), GetListClasses(psd->getCPS()), fileout + "-bps");
        WriteARFF(psd->getRPS(), GetListClasses(psd->getCPS()), fileout + "-rps");
        WriteARFF(psd->getSPS(), GetListClasses(psd->getCPS()), fileout + "-sps");
        WriteARFF(psd->getTPS(), GetListClasses(psd->getCPS()), fileout + "-tps");

	//MOstramos tiempos finales
	time (&time2);
	std::cout << "\nThe finish time is: " << ctime (&time2) << std::endl;
	printf ("%.f seconds.\n", difftime(time2, time1));
}

/**
 * Prototype Selection Discriminative. Lee los grafos desde archivo (id,clase) y las
 * distancias de edición.
 */
void PSDWithGEDsCalculated(std::string filename, std::string filegeds, float *Ws, int nprot, std::string fileout)
{
	time_t time1, time2;
	time (&time1);
	std::cout << "The start psc time is: " << ctime (&time1) << std::endl;

	//Leemos ids grafos y clases desde archivo
	std::vector<std::vector<std::string> > instances = ReadCSV(filename);
	//leemos distancias desde archivo
	std::map<std::string, double> distances = ReadGEDs(filegeds);

	//Copiamos a una lista de grafos unicamente ids y clases
	std::vector<Graph*> graphs;
	for(int i=0; i < (int)instances.size(); i++)
	{
		Graph *g = new Graph(instances[i][0], instances[i][1]);
		graphs.push_back(g);
	}
	//Limpiamos arreglo de instancias
	instances.clear();

	//Creamos objeto PSD
	PSD *psd = new PSD(distances, fileout);
	//ejecutamos Algoritmo de IS PSD
	psd->execute(graphs, Ws, nprot);
	//Obtenemos los prototipos

	//escribimos en archivo los prototipos
	WriteCSV(psd->getCPS(), fileout + "-cps");
	WriteCSV(psd->getBPS(), fileout + "-bps");
	WriteCSV(psd->getRPS(), fileout + "-rps");
	WriteCSV(psd->getSPS(), fileout + "-sps");
	WriteCSV(psd->getTPS(), fileout + "-tps");

        WriteARFF(psd->getCPS(), GetListClasses(psd->getCPS()), fileout + "-cps");
        WriteARFF(psd->getBPS(), GetListClasses(psd->getCPS()), fileout + "-bps");
        WriteARFF(psd->getRPS(), GetListClasses(psd->getCPS()), fileout + "-rps");
        WriteARFF(psd->getSPS(), GetListClasses(psd->getCPS()), fileout + "-sps");
        WriteARFF(psd->getTPS(), GetListClasses(psd->getCPS()), fileout + "-tps");
            
	time (&time2);
	std::cout << "\nThe finish time is: " << ctime (&time2) << std::endl;
	printf ("%.f seconds.\n", difftime(time2, time1));
}


/**
 * PSDWithGEDsAndKFold. Selección de prototipos discriminativos considerando la
 * discriminacion entre clases aplicados con KFold leidos desde archivo.
 */
void PSDWithGEDsAndKFold(std::string pathfolds, std::string filegeds, float *Ws, int nprot, std::string fileout, int nfolds, int seed)
{
    time_t time1, time2;
    
    //Leemos distancias
    std::cout << "Leyendo distancias..." << std::endl;
    std::map<std::string, double> distances = ReadGEDs(filegeds);
    //Variables auxiliares de grafos
    std::vector<std::vector<std::string> > gr_tr;
    std::vector<Graph *> graphs;
    //Creamos objeto KNN
    std::string filetrain, fileaux;
    
    //Creamos objeto PSD
    PSD *psd = new PSD(distances, fileout);

    for(int s = 0; s < seed; s++)
    {
        for(int f = 0; f < nfolds; f++)
        {
            std::stringstream s_fold, s_seed;
            s_fold << (f+1);
            s_seed << s;
            
            fileaux = "-train-fold-" + s_fold.str() + "-seed-" + s_seed.str();
            filetrain = pathfolds + fileaux + ".csv";
            
            std::cout << "Leyendo " << filetrain << "..." << std::endl;
            //Leemos los conjuntos de entrenamiento y prueba
            
            gr_tr = ReadCSV(filetrain);
            std::cout << "T: " << gr_tr.size() <<std::endl; 
            
            //Copiamos conjunto de entrenamiento
            for(int i = 0; i < gr_tr.size(); i++)
            {
                //Creamos grafo y añadimos a lista
                Graph *g = new Graph(gr_tr[i][0], gr_tr[i][1]);
                graphs.push_back(g);
            }
            
            time (&time1);
            std::cout << "The start psd time is: " << ctime (&time1);

            //ejecutamos Algoritmo de IS PSD
            psd->execute(graphs, Ws, nprot);
            
            //escribimos en archivo los prototipos
            //WriteCSV(psd->getCPS(), fileout + fileaux);
            //WriteCSV(psd->getBPS(), fileout + fileaux);
            //WriteCSV(psd->getRPS(), fileout + fileaux);
            WriteCSV(psd->getSPS(), fileout + fileaux);
            //WriteCSV(psd->getTPS(), fileout + fileaux);
            
            //WriteARFF(psd->getCPS(), GetListClasses(psd->getCPS()), fileout + fileaux);
            //WriteARFF(psd->getBPS(), GetListClasses(psd->getBPS()), fileout + fileaux);
            //WriteARFF(psd->getRPS(), GetListClasses(psd->getRPS()), fileout + fileaux);
            WriteARFF(psd->getSPS(), GetListClasses(psd->getSPS()), fileout + fileaux);
            //WriteARFF(psd->getTPS(), GetListClasses(psd->getTPS()), fileout + fileaux);
            
            time (&time2);
            std::cout << "The finish time is: " << ctime (&time2);
            std::cout << "T: "<< psd->getSPS().size() << "  Porcentaje de retención: " << ((double)psd->getSPS().size()/(double)graphs.size() *100) << "  Tiempo: " << difftime(time2, time1) << " seconds.\n" << std::endl;
            
            graphs.clear();
            gr_tr.clear();
        }
    }
}

/**
 * Prototype Selection via Clustering. Método de selección de instancias basado
 * en la relevancia de cada una de las instancias con respecto a las demás ins-
 * tancias de la clase.
 */
void PSCNormalExecute(std::string path, std::string database, double alpha, double tnode, double tedge, double w_substedge, std::string label, int K, bool concatid, std::string fileout)
{
	time_t time1, time2;
	//leemos los grafos de los archivos CXL
	std::vector<Graph *> graphs_tr = readlistgraphs(path,"train.cxl",label, concatid);
	//Este vector guarda las posiciones de los prototipos seleccionados por PSC
	std::vector<int> ps;

	time (&time1);
	std::cout << "The start psc time is: " << ctime (&time1) << std::endl;

	//Creamos objeto PSR y ejecutamos la seleccion de instancias
	PSC *psc = new PSC(database, alpha, tnode, tedge, w_substedge, false, fileout);
	psc->execute(graphs_tr, 6);

	//Obtenemos las posiciones de los prototipos seleccionados
	std::vector<int> lpsc = psc->getListPSC();

	//Actualizamos la lista de grafos con los prorotipos seleccionados
	std::vector<Graph*> graphs_reduced;
	for(int i = 0; i < (int)lpsc.size(); i++)
		graphs_reduced.push_back(graphs_tr[lpsc[i]]);

	//escribimos en archivo los prototipos
	WriteCSV(graphs_reduced, fileout);
        WriteARFF(graphs_reduced, GetListClasses(graphs_reduced), fileout);

	time (&time2);
	std::cout << "\nThe finish time is: " << ctime (&time2) << std::endl;
	printf ("%.f seconds.\n", difftime(time2, time1));
}

/**
 * Prototype Selection via Clustering. Lee los grafos desde archivo (id,clase) y las
 * distancias de edición.
 */
void PSCWithGEDsCalculated(std::string filename, std::string filegeds, std::string fileout)
{
	time_t time1, time2;
	time (&time1);
	std::cout << "The start psc time is: " << ctime (&time1) << std::endl;

	//Leemos ids grafos y clases desde archivo
	std::vector<std::vector<std::string> > instances = ReadCSV(filename);
	//leemos distancias desde archivo
	std::map<std::string, double> distances = ReadGEDs(filegeds);

	//Copiamos a una lista de grafos unicamente ids y clases
	std::vector<Graph*> graphs;
	for(int i=0; i < (int)instances.size(); i++)
	{
		Graph *g = new Graph(instances[i][0], instances[i][1]);
		graphs.push_back(g);
	}
	//Limpiamos arreglo de instancias
	instances.clear();

	//Creamos objeto PSC
	PSC *psc = new PSC(distances, fileout);
	//ejecutamos Algoritmo de IS PSR
	psc->execute(graphs, 6);
	//Obtenemos los prototipos

	std::vector<int> lpsc = psc->getListPSC();

	//Actualizamos la lista de grafos con los prorotipos seleccionados
	std::vector<Graph*> graphs_reduced;
	for(int i = 0; i < (int)lpsc.size(); i++)
		graphs_reduced.push_back(graphs[lpsc[i]]);

	//escribimos en archivo los prototipos
	WriteCSV(graphs_reduced, fileout + ".csv");
        WriteARFF(graphs_reduced, GetListClasses(graphs_reduced), fileout + ".csv");

	time (&time2);
	std::cout << "\nThe finish time is: " << ctime (&time2) << std::endl;
	printf ("%.f seconds.\n", difftime(time2, time1));
}


/**
 * Prototype Selection via Clustering. Lee los grafos desde archivo (id,clase) y las
 * distancias de edición aplicando un K-FOld Cross Validation leido también desde
 * archivo.
 */
void PSCWithGEDsAndKFold(std::string path, std::string filepath, std::string filegeds, std::string fileout, int nfolds, int seed)
{
    time_t time1, time2;
    
    //Leemos grafos desde archivo GXL
    std::vector<Graph *> graphs_tr = readlistgraphs(path,"train.cxl","fingerprints", true);
    std::vector<Graph *> graphs_va = readlistgraphs(path, "valid.cxl", "fingerprints", true);
    std::vector<Graph *> graphs_ts = readlistgraphs(path,"test.cxl","fingerprints", true);
    
    //juntamos todos los grafos en una sola lista
    graphs_tr.insert(graphs_tr.end(), graphs_va.begin(), graphs_va.end());
    graphs_tr.insert(graphs_tr.end(), graphs_ts.begin(), graphs_ts.end());
    
    //Leemos distancias
    std::cout << "Leyendo distancias..." << std::endl;
    std::map<std::string, double> distances = ReadGEDs(filegeds);
    //Variables auxiliares de grafos
    std::vector<std::vector<std::string> > gr_tr;
    std::vector<Graph *> graphs;
    //Creamos objeto KNN
    std::string filetrain, fileaux;
    
    //Creamos objeto PSC
    PSC *psc = new PSC(distances, fileout);

    for(int s = 0; s < seed; s++)
    {
        for(int f = 0; f < nfolds; f++)
        {
            std::stringstream s_fold, s_seed;
            s_fold << (f+1);
            s_seed << s;
            
            fileaux = "-train-fold-" + s_fold.str() + "-seed-" + s_seed.str();
            filetrain = filepath + fileaux + ".csv";
            
            std::cout << "Leyendo " << filetrain << "..." << std::endl;
            //Leemos los conjuntos de entrenamiento y prueba
            
            gr_tr = ReadCSV(filetrain);
            std::cout << "T: " << gr_tr.size() <<std::endl; 
            
            //creamos grafos, para pasar como parametro a la función KNN
            //Copiamos conjunto de entrenamiento
            for(int i = 0; i < gr_tr.size(); i++)
            {
                //Creamos grafo y añadimos a lista
                Graph *g = new Graph(gr_tr[i][0], gr_tr[i][1]);
                //g->size = graphs_tr[atoi(g->GetId().c_str())]->GetNumNodes();
                graphs.push_back(g);
            }
            time (&time1);
            std::cout << "The start psc time is: " << ctime (&time1);

            //ejecutamos Algoritmo de IS PSC
            psc->execute(graphs, 6);
            //Obtenemos los prototipos

            std::vector<int> lpsc = psc->getListPSC();
            
            //Actualizamos la lista de grafos con los prorotipos seleccionados
            std::vector<Graph*> graphs_reduced;
            for(int i = 0; i < (int)lpsc.size(); i++)
                graphs_reduced.push_back(graphs[lpsc[i]]);

            //escribimos en archivo los prototipos
            WriteCSV(graphs_reduced, fileout + fileaux);
            WriteARFF(graphs_reduced, GetListClasses(graphs_reduced), fileout + fileaux);

            time (&time2);
            std::cout << "The finish time is: " << ctime (&time2);
            std::cout << "T: "<< graphs_reduced.size() << "  Porcentaje de retención: " << psc->getRetPSC() << "  Tiempo: " << difftime(time2, time1) << " seconds.\n" << std::endl;
            
            graphs.clear();
            graphs_reduced.clear();
            gr_tr.clear();
        }
    }
}

/**
 * Prototype Selection via Relevance. Método de selección de instancias basado
 * en la relevancia de cada una de las instancias con respecto a las demás ins-
 * tancias de la clase.
 */
void PSRNormalExecute(std::string path, std::string database, double alpha, double tnode, double tedge, double w_substedge, std::string label, int K, bool concatid, std::string fileout)
{
	time_t time1, time2;
	//leemos los grafos de los archivos CXL
	std::vector<Graph *> graphs_tr = readlistgraphs(path,"train.cxl",label, concatid);
	std::vector<Graph *> graphs_reduced;
	//Este vector guarda las posiciones de los prototipos seleccionados por PSR
	std::vector<int> psr3;

	time (&time1);
	std::cout << "The start psr time is: " << ctime (&time1) << std::endl;

	//Creamos objeto PSR y ejecutamos la seleccion de instancias
	PSR *psr = new PSR(database, 30, 5, alpha, tnode, tedge, w_substedge, false, fileout);
	psr->execute(graphs_tr);

	//Obtenemos las posiciones de los prototipos seleccionados
	psr3 = psr->getPSR3();

	//Copiamos unicamente las instancias que fueron seleccionadas a un nuevo arreglo
	for(int i = 0; i < (int)psr3.size(); i++)
		graphs_reduced.push_back(graphs_tr[psr3[i]]);

        //escribimos en archivo los prototipos
        WriteCSV(graphs_reduced, fileout);
        WriteARFF(graphs_reduced, GetListClasses(graphs_reduced), fileout);
        
	//MOstramos tiempos finales
	time (&time2);
	std::cout << "\nThe finish time is: " << ctime (&time2) << std::endl;
	double seconds = difftime(time2, time1);

	printf ("%.f seconds.\n", seconds);
}

/**
 * Prototype Selection via Relevance. Lee los grafos desde archivo (id,clase) y las
 * distancias de edición con la finalidad
 */
void PSRWithGEDsCalculated(std::string filename, std::string filegeds, std::string fileout)
{
	time_t time1, time2;
	time (&time1);
	std::cout << "The start psr time is: " << ctime (&time1) << std::endl;
	//Leemos ids grafos y clases desde archivo
	std::vector<std::vector<std::string> > instances = ReadCSV(filename);
	//leemos distancias desde archivo
	std::map<std::string, double> distances = ReadGEDs(filegeds);

	//Copiamos a una lista de grafos unicamente ids y clases
	std::vector<Graph*> graphs;
	for(int i=0; i < (int)instances.size(); i++)
	{
		Graph *g = new Graph(instances[i][0], instances[i][1]);
		graphs.push_back(g);
	}
	//Limpiamos arreglo de instancias
	instances.clear();

	//Creamos objeto PSR
	PSR *psr = new PSR(30, 5, distances, fileout);
	//ejecutamos Algoritmo de IS PSR
	psr->execute(graphs);
	//Obtenemos los prototipos
	std::vector<int> psr3 = psr->getPSR3();

	//Actualizamos la lista de grafos con los prorotipos seleccionados
	std::vector<Graph*> graphs_reduced;
	for(int i = 0; i < (int)psr3.size(); i++)
		graphs_reduced.push_back(graphs[psr3[i]]);

	//escribimos en archivo los prototipos
	WriteCSV(graphs_reduced, fileout);
        WriteARFF(graphs_reduced, GetListClasses(graphs_reduced), fileout);

	time (&time2);
	std::cout << "\nThe finish time is: " << ctime (&time2) << std::endl;
	printf ("%.f seconds.\n", difftime(time2, time1));
}

/**
 * Prototype Selection via Relevance. Lee los grafos desde archivo (id,clase) y las
 * distancias de edición, además se realiza K-FOld Cross Validation.
 */
void PSRWithGEDsAndKFold(std::string filepath, std::string filegeds, std::string fileout, int nfolds, int seed)
{
    time_t time1, time2;
    //Leemos distancias
    std::cout << "Leyendo distancias..." << std::endl;
    std::map<std::string, double> distances = ReadGEDs(filegeds);
    //Variables auxiliares de grafos
    std::vector<std::vector<std::string> > gr_tr;
    std::vector<Graph *> graphs;
    //Creamos objeto KNN
    std::string filetrain, fileaux;
    
    //Creamos objeto PSR
    PSR *psr = new PSR(30, 5, distances, fileout);    

    for(int s = 0; s < seed; s++)
    {
        for(int f = 0; f < nfolds; f++)
        {
            std::stringstream s_fold, s_seed;
            s_fold << (f+1);
            s_seed << s;
            
            fileaux = "-train-fold-" + s_fold.str() + "-seed-" + s_seed.str();
            filetrain = filepath + fileaux + ".csv";
            
            std::cout << "Leyendo " << filetrain << "..." << std::endl;
            //Leemos los conjuntos de entrenamiento y prueba
            
            gr_tr = ReadCSV(filetrain);
            std::cout << "T: " << gr_tr.size() <<std::endl; 
            
            //creamos grafos, para pasar como parametro a la función KNN
            //Copiamos conjunto de entrenamiento
            for(int i = 0; i < gr_tr.size(); i++)
            {
                //Creamos grafo y añadimos a lista
                Graph *g = new Graph(gr_tr[i][0], gr_tr[i][1]);
                graphs.push_back(g);
            }
            time (&time1);
            std::cout << "The start psr time is: " << ctime (&time1);

            //ejecutamos Algoritmo de IS PSR
            psr->execute(graphs);
            
            //Obtenemos los prototipos
            std::vector<int> psr3 = psr->getPSR3();

            //Actualizamos la lista de grafos con los prorotipos seleccionados
            std::vector<Graph*> graphs_reduced;
            for(int i = 0; i < (int)psr3.size(); i++)
                graphs_reduced.push_back(graphs[psr3[i]]);

            //escribimos en archivo los prototipos
            WriteCSV(graphs_reduced, fileout + fileaux);

            time (&time2);
            std::cout << "The finish time is: " << ctime (&time2);
            std::cout << "T: "<< graphs_reduced.size() << "  Porcentaje de retención: " << psr->getRetPSR3() << "  Tiempo: " << difftime(time2, time1) << " seconds.\n" << std::endl;
            
            graphs.clear();
            graphs_reduced.clear();
            gr_tr.clear();
        }
    }
}


/*
 * Escribimos una base de datos CXL en una archivo. Unicamente id's y clases.
 *
 */
void GraphsToFile(std::string path, std::string database, std::string label, bool concat, std::string fileout, std::string type)
{
	//Leemos gxl que contienen los grafos
	std::vector<Graph *> graphs_tr = readlistgraphs(path, "train.cxl", label, concat);
	//std::vector<Graph *> graphs_va = readlistgraphs(path, "valid.cxl", label, concat);
	//std::vector<Graph *> graphs_ts = readlistgraphs(path, "test.cxl", label, concat);

	if(database == "fingerprint")
	{
		graphs_tr = PreprocessFingerPrint(graphs_tr);
		//graphs_va = PreprocessFingerPrint(graphs_va);
		//graphs_ts = PreprocessFingerPrint(graphs_ts);
	}

	//juntamos todos los grafos en una sola lista
	//graphs_tr.insert(graphs_tr.end(), graphs_va.begin(), graphs_va.end());
	//graphs_tr.insert(graphs_tr.end(), graphs_ts.begin(), graphs_ts.end());
	//extraemos el listado de clases
	std::map<std::string,int> listcls = GetListClasses(graphs_tr);

	//escribimos en archivo el listado de grafos de acuerdo a su tipo
	if (type == "arff")
            WriteARFFChangeID(graphs_tr, listcls, fileout);
        else if (type == "num")
            WriteNum(graphs_tr, listcls, fileout);
	else
            WriteCSVChangeID(graphs_tr, listcls, fileout);
}

/**
 * Calcula las distancias de edición del conjunto de grafos (Entrenamiento y prueba) y
 * los almacena en archivo para su uso posterior en pruebas experimentales.
 * @param path
 * @param database
 * @param alpha
 * @param tnode
 * @param tedge
 * @param w_substedge
 * @param label
 * @param K
 * @param concat
 * @param fileout
 * @param formatnumeric
 */
void GEDToFile(std::string path, std::string database, double alpha, double tnode, double tedge, double w_substedge, std::string label, int K, bool concat, std::string fileout, bool formatnumeric)
{
	//Leemos grafos desde archivo GXL
	std::vector<Graph *> graphs_tr = readlistgraphs(path,"train.cxl",label, concat);
	std::vector<Graph *> graphs_va = readlistgraphs(path, "valid.cxl", label, concat);
	std::vector<Graph *> graphs_ts = readlistgraphs(path,"test.cxl",label, concat);

        //Realizamos preprocesamiento de los datasets en caso de ser la base de datos fingerprint
	if(database == "fingerprint") 
	{
		graphs_tr = PreprocessFingerPrint(graphs_tr);
		graphs_va = PreprocessFingerPrint(graphs_va);
		graphs_ts = PreprocessFingerPrint(graphs_ts);
	}

	time_t time_start;
	clock_t time_init, time_end;
	std::string filedist = "distances", fileinfo = "info";

	std::ofstream myfile;
	myfile.open( (fileout + "-" + filedist + (formatnumeric?"-num":"") + ".txt").c_str() );

	double d = 0;
	int i, j;
	std::vector<Graph *> graphs;

	time (&time_start);
	std::cout << "The start time is: " << ctime (&time_start) << std::endl;

	//Copiamos grafos del conjunto de validacion
	for(i = 0; i < (int)graphs_va.size(); i++)
		graphs_tr.push_back(graphs_va[i]);
	//Copiamos grafos del conjunto de prueba
	for(i = 0; i < (int)graphs_ts.size(); i++)
		graphs_tr.push_back(graphs_ts[i]);

	int n = graphs_tr.size(), c = 1;
	double diffticks,diffms;

	//Calculamos las distancias de edición
	for(i = 0; i < n ; i++)
	{
		for (j = i; j < n; j++)
		{
			time_init = clock();
			//Calculamos la distancia de edición
			d = compare(database, graphs_tr[i], graphs_tr[j], alpha, tnode, tedge, w_substedge, false);
			time_end = clock();

                        //Calculamos tiempo que le toma por calcular la distancia de edición
			diffticks = time_end - time_init;
			diffms = (diffticks) / (CLOCKS_PER_SEC/1000);

			//Guardamos en archivo la informacion referente a la distancia de edición en grafos
                        if(!formatnumeric) //verificamos si es necesario imprimir en formato de enteros o podemos imprimirlo en cadena
                            myfile << graphs_tr[i]->GetId() << "," << graphs_tr[j]->GetId() << "," << d << "," << diffms << std::endl;
                        else
                            myfile << i << "," << j << "," << d << "," << diffms << std::endl;
			
			c++;
		}
	}

	myfile.close();

	time(&time_end);

	//Guardamos informacion referente a toda la ejecución
	myfile.open( (fileout + "-" + fileinfo + ".txt").c_str() );
	myfile << "The start time is: " << ctime (&time_start) << std::endl;
	myfile << "\nThe finish time is: " << ctime (&time_end) << std::endl;
	myfile << difftime(time_end, time_start) << " seconds. \n";
	myfile.close();

	std::cout << "\nThe finish time is: " << ctime (&time_end) << std::endl;
	std::cout << difftime(time_end, time_start) << " seconds. \n";
}

/**
 * ClassifyWithKnn. Método de clasificación con K- Nearest Neighbor. Prueba normal con las bases de datos.
 * @param path: Ruta del archivo
 * @param database: nombre de la base de datos, para su uso en la selección de la GED.
 * @param alpha: 
 * @param tnode: costo de nodos.
 * @param tedge: costo de aristas.
 * @param w_substedge: costo sustitucion aritas.
 * @param label: etiqueta de lectura de grafos en cxl.
 * @param K: el K vecinos mas cercanos utilizados en KNN.
 * @param concatid
 */
void ClassifyWithKnn(std::string path, std::string database, double alpha, double tnode, double tedge, double w_substedge, std::string label, int K, bool concatid)
{
    //Leemos listado de grafos de entrenamiento y prueba
    std::vector<Graph *> graphs_tr = readlistgraphs(path,"train.cxl",label, concatid);
    std::vector<Graph *> graphs_ts = readlistgraphs(path,"test.cxl",label, concatid);

    //Si es la base de datos fingerprint, hacemos preprocesamiento cambiando las etiquetas de la
    //clase (revisar tesis o papers de riesen donde menciona que hace este cambio).
    if(database == "fingerprint")
    {
            graphs_tr = PreprocessFingerPrint(graphs_tr);
            graphs_ts = PreprocessFingerPrint(graphs_ts);
    }

    time_t time1, time2;
    time (&time1);
    std::cout << "The start time is: " << ctime (&time1) << std::endl;

    //ejecutamos KNN
    KNN *knn = new KNN(alpha, tnode, tedge, w_substedge, false, database);
    knn->test(database, graphs_tr, graphs_ts,K);

    //Mostramos tiempos en la ejecución
    time (&time2);
    std::cout << "\nThe finish time is: " << ctime (&time2) << std::endl;
    double seconds = difftime(time2, time1);

    printf ("%.f seconds.\n", seconds);
}

/**
 * ClassifyWithKnnFromFile. Método utilizado para la clasificación de grafos con KNN
 * desde archivo.
 * @param filetrain
 * @param filetest
 * @param filegeds
 * @param K
 */
void ClassifyWithKnnFromFileWithKFold(std::string pathtrain, std::string pathprots, std::string pathtest, std::string pathout, std::string filegeds, int K, int nfolds, int seed, bool onlyprot)
{
    //Convertimos el K a string
    std::stringstream s_k;
    s_k << K;
    //Genearamos el archivo donde se guardaran los resultados
    std::string fileresults = pathout + "-KNN-" + s_k.str() + ".txt";
    std::ofstream myFileResults;
    //Generamos archivo arff y txt
    myFileResults.open(fileresults.c_str());
    
    //Leemos GED's
    myFileResults << "Leyendo distancias..." << std::endl;
    std::map<std::string, double> distances = ReadGEDs(filegeds);
    
    std::vector<std::vector<std::string> > gr_tr;
    std::vector<std::vector<std::string> > gr_pr;
    std::vector<std::vector<std::string> > gr_ts;
    std::vector<Graph *> graphs_tr;
    std::vector<Graph *> graphs_pr;
    std::vector<Graph *> graphs_ts;
    
    time_t time1, time2;
    //Creamos objeto KNN
    KNN *knn = new KNN(distances);
    double acctrain, accprots, ret;
    double avgacctrain, avgaccprots, avgret;
    std::string filetrain, fileprots, filetest;
    avgacctrain = 0;
    avgaccprots = 0;
    avgret = 0;
    
    myFileResults << "\t\t\t\t\t\t\t\t\tOrig.\t\t\tGIS" << std::endl;
    myFileResults << "Fold\t\t\t\t\t\t\tAccuracy\tRet.\t\tAccuracy\tRet.\t\tTime." << std::endl;
    
    for(int s = 0; s < seed; s++)
    {
        for(int f = 0; f < nfolds; f++)
        {
            std::stringstream s_fold, s_seed;
            s_fold << (f+1);
            s_seed << s;
            
            //Leemos los conjuntos de entrenamiento y prueba
            filetrain = pathtrain + "-train-fold-" + s_fold.str() + "-seed-" + s_seed.str() + ".csv";
            gr_tr = ReadCSV(filetrain);
            fileprots = pathprots + "-train-fold-" + s_fold.str() + "-seed-" + s_seed.str() + ".csv";
            gr_pr = ReadCSV(fileprots);
            filetest = pathtest + "-test-fold-" + s_fold.str() + "-seed-" + s_seed.str() + ".csv";
            gr_ts = ReadCSV(filetest);
            
            //creamos grafos, para pasar como parametro a la función KNN
            //Copiamos conjunto de entrenamiento
            for(int i = 0; i < gr_tr.size(); i++)
            {
                //Creamos grafo y añadimos a lista
                Graph *g = new Graph(gr_tr[i][0], gr_tr[i][1]);
                graphs_tr.push_back(g);
            }
            
            //Copiamos conjunto de prueba
            for(int i = 0; i < gr_pr.size(); i++)
            {
                //Creamos grafo y añadimos a lista
                Graph *g = new Graph(gr_pr[i][0], gr_pr[i][1]);
                graphs_pr.push_back(g);
            }
            
            //Copiamos conjunto de prueba
            for(int i = 0; i < gr_ts.size(); i++)
            {
                //Creamos grafo y añadimos a lista
                Graph *g = new Graph(gr_ts[i][0], gr_ts[i][1]);
                graphs_ts.push_back(g);
            }

            time(&time1);
            
            if(!onlyprot)
            {
                //ejecutamos KNN con conjunto de entrenamiento original (FOLD)
                knn->test(filetrain + ", " + filetest, graphs_tr, graphs_ts, K);
                acctrain = knn->getAccuracy();
            }
            
            //ejecutamos KNN con seleccion de prototipos
            knn->test(fileprots + ", " + filetest, graphs_pr, graphs_ts, K);
            accprots = knn->getAccuracy();
            
            //Calculamos valor de retención
            ret = ((double)graphs_pr.size()/(double)graphs_tr.size())*100;
            
            ctime (&time2);
            //imprimimos informacion
            myFileResults << filetrain << "\t" << acctrain << "\t\t100.00\t\t" << accprots << "\t\t" << ret << "\t\t" << difftime(time2, time1) << " seg." << std::endl;

            avgacctrain += acctrain;
            avgaccprots += accprots;
            avgret += ret;
            
            //Limpiamos variables
            graphs_tr.clear();
            graphs_pr.clear();
            graphs_ts.clear();
            gr_tr.clear();
            gr_pr.clear();
            gr_ts.clear();
        }
    }
    avgacctrain /= (double)(nfolds*seed);
    avgaccprots /= (double)(nfolds*seed);
    avgret /= (double)(nfolds*seed);
    
    myFileResults << "Average:\t\t\t\t\t\t" << avgacctrain << "\t\t100.00\t\t" << avgaccprots << "\t\t" << avgret << std::endl;
    myFileResults << ctime (&time2)<< std::endl;
    
    //Limpiamos distancias
    distances.clear();
    
    myFileResults.close();
}




int main() {

    //readCsv("resources/csv/letter-low.csv");
    //readGeds("resources/distances/letter-low-distances.txt");

    //GraphsToFile("resources/data/letters/low/", "letter", "fingerprints", false, "resources/num/letter-low", "num");
    //GraphsToFile("resources/data/letters/med/", "letter","fingerprints", false,"resources/num/letter-med", "num");
    //GraphsToFile("resources/data/letters/high/", "letter", "fingerprints", false, "resources/num/letter-high", "num");
    //GraphsToFile("resources/data/fingerprint/", "fingerprint", "fingerprints", true, "resources/num/fingerprint", "num");
    //GraphsToFile("resources/data/grec/", "grec", "fingerprints", true, "resources/num/grec", "num");
    //GraphsToFile("resources/data/protein/", "protein", "enzymes", false, "resources/arff/protein", "arff");
    //GraphsToFile("resources/data/aids/", "aids", "fingerprints", false, "resources/num/aids", "num");
    //GraphsToFile("resources/data/mutagenicity/", "aids", "mutagenicity", false, "resources/num/mutagenicity", "num");
    
    /*
    GraphsToFile("resources/data/letters/low/", "letter", "fingerprints", false, "resources/arff/letter-low", "arff");
    GraphsToFile("resources/data/letters/med/", "letter","fingerprints", false,"resources/arff/letter-med", "arff");
    GraphsToFile("resources/data/letters/high/", "letter", "fingerprints", false, "resources/arff/letter-high", "arff");
    GraphsToFile("resources/data/fingerprint/", "fingerprint", "fingerprints", true, "resources/arff/fingerprint", "arff");
    GraphsToFile("resources/data/grec/", "grec", "fingerprints", true, "resources/arff/grec", "arff");
    GraphsToFile("resources/data/protein/", "protein", "enzymes", false, "resources/arff/protein", "arff");
    GraphsToFile("resources/data/aids/", "aids", "fingerprints", false, "resources/arff/aids", "arff");
    GraphsToFile("resources/data/mutagenicity/", "aids", "mutagenicity", false, "resources/arff/mutagenicity", "arff");
    */
    
    //GraphsToFile("resources/data/letters/low/", "letter", "fingerprints", false, "resources/csv/letter-low", "csv");
    //GraphsToFile("resources/data/letters/med/", "letter","fingerprints", false,"resources/csv/letter-med", "csv");
    //GraphsToFile("resources/data/letters/high/", "letter", "fingerprints", false, "resources/csv/letter-high", "csv");
    //GraphsToFile("resources/data/fingerprint/", "fingerprint", "fingerprints", true, "resources/csv/fingerprint", "csv");
    //GraphsToFile("resources/data/grec/", "grec", "fingerprints", true, "resources/csv/grec", "csv");
    //GraphsToFile("resources/data/protein/", "protein", "enzymes", false, "resources/csv/protein", "csv");
    //GraphsToFile("resources/data/aids/", "aids", "fingerprints", false, "resources/csv/aids", "csv");
    //GraphsToFile("resources/data/mutagenicity/", "aids", "mutagenicity", false, "resources/csv/mutagenicity", "csv");

    //GEDToFile("resources/data/letters/low/", "letter", 0.75, 0.3, 0.7, 2, "fingerprints", 5, false, "resources/geds/letter-low", true);
    //GEDToFile("resources/data/letters/med/", "letter", 0.25, 0.7, 1.9, 2, "fingerprints", 5, false, "resources/geds/letter-med", true);
    //GEDToFile("resources/data/letters/high/", "letter", 0.25, 0.9, 1.7, 2, "fingerprints", 5, false, "resources/geds/letter-high", true);
    //GEDToFile("resources/data/fingerprint/", "fingerprint", 0.75, 0.7, 0.5, 2, "fingerprints", 3, true, "resources/geds/fingerprint", true);
    //GEDToFile("resources/data/grec/", "grec", 0.75, 90, 15, 1, "fingerprints", 3, true, "resources/geds/grec", true);
    //GEDToFile("resources/data/protein/", "protein", 0.75, 11, 1.0, 2, "enzymes", 1, false, "resources/geds/protein", true);
    //GEDToFile("resources/data/aids/", "aids", 0.25, 1.1, 0.1, 2, "fingerprints", 1, false, "resources/geds/aids", true);
    //GEDToFile("resources/data/mutagenicity/", "aids", 0.25, 11, 1.1, 2, "mutagenicity", 5, false, "resources/geds/mutagenicity", true);
    
    //Archivos ARFF faltantes
    //WriteCSVToARFF_FilesKFold("resources/psr/letter-low-psr", "resources/psr/letter-low-psr", 5, 2);
    //WriteCSVToARFF_FilesKFold("resources/psr/letter-med-psr", "resources/psr/letter-med-psr", 5, 2);
    //WriteCSVToARFF_FilesKFold("resources/psr/letter-high-psr", "resources/psr/letter-high-psr", 5, 2);
    //WriteCSVToARFF_FilesKFold("resources/psr/grec-psr", "resources/psr/grec-psr", 5, 2);
    //WriteCSVToARFF_FilesKFold("resources/psr/fingerprint-psr", "resources/psr/fingerprint-psr", 5, 2);
    //WriteCSVToARFF_FilesKFold("resources/psr/protein-psr", "resources/psr/protein-psr", 5, 2);
    //WriteCSVToARFF_FilesKFold("resources/psr/aids-psr", "resources/psr/aids-psr", 5, 2);
    
    
    //---Calculo de KNN en el espacio de grafos para todas las bases de datos con K-Fold Cross 5x2 (CONJUNTO COMMPLETO)
    
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/geds/letter-low-distances-num.txt", 1, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/geds/letter-low-distances-num.txt", 3, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/geds/letter-low-distances-num.txt", 5, 5, 2);
    
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/geds/letter-med-distances-num.txt", 1, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/geds/letter-med-distances-num.txt", 3, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/geds/letter-med-distances-num.txt", 5, 5, 2);
    
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/geds/letter-high-distances-num.txt", 1, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/geds/letter-high-distances-num.txt", 3, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/geds/letter-high-distances-num.txt", 5, 5, 2);
    
    //ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/geds/grec-distances-num.txt", 1, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/geds/grec-distances-num.txt", 3, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/geds/grec-distances-num.txt", 5, 5, 2);
    
    //ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/geds/fingerprint-distances-num.txt", 1, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/geds/fingerprint-distances-num.txt", 3, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/geds/fingerprint-distances-num.txt", 5, 5, 2);
    
    //ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/geds/protein-distances-num.txt", 1, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/geds/protein-distances-num.txt", 3, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/geds/protein-distances-num.txt", 5, 5, 2);
    
    //ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/geds/aids-distances-num.txt", 1, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/geds/aids-distances-num.txt", 3, 5, 2);
    //ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/geds/aids-distances-num.txt", 5, 5, 2);
    
    
    //---- Ejecuta PSR con GEDs calculadas y KFoldCross estratificado
    
    //PSRWithGEDsAndKFold("resources/folds/letter-low", "resources/geds/letter-low-distances-num.txt", "resources/psr/letter-low-psr", 5, 2);
    //PSRWithGEDsAndKFold("resources/folds/letter-med", "resources/geds/letter-med-distances-num.txt", "resources/psr/letter-med-psr", 5, 2);
    //PSRWithGEDsAndKFold("resources/folds/letter-high", "resources/geds/letter-high-distances-num.txt", "resources/psr/letter-high-psr", 5, 2);
    //PSRWithGEDsAndKFold("resources/folds/grec", "resources/geds/grec-distances-num.txt", "resources/psr/grec-psr", 5, 2);
    //PSRWithGEDsAndKFold("resources/folds/fingerprint", "resources/geds/fingerprint-distances-num.txt", "resources/psr/fingerprint-psr", 5, 2);
    //PSRWithGEDsAndKFold("resources/data/protein/", "resources/folds/protein", "resources/geds/protein-distances-num.txt", "resources/psr/protein-psr", 5, 2);
    //PSRWithGEDsAndKFold("resources/data/aids/", "resources/folds/aids", "resources/geds/aids-distances-num.txt", "resources/psr/aids-psr", 5, 2);
    
    //---- Ejecuta PSC con GEDs calculadas y KFoldCross estratificado
    
    //PSCWithGEDsAndKFold("resources/data/letters/low/","resources/folds/letter-low", "resources/geds/letter-low-distances-num.txt", "resources/psc/letter-low-psc", 5, 2);
    //PSCWithGEDsAndKFold("resources/data/letters/med/","resources/folds/letter-med", "resources/geds/letter-med-distances-num.txt", "resources/psc/letter-med-psc", 5, 2);
    //PSCWithGEDsAndKFold("resources/data/letters/high/","resources/folds/letter-high", "resources/geds/letter-high-distances-num.txt", "resources/psc/letter-high-psc", 5, 2);
    //PSCWithGEDsAndKFold("resources/data/grec/", "resources/folds/grec", "resources/geds/grec-distances-num.txt", "resources/psc/grec-psc", 5, 2);
    //PSCWithGEDsAndKFold("resources/data/fingerprint/","resources/folds/fingerprint", "resources/geds/fingerprint-distances-num.txt", "resources/psc/fingerprint-psc", 5, 2);
    //PSCWithGEDsAndKFold("resources/data/protein/","resources/folds/protein", "resources/geds/protein-distances-num.txt", "resources/psc/protein-psc", 5, 2);
    //PSCWithGEDsAndKFold("resources/data/aids/","resources/folds/aids", "resources/geds/aids-distances-num.txt", "resources/psc/aids-psc", 5, 2);
    
    
    //--- Selección de prototipos discriminativos con K-FOld Cross Validation
    float WsLetterlow[5]= {0.11, 0.01, 0.10, 0.53, 0.01};
    float WsLetterMed[5] =      {0.10, 0.60, 0.01, 0.93, 0.05};
    float WsLetterHigh[5] =    {0.13, 0.14, 0.01, 0.79, 0.01};
    float WsGrec[5] =           {0.50, 0.50, 0.50, 0.50, 0.50};
    float WsFingerprint[5] =   {0.02, 0.27, 0.15, 0.20, 0.02};
    float WsAids[5] =           {0.46, 0.46, 0.01, 0.42, 0.01};
    float WsProtein[5] =        {0.04, 0.01, 0.05, 0.01, 0.01};
    //float WsMutagenicity[5] =   {0.82, 0.19, 0.30, 0.15, 0.01};
    
    //SPS--PSDWithGEDsAndKFold("resources/folds/letter-low", "resources/geds/letter-low-distances-num.txt", WsLetterlow, 15, "resources/psd/letter-low-psd", 5, 2);
    //SPS--PSDWithGEDsAndKFold("resources/folds/letter-med", "resources/geds/letter-med-distances-num.txt", WsLetterMed, 30, "resources/psd/letter-med-psd", 5, 2);
    //SPS--PSDWithGEDsAndKFold("resources/folds/letter-high", "resources/geds/letter-high-distances-num.txt", WsLetterHigh, 30, "resources/psd/letter-high-psd",5 , 2);
    //SPS--PSDWithGEDsAndKFold("resources/folds/grec", "resources/geds/grec-distances-num.txt", WsGrec, 10, "resources/psd/grec-psd", 5, 2);
    //PSDWithGEDsAndKFold("resources/folds/fingerprint", "resources/geds/fingerprint-distances-num.txt", WsFingerprint, 100, "resources/psd/fingerprint-psd", 5, 2);
    //SPS--PSDWithGEDsAndKFold("resources/folds/aids", "resources/geds/aids-distances-num.txt", WsAids, 50, "resources/psd/aids-psd", 5, 2);
    //PSDWithGEDsAndKFold("resourcs/folds/protein", "resources/geds/protein-distances-num.txt", WsProtein, 30, "resources/psd/protein-psd", 5, 2);

    //-----Escribir archivo .DAT generado por DROP3 y DROP5 
    //WriteDATtoCSVAndARFF("resources/drop/LETTER-LOW-traindrop3.dat", "resources/folds/letter-low", 5, 2, "resources/drop/letter-low-drop3");
    //WriteDATtoCSVAndARFF("resources/drop/LETTER-LOW-traindrop5.dat", "resources/folds/letter-low", 5, 2, "resources/drop/letter-low-drop5");
    //WriteDATtoCSVAndARFF("resources/drop/LETTER-MED-traindrop3.dat", "resources/folds/letter-med", 5, 2, "resources/drop/letter-med-drop3");
    //WriteDATtoCSVAndARFF("resources/drop/LETTER-MED-traindrop5.dat", "resources/folds/letter-med", 5, 2, "resources/drop/letter-med-drop5");
    //WriteDATtoCSVAndARFF("resources/drop/LETTER-HIGH-traindrop3.dat", "resources/folds/letter-high", 5, 2, "resources/drop/letter-high-drop3");
    //WriteDATtoCSVAndARFF("resources/drop/LETTER-HIGH-traindrop5.dat", "resources/folds/letter-high", 5, 2, "resources/drop/letter-high-drop5");
    //WriteDATtoCSVAndARFF("resources/drop/GREC-traindrop3.dat", "resources/folds/grec", 5, 2, "resources/drop/grec-drop3");
    //WriteDATtoCSVAndARFF("resources/drop/GREC-traindrop5.dat", "resources/folds/grec", 5, 2, "resources/drop/grec-drop5");
    //WriteDATtoCSVAndARFF("resources/drop/FINGERPRINT-traindrop3.dat", "resources/folds/fingerprint", 5, 2, "resources/drop/fingerprint-drop3");
    //WriteDATtoCSVAndARFF("resources/drop/FINGERPRINT-traindrop5.dat", "resources/folds/fingerprint", 5, 2, "resources/drop/fingerprint-drop5");
    //WriteDATtoCSVAndARFF("resources/drop/AIDS-traindrop3.dat", "resources/folds/aids", 5, 2, "resources/drop/aids-drop3");
    //WriteDATtoCSVAndARFF("resources/drop/AIDS-traindrop5.dat", "resources/folds/aids", 5, 2, "resources/drop/aids-drop5");
    //WriteDATtoCSVAndARFF("resources/drop/PROTEIN-traindrop3.dat", "resources/folds/protein", 5, 2, "resources/drop/protein-drop3");
    //WriteDATtoCSVAndARFF("resources/drop/PROTEIN-traindrop5.dat", "resources/folds/protein", 5, 2, "resources/drop/protein-drop5");
    
    
    //---Calculo de KNN en el espacio de grafos para todas las bases de datos con K-Fold Cross 5x2 (PSR)
    /*
    for(int k = 5; k <= 5 ; k+=2)
    {
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/psr/letter-low-psr", "resources/folds/letter-low", "resources/psr/graphs/classification/letter-low-psr", "resources/geds/letter-low-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/psr/letter-med-psr", "resources/folds/letter-med", "resources/psr/graphs/classification/letter-med-psr", "resources/geds/letter-med-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/psr/letter-high-psr", "resources/folds/letter-high", "resources/psr/graphs/classification/letter-high-psr", "resources/geds/letter-high-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/psr/grec-psr", "resources/folds/grec", "resources/psr/graphs/classification/grec-psr", "resources/geds/grec-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/psr/protein-psr", "resources/folds/protein", "resources/psr/graphs/classification/protein-psr", "resources/geds/protein-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/psr/fingerprint-psr", "resources/folds/fingerprint", "resources/psr/graphs/classification/fingerprint-psr", "resources/geds/fingerprint-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/psr/aids-psr", "resources/folds/aids", "resources/psr/graphs/classification/aids-psr", "resources/geds/aids-distances-num.txt", k, 5, 2, false);        
    }
    */
    
    
    
    
    //---Calculo de KNN en el espacio de grafos para todas las bases de datos con K-Fold Cross 5x2 (PSC)
    
    /*for(int k = 5; k <= 5 ; k+=2)
    {
        
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/psc/letter-low-psc", "resources/folds/letter-low", "resources/psc/graphs/classification/letter-low-psc", "resources/geds/letter-low-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/psc/letter-med-psc", "resources/folds/letter-med", "resources/psc/graphs/classification/letter-med-psc", "resources/geds/letter-med-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/psc/letter-high-psc", "resources/folds/letter-high", "resources/psc/graphs/classification/letter-high-psc", "resources/geds/letter-high-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/psc/grec-psc", "resources/folds/grec", "resources/psc/graphs/classification/grec-psc", "resources/geds/grec-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/psc/protein-psc", "resources/folds/protein", "resources/psc/graphs/classification/protein-psc", "resources/geds/protein-distances-num.txt", k, 5, 2, false); 
        ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/psc/fingerprint-psc", "resources/folds/fingerprint", "resources/psc/graphs/classification/fingerprint-psc", "resources/geds/fingerprint-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/psc/aids-psc", "resources/folds/aids", "resources/psc/graphs/classification/aids-psc", "resources/geds/aids-distances-num.txt", k, 5, 2, false);
    }*/
    
    
    
    
    //---Calculo de KNN en el espacio de grafos para todas las bases de datos con K-Fold Cross 5x2 (IRB)
    /*
    for(int k = 5; k <= 5 ; k+=2)
    {
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/irb/letter-low-irb", "resources/folds/letter-low", "resources/irb/graphs/classification/letter-low-irb", "resources/geds/letter-low-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/irb/letter-med-irb", "resources/folds/letter-med", "resources/irb/graphs/classification/letter-med-irb", "resources/geds/letter-med-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/irb/letter-high-irb", "resources/folds/letter-high", "resources/irb/graphs/classification/letter-high-irb", "resources/geds/letter-high-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/irb/grec-irb", "resources/folds/grec", "resources/irb/graphs/classification/grec-irb", "resources/geds/grec-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/irb/protein-irb", "resources/folds/protein", "resources/irb/graphs/classification/protein-irb", "resources/geds/protein-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/irb/fingerprint-irb", "resources/folds/fingerprint", "resources/irb/graphs/classification/fingerprint-irb", "resources/geds/fingerprint-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/irb/aids-irb", "resources/folds/aids", "resources/irb/graphs/classification/aids-irb", "resources/geds/aids-distances-num.txt", k, 5, 2, false);        
    }
    */
    
    //---Calculo de KNN en el espacio de grafos para todas las bases de datos con K-Fold Cross 5x2 (DROP3)
    
    /*
    for(int k = 5; k <= 5 ; k+=2)
    {
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/drop/letter-low-drop3", "resources/folds/letter-low", "resources/drop/graphs/drop3/classification/letter-low-drop3", "resources/geds/letter-low-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/drop/letter-med-drop3", "resources/folds/letter-med", "resources/drop/graphs/drop3/classification/letter-med-drop3", "resources/geds/letter-med-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/drop/letter-high-drop3", "resources/folds/letter-high", "resources/drop/graphs/drop3/classification/letter-high-drop3", "resources/geds/letter-high-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/drop/grec-drop3", "resources/folds/grec", "resources/drop/graphs/drop3/classification/grec-drop3", "resources/geds/grec-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/drop/protein-drop3", "resources/folds/protein", "resources/drop/graphs/drop3/classification/protein-drop3", "resources/geds/protein-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/drop/fingerprint-drop3", "resources/folds/fingerprint", "resources/drop/graphs/drop3/classification/fingerprint-drop3", "resources/geds/fingerprint-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/drop/aids-drop3", "resources/folds/aids", "resources/drop/graphs/drop3/classification/aids-drop3", "resources/geds/aids-distances-num.txt", k, 5, 2, false);
    }
    */
    

    //---Calculo de KNN en el espacio de grafos para todas las bases de datos con K-Fold Cross 5x2 (DROP5)
    
    /*
    for(int k = 5; k <= 5 ; k+=2)
    {
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/drop/letter-low-drop5", "resources/folds/letter-low", "resources/drop/graphs/drop5/classification/letter-low-drop5", "resources/geds/letter-low-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/drop/letter-med-drop5", "resources/folds/letter-med", "resources/drop/graphs/drop5/classification/letter-med-drop5", "resources/geds/letter-med-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/drop/letter-high-drop5", "resources/folds/letter-high", "resources/drop/graphs/drop5/classification/letter-high-drop5", "resources/geds/letter-high-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/drop/grec-drop5", "resources/folds/grec", "resources/drop/graphs/drop5/classification/grec-drop5", "resources/geds/grec-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/drop/protein-drop5", "resources/folds/protein", "resources/drop/graphs/drop5/classification/protein-drop5", "resources/geds/protein-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/drop/fingerprint-drop5", "resources/folds/fingerprint", "resources/drop/graphs/drop5/classification/fingerprint-drop5", "resources/geds/fingerprint-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/drop/aids-drop5", "resources/folds/aids", "resources/drop/graphs/drop5/classification/aids-drop5", "resources/geds/aids-distances-num.txt", k, 5, 2, false);        
    }
     */
    
    
    
    //---Calculo de KNN en el espacio de grafos para todas las bases de datos con K-Fold Cross 5x2 (PSD-SPS)

    
    for(int k = 1; k <= 5 ; k+=2)
    {
        //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-low", "resources/psd/letter-low-psd", "resources/folds/letter-low", "resources/psd/graphs/classification/letter-low-psd", "resources/geds/letter-low-distances-num.txt", k, 5, 2, false);
        //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-med", "resources/psd/letter-med-psd", "resources/folds/letter-med", "resources/psd/graphs/classification/letter-med-psd", "resources/geds/letter-med-distances-num.txt", k, 5, 2, false);
        //ClassifyWithKnnFromFileWithKFold("resources/folds/letter-high", "resources/psd/letter-high-psd", "resources/folds/letter-high", "resources/psd/graphs/classification/letter-high-psd", "resources/geds/letter-high-distances-num.txt", k, 5, 2, false);
        //ClassifyWithKnnFromFileWithKFold("resources/folds/grec", "resources/psd/grec-psd", "resources/folds/grec", "resources/psd/graphs/classification/grec-psd", "resources/geds/grec-distances-num.txt", k, 5, 2, false);
        ClassifyWithKnnFromFileWithKFold("resources/folds/protein", "resources/psd/protein-psd", "resources/folds/protein", "resources/psd/graphs/classification/protein-psd", "resources/geds/protein-distances-num.txt", k, 5, 2, false);
        //ClassifyWithKnnFromFileWithKFold("resources/folds/fingerprint", "resources/psd/fingerprint-psd", "resources/folds/fingerprint", "resources/psd/graphs/classification/fingerprint-psd", "resources/geds/fingerprint-distances-num.txt", k, 5, 2, false);
        //ClassifyWithKnnFromFileWithKFold("resources/folds/aids", "resources/psd/aids-psd", "resources/folds/aids", "resources/psd/graphs/classification/aids-psd", "resources/geds/aids-distances-num.txt", k, 5, 2, false);        
    }
    
    
    //--------------------------------------------------------------------------------------------
    //------------------------------- OTRO TIPO DE EJECUCIONES -----------------------------------
    //--------------------------------------------------------------------------------------------
    //===========================================================================================
    
    //------- Ejecuta PSC cuando las distancias ya han sido calculadas y se leen desde archivo
    
    //PSCWithGEDsCalculated("resources/csv/letter-low.csv", "resources/geds/letter-low-distances-num.txt", "resources/psc/graphs/letter-low-psc");
    //PSCWithGEDsCalculated("resources/csv/letter-med.csv", "resources/geds/letter-med-distances-num.txt", "resources/psr/graphs/letter-med-psr");
    //PSCWithGEDsCalculated("resources/csv/letter-high.csv", "resources/geds/letter-high-distances-num.txt", "resources/psr/graphs/letter-high-psr");
    //PSCWithGEDsCalculated("resources/csv/fingerprint.csv", "resources/geds/fingerprint-distances-num.txt", "resources/psr/graphs/fingerprint-psr");
    //PSCWithGEDsCalculated("resources/csv/grec.csv", "resources/geds/grec-distances-num.txt", "resources/psr/graphs/grec-psr");
    //PSCWithGEDsCalculated("resources/csv/protein.csv", "resources/geds/protein-distances-num.txt", "resources/psr/graphs/protein-psr");
    //PSCWithGEDsCalculated("resources/csv/aids.csv", "resources/geds/aids-distances-num.txt", "resources/psr/graphs/aids-psr");

    
    //------- Ejecuta PSR cuando las distancias ya han sido calculadas y se leen desde archivo
    
    //PSRWithGEDsCalculated("resources/csv/letter-low.csv", "resources/geds/letter-low-distances.txt", "resources/psr/graphs/letter-low-psr");
    //PSRWithGEDsCalculated("resources/csv/letter-med.csv", "resources/geds/letter-med-distances.txt", "resources/psr/graphs/letter-med-psr");
    //PSRWithGEDsCalculated("resources/csv/letter-high.csv", "resources/geds/letter-high-distances.txt", "resources/psr/graphs/letter-high-psr");
    //PSRWithGEDsCalculated("resources/csv/fingerprint.csv", "resources/geds/fingerprint-distances.txt", "resources/psr/graphs/fingerprint-psr");
    //PSRWithGEDsCalculated("resources/csv/grec.csv", "resources/geds/grec-distances.txt", "resources/psr/graphs/grec-psr");
    //PSRWithGEDsCalculated("resources/csv/protein.csv", "resources/geds/protein-distances.txt", "resources/psr/graphs/protein-psr");
    //PSRWithGEDsCalculated("resources/csv/aids.csv", "resources/geds/aids-distances.txt", "resources/psr/graphs/aids-psr");
    //PSRWithGEDsCalculated("resources/csv/mutagenicity.csv", "resources/geds/mutagenicity-distances.txt", "resources/psr/graphs/mutagenicity-psr");

    
    //---Prototipos discriminativos PSD sin KFold
    
    //PSDWithGEDsCalculated("resources/csv/letter-low.csv", "resources/geds/letter-low-distances.txt", WsLetterlow, 10, "resources/psd/graphs/letter-low-psd");
    //PSDWithGEDsCalculated("resources/csv/letter-med.csv", "resources/geds/letter-med-distances.txt", WsLetterMed, 10, "resources/psd/graphs/letter-med-psd");
    //PSDWithGEDsCalculated("resources/csv/letter-high.csv", "resources/geds/letter-high-distances.txt", WsLetterHigh, 10, "resources/psd/graphs/letter-high-psd");
    //PSDWithGEDsCalculated("resources/csv/grec.csv", "resources/geds/grec-distances.txt", WsGrec, 10, "resources/psd/graphs/grec-psd");
    //PSDWithGEDsCalculated("resources/csv/fingerprint.csv", "resources/geds/fingerprint-distances.txt", WsFingerprints, 10, "resources/psd/graphs/fingerprint-psd");
    //PSDWithGEDsCalculated("resources/csv/aids.csv", "resources/geds/aids-distances.txt", WsAids, 10, "resources/psd/graphs/aids-psd");
    //PSDWithGEDsCalculated("resources/csv/mutagenicity.csv", "resources/geds/mutagenicity-distances.txt", WsMutagenicity, 10, "resources/psd/graphs/mutagenicity-psd");
    //PSDWithGEDsCalculated("resources/csv/protein.csv", "resources/geds/protein-distances.txt", WsProtein, 10, "resources/psd/graphs/protein-psd");

    
    //---Ejecucion normal de PSR
    
    //PSRNormalExecute("resources/data/letters/low/", "letter", 0.75, 0.3, 0.7, 2, "fingerprints", 5, false, "resources/psr/graphs/letter-low-psr");
    //PSRNormalExecute("resources/data/letters/med/", "letter", 0.25, 0.9, 1.7, 2, "fingerprints", 5, false, "resources/psr/graphs/letter-med-psr");
    //PSRNormalExecute("resources/data/letters/high/", "letter", 0.25, 0.9, 1.7, 2, "fingerprints", 5, false, "resources/psr/graphs/letter-high-psr");
    //PSRNormalExecute("resources/data/fingerprint/", "fingerprint", 0.25, 0.7, 1.5, 2, "fingerprints", 5, false, "resources/psr/graphs/fingerprint-psr");
    //PSRNormalExecute("resources/data/grec/", "grec", 0.50, 90, 15, 2, "fingerprints", 5, true, "resources/psr/graphs/grec-psr");
    //PSRNormalExecute("resources/data/protein/", "protein", 0.75, 11, 1.0, 2, "enzymes", 1, false, "resources/psr/graphs/protein-psr");
    //PSRNormalExecute("resources/data/aids/", "aids", 0.25, 1.1, 0.1, 2, "fingerprints", 1, false, "resources/psr/graphs/aids-psr");
    //PSRNormalExecute("resources/data/mutagenicity/", "aids", 0.25, 11, 1.1, 2, "mutagenicity", 5, false, "resources/psr/graphs/mutagenicity-psr");

    //---Clasificación normal con KNN
    
    //ClassifyWithKnn("resources/data/letters/low/", "letter", 0.75, 0.3, 0.7, 2, "fingerprints", 5, false);
    //ClassifyWithKnn("resources/data/letters/med/", "letter", 0.25, 0.7, 1.9, 2, "fingerprints", 5, false);
    //ClassifyWithKnn("resources/data/letters/high/", "letter", 0.25, 0.9, 1.7, 2, "fingerprints", 5, false);
    //ClassifyWithKnn("resources/data/fingerprint/", "fingerprint", 0.75, 0.7, 0.5, 2, "fingerprints", 3, false);
    //ClassifyWithKnn("resources/data/grec/", "grec", 0.75, 90, 15, 1, "fingerprints", 3, true);
    //ClassifyWithKnn("resources/data/protein/", "protein", 0.75, 11, 1.0, 2, "enzymes", 1, false);
    //ClassifyWithKnn("resources/data/aids/", "aids", 0.25, 1.1, 0.1, 2, "fingerprints", 1, false);
    //ClassifyWithKnn("resources/data/mutagenicity/", "aids", 0.25, 11, 1.1, 2, "mutagenicity", 5, false);

    
    return 0;
}

