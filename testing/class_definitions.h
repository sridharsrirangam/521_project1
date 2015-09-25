#ifndef MY_HEADER_CLASSES
#define MY_HEADER_CLASSES

#include <vector>
#include <math.h>

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
       set(int assoc):block_inset(assoc),LRU_bits(assoc){
               ASSOC=assoc;
               for(int i=0;i<assoc;i++){
               LRU_bits[i]=i; //change to (assoc-i)
           }
#ifdef DEBUG
           cout<<"set class is initialised"<<endl;
         cout<<assoc<<endl;
         for(int i=0;i<assoc;i++) cout<<LRU_bits[i]<<endl;
#endif
        
    }
//void set_init(int a){ASSOC=a;}
    void check_tag(unsigned int tag_address,char operation);
    void LRU_increment(int block_id);
    void allocate_and_assign(unsigned int tag_requested,char operation);
    int minimum();
};

class cache_class{
    public:
       
      unsigned  int set_number;
      unsigned  int associativity;
       vector <set> set_incache;
       int block_bits,set_bits;
//cache_class(int Number_of_sets,int associativity_of_set):set_incache(set_number){}
   // {set_number=Number_of_sets; associativity=associativity_of_set;}
cache_class(int num_sets,int set_assoc,int block_size):set_incache(num_sets,set(set_assoc)){

    block_bits=log2(block_size);
    set_bits=log2(num_sets);
    set_number=num_sets;
    associativity=set_assoc;

#ifdef DEBUG  
    cout<<"cache class is initialised"<<endl;
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
    void request_block(unsigned int address,char operation);
    void print_cache();
};

#endif


