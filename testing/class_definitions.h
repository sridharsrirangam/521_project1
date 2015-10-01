#ifndef MY_HEADER_CLASSES
#define MY_HEADER_CLASSES

#include <vector>
#include <math.h>

typedef unsigned int uint;
#define DEBUG 0
using namespace std;
class block_basic{
    public:
   unsigned int tag;
    bool valid_bit, dirty_bit;
     //remove
   // void toggle_valid_bit();
    
  //  void incr_LRU_bit(); //there is a function in set, so this function is not required.

    
    //maybe a function to set block parameters

    block_basic(){

        cout<<"block class is initialted"<<endl;

        tag=0;
        valid_bit=0;
    dirty_bit=0; }

};

// maybe use this function when a new block is loaded
/*block_basic::block_basic(int a){
 
    tag=0;
    valid_bit=0;
    LRU_bit=0;

}*/

//victim begins
class cache_class;

class victim_cache {
    public:
    vector<block_basic> block_invictim;
   vector<int> LRU_bits_victim;
   vector<unsigned int> address;
   int NUM_BLOCK;
   int block_bits;
   cache_class *next_level;
    victim_cache(int num_blocks,cache_class *NextLevel,int blockBits):block_invictim(num_blocks),LRU_bits_victim(num_blocks),address(num_blocks){
        NUM_BLOCK=num_blocks;
        block_bits=blockBits;
        next_level=NextLevel;
        for(int i=0;i<NUM_BLOCK;i++)
        {
            LRU_bits_victim[i]=i;
        }
#ifdef DEBUG
        cout<<"victim cache is initiated"<<endl;
        cout<<"LRU bits of victim cache ";
        for(int i=0;i<NUM_BLOCK;i++){
            cout<<LRU_bits_victim[i]<<endl;
#endif      
        }//end of for

    }//end of constructor
    
    void victim_LRU_increment(int block_id);
    int check_tag_victim(unsigned int full_address,block_basic **hit_block,int *block_invictim_id );
    void swap_block(block_basic *swap_input,block_basic *swap_output);
    void allocate_in_victim(unsigned int tag_requested,unsigned int address_org,bool valid_bit,bool dirty_bit);
    int minimum();
};




class set{
    public:
       // int ASSOC;
        //set(int Level_assoc){ASSOC=Level_assoc;}
        //initialisation of blocks in set
      // block_basic *block_inset;
      // block_inset= new block_basic[ASSOC];
       vector<block_basic> block_inset;
     //ASSOC was removed form constructor brackets
      vector<int> LRU_bits;
      int ASSOC;
      int cacheLevel;
      cache_class *next_level;
      int num_victim;
      victim_cache *victim_Cache;
       set(int assoc,int cache_level,cache_class *nextLevel,int num_blocks_victim,victim_cache *victimCache):block_inset(assoc),LRU_bits(assoc){
               ASSOC=assoc;
               cacheLevel=cache_level;
               next_level=nextLevel;
               num_victim=num_blocks_victim;
               for(int i=0;i<assoc;i++){
               LRU_bits[i]=i; //change to (assoc-i)
               victim_Cache=victimCache;
           }
#ifdef DEBUG
           cout<<"set class of level "<<cacheLevel<<" is  initialised"<<endl;
         cout<<assoc<<endl;
         for(int i=0;i<assoc;i++) cout<<LRU_bits[i]<<endl;
#endif
        
    }
//void set_init(int a){ASSOC=a;}
    //void check_tag(unsigned int tag_address,unsigned int addressNextLevel,cache_class *next_level,char operation,unsigned int &L1_read_misses,unsigned int &L1_write_misses);
    int check_tag(unsigned int tag_address,char operation,unsigned int address_org,int calling_set,int set_bits,int block_bits);
    void LRU_increment(int block_id);
    void allocate_and_assign(unsigned int tag_requested,char operation,unsigned int address_org,int calling_set,int set_bits,int block_bits);
    int minimum();
};

class cache_class{
    public:
      int count;
      int cacheLevel;
      unsigned  int set_number;
      unsigned  int associativity;
       vector <set> set_incache;
       int block_bits,set_bits;
       uint Level_reads,Level_writes,Level_read_misses,Level_write_misses;
        cache_class *next_level;
        victim_cache victimCache;
       victim_cache *victimPntr;
        int num_victim;
//cache_class(int Number_of_sets,int associativity_of_set):set_incache(set_number){}
   // {set_number=Number_of_sets; associativity=associativity_of_set;}
cache_class(int num_sets,int set_assoc,int block_size,cache_class *NextLevel,int cache_level,int num_blocks_victim):victimCache(num_blocks_victim,NextLevel,log2(block_size)),set_incache(num_sets,set(set_assoc,cache_level,NextLevel,num_blocks_victim,&victimCache)){

    block_bits=log2(block_size);
    set_bits=log2(num_sets);
    set_number=num_sets;
    associativity=set_assoc;
    next_level=NextLevel;
    count=0;
    cacheLevel=cache_level;
    num_victim=num_blocks_victim;
    victimPntr=&victimCache;
   
#ifdef DEBUG  
    cout<<"cache class is initialised"<<endl;
    cout<<count<<endl;
    cout<<num_sets<<endl;
#endif
}

/*
void cache_init(int number_of_set,int associativity_of_set){
    set_number=number_of_set;
    associativity=associativity_of_set;

int i;
for(i=0;i<set_number;i++)
{
    set_incache[i].set_init(associativity);
}
}//end of cache init

// set *cache;
       // cache=new set[set_number]
      
      //set *set_incache;
     // set_incache= new set[set_number];
     */
   // void request_block(unsigned int address,char operation,unsigned int &L1_reads,unsigned int &L1_read_misses,unsigned int &L1_writes,unsigned int &L1_write_misses,cache_class *next_level);
    void request_block(unsigned int address,char operation);
    
    void print_cache();
};

#endif


