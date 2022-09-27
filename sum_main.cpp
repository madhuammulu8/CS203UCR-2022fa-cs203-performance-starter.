#include <iostream>
#include <cstdlib>
#include <sstream>
#include "archlab.h"
#include <unistd.h>
#include<algorithm>
#include<cstdint>
#include<unordered_set>
#include <cstring>
#include <vector>
#include <map>
#include "perfstats.h"
#include "fast_URBG.hpp"
#include <random>
#include "sum.h"

int main(int argc, char *argv[])
{
	
	std::vector<int> mhz_s;
	std::vector<int> default_mhz;
//	fastest << cpu_frequencies_array[0];
	std::vector<std::string> functions;
	std::vector<std::string> default_functions;
	std::vector<uint64_t> space_sizes;
	std::vector<uint64_t> default_space_sizes;
	std::vector<uint64_t> query_counts;
	std::vector<uint64_t> default_query_counts;
	std::stringstream available_functions;
        int i;
        int reps=1,mhz;
        
        char *stat_file = NULL;
        char default_filename[] = "stat.csv";
        char preamble[1024];
        char epilogue[1024];

	// Parse user commands.	
        for(i = 1; i < argc; i++)
        {
            // This is an option.
            if(argv[i][0]=='-')
            {
                switch(argv[i][1])
                {
                    case 'o':
		        if(i+1 < argc && argv[i+1][0]!='-')
                            stat_file = argv[i+1];
                        break;
                    case 'r':
		        if(i+1 < argc && argv[i+1][0]!='-')
                            reps = atoi(argv[i+1]);
                        break;
		    case 's':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
                        unsigned long int size;
		                size = atoi(argv[i+1]);
	                    space_sizes.push_back(size);
			    }
			    else
			        break;
		        }
		        break;
		    case 'q':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
                        unsigned long int size;
		                size = atoi(argv[i+1]);
	                    query_counts.push_back(size);
			        }
			        else
			            break;
		        }
		        break;
		    case 'f':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
	                    functions.push_back(std::string(argv[i+1]));
			        }
			        else
			            break;
		        }
		        break;
		    case 'M':
		        for(;i+1<argc;i++)
		        {
		            if(argv[i+1][0]!='-')
		            {
		                mhz = atoi(argv[i+1]);
	                        mhz_s.push_back(mhz);
			    }
			    else
			        break;
		        }
		        break;

		    case 'h':
		        break;
                }
            }
        }
	if(stat_file==NULL)
	    stat_file = default_filename;


	default_functions.push_back("sum_of_locations");
	default_space_sizes.push_back(16*128*1024);
	default_query_counts.push_back(4096);
	if(functions.size()==0)
	    functions = default_functions;
	if(space_sizes.size()==0)
	    space_sizes = default_space_sizes;
	if(query_counts.size()==0)
	    query_counts = default_query_counts;
    std::uniform_int_distribution<int> dist(1024, 1024*1024);
    std::random_device rd;
	uint64_t seed =dist(rd);;

	uint64_t max_space_size = *std::max_element(space_sizes.begin(), space_sizes.end());
	uint64_t * search_space = new uint64_t[max_space_size];
	for(uint64_t i = 0; i < max_space_size; i++) {
		search_space[i] = i;
	}
	std::shuffle(search_space, &search_space[max_space_size], fast_URBG(seed));
	std::map<const std::string, uint64_t(*)(uint64_t *, uint32_t, uint64_t* , uint32_t)>
		function_map =
		{
#define FUNCTION(n) {#n, n}
			FUNCTION(sum_of_locations_solution),
			FUNCTION(sum_of_locations)
		};
	// for(uint64_t a = 0; a < max_space_size; a++) {
	// 	std::cerr << search_space[a] << "\n";
	// }
	uint64_t max_query_count = *std::max_element(query_counts.begin(), query_counts.end());
	uint64_t * query_list = new uint64_t[max_query_count];
	uint64_t _seed = seed;
	for(uint i = 0; i < max_query_count; i++) {
		query_list[i] = fast_rand(&_seed) % (max_space_size * 2);
	}
    
	perfstats_print_header(stat_file, "size,rep,function,query_counts,IC,Cycles,CPI,MHz,CT,ET,cmdlineMHz,answer");

	for(auto &mhz: mhz_s) 
        {
	        change_cpufrequnecy(mhz);
//		set_cpu_clock_frequency(mhz);
		for(auto & space_size: space_sizes ) {			
			std::shuffle(search_space, &search_space[space_size], fast_URBG(seed));
			for(auto & query_count: query_counts ) {
				for(uint r = 0; r < reps; r++) {
					for(auto & function : functions) {
						uint64_t a = 0;
						//pristine_machine();					
						uint64_t answer;
						{								
				            sprintf(preamble, "%lu,%d,%s,%lu,", space_size, r, function.c_str(),query_count);
	                        perfstats_init();
						    perfstats_enable();
							//enable_prefetcher();
							//set_cpu_clock_frequency(cpu_frequencies[0]);
                            answer = function_map[function](search_space, space_size, query_list, query_count);
						    perfstats_disable();
				            sprintf(epilogue, ",%d,%lu\n",mhz,answer);
						    perfstats_print(preamble, stat_file, epilogue);
	                        perfstats_deinit();
						}

						std::cerr << a << "\n";
					}
				}
			}
		}
	        restore_cpufrequnecy();

	}
	delete [] search_space;
	delete [] query_list;
	std::cout << "Execution complete\n" ;
//	archlab_write_stats();
	return 0;
}
