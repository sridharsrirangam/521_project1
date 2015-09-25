#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>
#include <fstream>
//#include <math.h> //used for log2
#include "class_definitions.h"

#define DEBUG 0 

using namespace std;

extern unsigned int L1_reads,L1_writes,L1_read_misses,L1_write_misses;

int main(int argc, char *argv[])
{ int block_size,L1_size,L1_assoc,vc_num_blocks,L2_size,L2_assoc;
  string trace_file;  
  string line;
  string trace;
  char *tag;
  char operation;
  unsigned int addr_file;
  int L1_number_of_sets,L2_number_of_sets;
  cout<<log2(4)<<endl; 
  if(argc!=8){
        cout<<"invalid arguments"<<endl;
       // exit(1);
    }
    //read arguments
    block_size=atoi(argv[1]);
    L1_size=atoi(argv[2]);
    L1_assoc=atoi(argv[3]);
    vc_num_blocks=atoi(argv[4]);
    L2_size=atoi(argv[5]);
    L2_assoc=atoi(argv[6]);
    trace_file=argv[7];
    L1_number_of_sets=((L1_size)/(L1_assoc*block_size));
   if((L2_size!=0)&&(L2_assoc!=0))
           {
    L2_number_of_sets=((L2_size)/(L2_assoc*block_size));
    }
    else
    { L2_number_of_sets=0;
    }

    //end of argument reads
#ifdef DEBUG
    cout<<"block size is "<<block_size<<endl;
    cout<<"L1 size is "<<L1_size<<endl;
    cout<<"L1 asscoc "<<L1_assoc<<endl;
    cout<<"vc is "<<vc_num_blocks<<endl;
    cout<<"L2 size is "<<L2_size<<endl;
    cout<<"L2_assoc "<<L2_assoc<<endl;
    cout<<"trace file "<<trace_file<<endl;
    cout<<"number of sets in L1 is "<<L1_number_of_sets<<endl;
    cout<<"number of sets in L2 is "<<L2_number_of_sets<<endl;

#endif
    cache_class L1_cache((L1_size/(L1_assoc*block_size)),L1_assoc,block_size);
    //L1_cache.cache_init(L1_number_of_sets,L1_assoc);
    cout<<L1_cache.set_incache[0].block_inset[0].valid_bit<<endl;
    ifstream myfile(argv[7]);
   if(myfile.is_open())
   {
       while(getline(myfile,line))
       {
         operation = line[0];
         trace=line.substr(2,8);
         addr_file=(unsigned int)strtoul(trace.c_str(),NULL,16);
         L1_cache.request_block(addr_file,operation);
#ifdef DEBUG
        // cout<<hex<<addr_file<<endl;
#endif
       }
       L1_cache.print_cache();
       cout<<"L1_reads "<<dec<<L1_reads<<endl;
       cout<<"L1_read_misses "<<L1_read_misses<<endl;
       cout<<"L1_writes "<<L1_writes<<endl;
       cout<<"L1_write_misses "<<L1_write_misses<<endl;
       myfile.close();
   }
   else cout<<"unable to open file";
  


    return 0;
}//end of main


