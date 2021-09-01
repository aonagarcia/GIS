#ifndef GED_H_
#define GED_H_

#include "graph_edit_dist_letter.h"
#include "graph_edit_dist_fingerprint.h"
#include "graph_edit_dist_grec.h"
#include "graph_edit_dist_protein.h"
#include "graph_edit_dist_aids.h"

double compare(std::string database, Graph *g_source, Graph *g_dest, double alpha, double weigth_insdel_node, double weigth_insdel_edge, double weigth_subst_edge, bool print_details)
{
	AbstractGraphEditDistance *ged;
	if(database == "letter") ged = new GraphEditDistanceLetter(g_source, g_dest, alpha, weigth_insdel_node, weigth_insdel_edge, weigth_subst_edge);
	if(database == "fingerprint") ged = new GraphEditDistanceFingerprint(g_source, g_dest, alpha, weigth_insdel_node, weigth_insdel_edge, weigth_subst_edge);
	if(database == "grec") ged = new GraphEditDistanceGrec(g_source, g_dest, alpha, weigth_insdel_node, weigth_insdel_edge, weigth_subst_edge);
	if(database == "protein") ged = new GraphEditDistanceProtein(g_source, g_dest, alpha, weigth_insdel_node, weigth_insdel_edge, weigth_subst_edge);
	if(database == "aids") ged = new GraphEditDistanceAids(g_source, g_dest, alpha, weigth_insdel_node, weigth_insdel_edge, weigth_subst_edge);

    if(print_details)
        ged->print_matrix();
	double d = ged->normalized_distance();
	//Liberamos GED
	delete ged;

	return d;
}

#endif
