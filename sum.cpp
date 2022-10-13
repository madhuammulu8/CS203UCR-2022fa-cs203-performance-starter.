#include <cstdlib>
#include "archlab.h"
#include <unistd.h>
#include<algorithm>
#include<cstdint>
#include<unordered_set>
#include "perfstats.h"
#include "sum.h"

#define START_C extern "C" { // this just hides the braces from the editor, so it won't try to indent everything.
#define END_C  }


// START_C

// uint64_t sum_of_locations_solution(uint64_t *search_space, uint32_t search_space_size, uint64_t* queries, uint32_t query_count)
// {
// 	uint64_t r = 0;

// 	for(uint32_t i = 0; i < query_count; i++) {
// 		for(uint32_t j = 0; j < search_space_size; j++) {
// 			if (search_space[j] == queries[i]) {
// 				r += i;
// 				break;
// 			}
// 		}
// 	}
// 	return r;
// }

// END_C

START_C

uint64_t sum_of_locations_solution(uint64_t *search_space, uint32_t search_space_size, uint64_t *queries, uint32_t query_count)
{
	uint64_t r = 0;
    std::unordered_set<uint64_t> intSet;  
    for (uint32_t i = 0; i < search_space_size; i++)
    {
            intSet.insert(search_space[i]);
    }
  
		for(uint32_t i = 0; i < query_count; i++) {
			if (intSet.find(queries[i])>0) {
				r += i;
				// break;
			}
	}
	return r;
}

END_C

