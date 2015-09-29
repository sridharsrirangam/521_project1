#include <iostream>
#include "class_definitions.h"

typedef unsigned int uint;
#define DEBUG 0
int L2_miss_global=0;
/*
unsigned int L1_reads=0;
unsigned int L1_writes=0;
unsigned int L1_read_misses=0;
unsigned int L1_write_misses=0;
unsigned int total_misses=0;
*/


//function for cpu to request L1 or L1 to request L2
//void cache_class::request_block(unsigned int address,char operation,unsigned int &Level_reads,unsigned int  &Level_read_misses,unsigned int  &Level_writes,uint &Level_write_misses, cache_class *next_level)
void cache_class::request_block(unsigned int address,char operation)
{
    unsigned int address_noblock,address_noset,set_id;
   bool hit_or_miss=0;
   int block_id;

    address_noblock=(address>>block_bits);
    address_noset=(address_noblock>>set_bits);
    set_id=(address_noblock%address_noset);
    count++;
    cout<<"-------------- "<<endl;
    cout<<"L "<<cacheLevel<<" cache"<<endl;
    cout<<"# "<<dec<<count<<" : "<<hex<<address<<" ( tag "<<address_noset<<" , index "<<dec<<set_id<<")"<<endl;
    
    hit_or_miss=set_incache[set_id].check_tag(address_noset,operation,address,set_id,set_bits,block_bits);
    if(operation=='r')
    { Level_reads++; if(hit_or_miss==0)Level_read_misses++;}
    else if(operation=='w')
    {Level_writes++;if(hit_or_miss==0)Level_write_misses++;}
    if(hit_or_miss==0)
    {   if(next_level!=NULL)
        next_level->request_block(address,'r');

       // if(operation=='r') Level_read_misses=Level_read_misses+1;
       // else if(operation=='w')Level_write_misses++;
    }
    

}


//void set::check_tag(unsigned int tag_address,uint addressNextLevel,cache_class *next_level,char operation,unsigned int &Level_read_misses, unsigned int &Level_write_misses)
int set::check_tag(unsigned int tag_address,char operation,unsigned int address_org,int calling_set,int set_bits,int block_bits)
{
    unsigned int tag_requested=tag_address;
    int temp_block_id;
    int hit_in_set=0;
    
  //  cout<<"tag address "<<hex<<tag_requested<<endl;
    for(int i=0;i<ASSOC;i++)
    {
        if((tag_requested==block_inset[i].tag)){//&&(block_inset[i].valid_bit==1)){
            temp_block_id=i;
            hit_in_set=1;
        }//end of if
    }//end of for
    if(hit_in_set==0)
    {
        //code for miss handling
       // next_level.request_block(tag_requested);
       // LRU_increment(); //maybe not required here if called in allocate funtion
      // total_misses++;
      // if(operation=='r')
           // Level_read_misses++;
       // else if(operation=='w')
           // Level_write_misses++;
        allocate_and_assign(tag_requested,operation,address_org,calling_set,set_bits,block_bits);
       /* if(*next_level!=NULL)
        {
        next_level->request_block(addressNextLevel,operation,NULL);
        }*/
        if(cacheLevel==2){if(operation=='r')L2_miss_global++;}
#ifdef DEBUG
        cout<<hex<<"MISS    ";
        if(operation=='w') cout<<"write"<<endl;
        cout<<"L2_global is "<<dec<<L2_miss_global<<endl;
#endif

    }
    else if(hit_in_set==1)
    {
        //hit handling
      #ifdef DEBUG
        cout<<"HIT  "<<operation<<endl;
#endif
       
        LRU_increment(temp_block_id);
        if(operation=='w')
        {
           block_inset[temp_block_id].dirty_bit=1;
         }//end of if

    }
return hit_in_set;
}


void set::LRU_increment(int block_id){
    for(int i=0;i<ASSOC;i++)
    {
        if(LRU_bits[i]<LRU_bits[block_id]) LRU_bits[i]++;
    }
    LRU_bits[block_id]=0;
}

void set::allocate_and_assign(unsigned int tag_requested,char operation,unsigned int address_org,int calling_set,int set_bits,int block_bits)
{ 
    int max=0;
    int index_id;
    for(int i=0;i<ASSOC;i++)
    {
        if(LRU_bits[i]>max){
            max=LRU_bits[i];
            index_id=i;
        }//end of if
    }//end of for
        //write function to check dirty bit and identify to call L2 request or not here
       if(block_inset[index_id].dirty_bit==1)
       {
           if(next_level!=NULL)
           {
            int address_evict = ((block_inset[index_id].tag<<set_bits)+calling_set)<<block_bits;
            next_level->request_block(address_evict,'w');
           }

       }
        block_inset[index_id].tag=tag_requested;
        block_inset[index_id].valid_bit=1;
        block_inset[index_id].dirty_bit=0;
        //written now
        LRU_increment(index_id);
        if(operation=='w')
        {
           block_inset[index_id].dirty_bit=1;
        }//end of if
}

void cache_class::print_cache()
{
    cout<<"======cache contents====="<<endl;
    for(int i=0;i<set_number;i++)
    {
        cout<<"set   "<<dec<<i<<":     ";
        for(int j=0;j<associativity;j++)
        {
            int l;
            l=set_incache[i].minimum();

            cout<<hex<<set_incache[i].block_inset[l].tag<<"  ";
            if(set_incache[i].block_inset[l].dirty_bit)
            {
                cout<<"D ";
            }
            else{
                cout<<"  ";
            }
    }
    cout<<"\n";
    }
    cout<<"L1_reads "<<dec<<Level_reads<<endl;
       cout<<"L1_read_misses "<<Level_read_misses<<endl;
       cout<<"L1_writes "<<Level_writes<<endl;
       cout<<"L1_write_misses "<<Level_write_misses<<endl;

}
int set::minimum()
{
    int min=100;
    int index_id;
    for(int i=0;i<ASSOC;i++)
    {
        if((LRU_bits[i]<min)&&(block_inset[i].valid_bit!=0))
        {
            min=LRU_bits[i];
            index_id=i;
        }
    }
    block_inset[index_id].valid_bit=0;
    return index_id;
}



