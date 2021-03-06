#pragma once

#include "AlphaReeb_Parameters.h"
#include "Split_Into_Conn_Comps.h"
#include "Dijkstra.h"
#include "Generate_Subclouds.h"
#include "Generate_Subgraphs.h"
#include "Group_Subgraphs.h"
#include "Connect_Clusters.h"
#include "Generate_AlphaReeb_Graph.h"
#include "Combine_Comps.h"

void AlphaReeb_Algorithm ( Graph const& input_graph, AlphaReeb_Parameters const& parameters, Graph& alphaReeb_graph )
{
    int num_comps;
    vector<vector<Data_Pt>> conn_comp_cloud;
    vector<Graph> conn_comp;
    
    Split_Into_Conn_Comps( input_graph, num_comps, conn_comp_cloud, conn_comp ); // Splitting the input graph into connected components.
    
    vector<Graph> alphaReeb_comp( num_comps );
    
    double min_comp_size = boost::num_vertices( input_graph ) * parameters.mcsf;
    
    for (int counter = 0; counter < num_comps; ++counter) // Looping over connected components.
    {
        if (conn_comp_cloud[counter].size() < min_comp_size) continue; // Disregards components with fewer vertices than the value of min_comp_size.
        
        if (conn_comp_cloud[counter].size() == 1)
        {
            Graph::vertex_descriptor v = boost::add_vertex( alphaReeb_comp[counter] );
            alphaReeb_comp[counter][v].pt = conn_comp_cloud[counter][0].pt;
            continue;
        }
        
        Graph intermediate_graph;
        vector<vector<Data_Pt>> subcloud;
        vector<Graph> subgraph;
        vector<Cluster> cluster;
        multimap<double, int> dijkstra_multimap; // Key = distance, value = index.
        
        Dijkstra( conn_comp[counter], dijkstra_multimap ); // Assigning filter values to each point.
        
        Generate_Subclouds( conn_comp_cloud[counter], dijkstra_multimap, parameters.alpha, subcloud );
        
        Generate_Subgraphs( conn_comp[counter], subcloud, subgraph );
        
        Group_Subgraphs( subgraph, subcloud, cluster );
        
        Connect_Clusters( cluster, intermediate_graph );
        
        Generate_AlphaReeb_Graph( intermediate_graph, parameters.alpha, alphaReeb_comp[counter] );
    }
    
    Combine_Comps( alphaReeb_comp, alphaReeb_graph ); // Combining the components into a single graph.
}
