#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "boruvka_parallel_star2.h"
#include "union_find.h"

#define THREADS 16

//when we say edge right here we mean that we parallelize
//over edges when finding the mins, (not edge contraction
//although the iniitial implementation might be edge contraction)
void find_MST_parallel_star2(Graph g){
    omp_set_num_threads(THREADS);
    int n = get_num_nodes(g);
    
    omp_lock_t *locks = new omp_lock_t[n];

    //store the edge index of the min weight edge incident on node i
    struct Edge* min_edges = new struct Edge[n];
    struct set *components = new struct set[n];

    bool can_be_contracted = true;

    struct Edge* mst_edges = new struct Edge[n];
    
    bool *coin_flips = new bool[n];
    bool *is_contracted = new bool[n];

    int mst_edges_idx = 0;
    //this is a hacky way to accommodate the fact that we look at every edge
    //even though we're contracting
    bool is_first_passes[n];
    #pragma omp parallel for schedule(static)
    for(int i = 0; i < n; i++){
        components[i].parent = i;
        components[i].rank = 0;
        is_first_passes[i] = true;
        omp_init_lock(&(locks[i]));
    }

    //continue looping until there's only 1 component
    //in the case of a disconnected graph, until num_components doesn't change
    while(can_be_contracted){
        #pragma omp parallel for shared(min_edges, locks) schedule(dynamic, THREADS)
        for(int i = 0; i < n; i++){
            const Vertex* start = edges_begin(g, i);
            const Vertex* end = edges_end(g,i);
            int weight_offset = -1;
            int set1 = find_parallel(components, i);
            for(const Vertex* v = start; v < end; v++){
                int set2 = find_parallel(components, *v);
                weight_offset++;
                if(set1 == set2)
                    continue;
                Edge e;
                e.src = i;
                e.dest = *v;
                e.weight = g->weights[g->offsets[i] + weight_offset];

                omp_set_lock(&(locks[set1]));

                if(is_first_passes[set1]){
                    min_edges[set1] = e;
                    is_first_passes[set1] = false;
                }
                else if(min_edges[set1].weight > e.weight)
                    min_edges[set1] = e;

                omp_unset_lock(&(locks[set1]));
            }
        }

        #pragma omp parallel for schedule(static)
        for(int i = 0; i < n; i++){
            coin_flips[i] = ((rand() % 2) == 1);
        }

        can_be_contracted = false;
        //contract based on min edges found 
        //uses edge contraction which we couldn't think of a good way
        //to parallelize
        #pragma omp parallel for schedule(dynamic, 256) 
        for(int i = 0; i < n; i++){
            int src = min_edges[i].src;
            int dest = min_edges[i].dest;

            int root1 = find_parallel(components, src);
            int root2 = find_parallel(components, dest);
            if(root1 == root2){
                continue;
            }
            can_be_contracted = true;
            if((coin_flips[root1] == coin_flips[root2])){
                continue;
            }
            if(coin_flips[root1]){
                if(!__sync_bool_compare_and_swap(&is_contracted[root2],false,true))
                    continue;
            }
            else{
                if(!__sync_bool_compare_and_swap(&is_contracted[root1],false,true))
                    continue;
            }
            if(coin_flips[root1]){
                union_parallel(components, root2, root1);
                mst_edges[root2] = min_edges[i];
            }
            else{
                union_parallel(components, root1, root2);
                mst_edges[root1] = min_edges[i];
            }
        }

        #pragma omp parallel for schedule(static)
        for(int i = 0; i < n; i++){
            is_first_passes[i] = true;
            is_contracted[i] = false;
        }
    }
/*
    for(int i = 0; i < n-1; i++){
        printf("%d,%d\n", mst_edges[i].src, mst_edges[i].dest);
    }
  */  
    delete[] min_edges;
    delete[] components;
}
