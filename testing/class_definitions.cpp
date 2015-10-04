#include <iostream>
#include "class_definitions.h"

typedef unsigned int uint;
#define DEBUG 0
int L2_miss_global=0;
int swap_requests=0;
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
   int hit_or_miss=0;
   int block_id;

    address_noblock=(address>>block_bits);
    address_noset=(address_noblock>>set_bits);
    set_id=(address_noblock%address_noset);
    count++;
#if DEBUG
    cout<<"-------------- "<<endl;
    cout<<"L "<<cacheLevel<<" cache"<<endl;
    cout<<"# "<<dec<<count<<" : "<<hex<<address<<" ( tag "<<address_noset<<" , index "<<dec<<set_id<<")"<<endl;
#endif    
    hit_or_miss=set_incache[set_id].check_tag(address_noset,operation,address,set_id,set_bits,block_bits);
    if(operation=='r')
    { Level_reads++; if((hit_or_miss==0)||(hit_or_miss==2))Level_read_misses++;}
    else if(operation=='w')
    {Level_writes++;if((hit_or_miss==0)||(hit_or_miss==2))Level_write_misses++;}
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
    block_basic *victim_hit_block;
    int hit_in_victim=0;
   int block_invictim_id;
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
        if(num_victim!=0){
           hit_in_victim=victim_Cache->check_tag_victim(address_org, &victim_hit_block,&block_invictim_id);
           if(hit_in_victim==1){
               int max=0;
               int index_id;
               for(int i=0;i<ASSOC;i++)
               {
                   if(LRU_bits[i]>max)
                   {
                       max=LRU_bits[i];
                       index_id=i;
                   }
               }

           int address_evict = ((block_inset[index_id].tag<<set_bits)+calling_set)<<block_bits;
          // cout<<hex<<address_org<<"hit in victim"<<endl;
         // cout<<"address swapped out"<<victim_Cache->address[block_invictim_id]; 
          victim_Cache->address[block_invictim_id]=address_evict;
         // cout<<"address swapped in"<<victim_Cache->address[block_invictim_id]<<endl;
           victim_Cache->swap_block(&block_inset[index_id], victim_hit_block);
           //last addred code to correct dirty bit issue
           if(operation=='w')
           {block_inset[index_id].dirty_bit=1;
           }
           
           LRU_increment(index_id);
           }
#if DEBUG
#endif


       }
       if((num_victim==0)||(hit_in_victim==0)){
        allocate_and_assign(tag_requested,operation,address_org,calling_set,set_bits,block_bits);}
       /* if(*next_level!=NULL)
        {
        next_level->request_block(addressNextLevel,operation,NULL);
        }*/
       // if(cacheLevel==2){if(operation=='r')L2_miss_global++;}
#if DEBUG
        cout<<hex<<"MISS    ";
        if(operation=='w') cout<<"write"<<endl;
        cout<<"L2_global is "<<dec<<L2_miss_global<<endl;
#endif

    }
    else if(hit_in_set==1)
    {
        //hit handling
      #if DEBUG
        cout<<"HIT  "<<operation<<endl;
#endif
       
        LRU_increment(temp_block_id);
        if(operation=='w')
        {
           block_inset[temp_block_id].dirty_bit=1;
         }//end of if

    }
    int hit_return;
    if((hit_in_set==0)&&(hit_in_victim==0)) hit_return=0;
    else if(hit_in_set==0) hit_return=2;
    else hit_return=1;
return hit_return;
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
     
  int address_evict = ((block_inset[index_id].tag<<set_bits)+calling_set)<<block_bits;
    if(num_victim==0){
          if(block_inset[index_id].dirty_bit==1)
       {
           if(next_level!=NULL) //victim check woll come here
           {
            next_level->request_block(address_evict,'w');
           }}

       }
      if((num_victim!=0)&&block_inset[index_id].valid_bit)
      {victim_Cache->allocate_in_victim(block_inset[index_id].tag,address_evict,block_inset[index_id].valid_bit,block_inset[index_id].dirty_bit);
     
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
    if(num_victim!=0)
    {
        for(int i=0;i<num_victim;i++)
        {
            int l;
            l=victimCache.minimum();
            cout<<hex<<(victimCache.address[l]>>block_bits)<<" ";
            if(victimCache.block_invictim[l].dirty_bit) cout<<"D "; else cout<<" ";
        }
        cout<<"\n";
    }
    cout<<"L1_reads "<<dec<<Level_reads<<endl;
       cout<<"L1_read_misses "<<Level_read_misses<<endl;
       cout<<"L1_writes "<<Level_writes<<endl;
       cout<<"L1_write_misses "<<Level_write_misses<<endl;
       cout<<"swap requests"<<swap_requests<<endl;

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

int victim_cache::minimum()
{
    int min=1000;
    int index_id;
    for(int i=0;i<NUM_BLOCK;i++)
    {
        if((LRU_bits_victim[i]<min)&&(block_invictim[i].valid_bit!=0))
        {
            min=LRU_bits_victim[i];
            index_id=i;
        }
    }
    block_invictim[index_id].valid_bit=0;
    return index_id;
}

int victim_cache::check_tag_victim(unsigned int full_address,block_basic **hit_block,int *block_invictim_id)
{
    unsigned int addr_requested = full_address;
    int temp_block_id;
    int hit_in_set=0;
    for(int i=0;i<NUM_BLOCK;i++)
    {
        if((addr_requested>>block_bits)==(address[i]>>block_bits)){
            temp_block_id=i;
            hit_in_set=1;
        } //end of if
    }//end of for
    if(hit_in_set) victim_LRU_increment(temp_block_id);
    int hit_or_miss=hit_in_set;
    *block_invictim_id=temp_block_id;
    *hit_block=&block_invictim[temp_block_id];
    return hit_or_miss;
    //depending on hit or miss from victim, L1 should call L2
}
void victim_cache::victim_LRU_increment(int block_id){
        for(int i=0;i<NUM_BLOCK;i++)
                {
                 if(LRU_bits_victim[i]<LRU_bits_victim[block_id]) LRU_bits_victim[i]++;
                                }
            LRU_bits_victim[block_id]=0;
}

void victim_cache::swap_block(block_basic *swap_input, block_basic *swap_output){
    block_basic temp;
 swap_requests++; 
    temp.tag= swap_input->tag;
    temp.valid_bit=swap_input->valid_bit;
    temp.dirty_bit=swap_input->dirty_bit;

    swap_input->tag=swap_output->tag;
    swap_input->valid_bit=swap_output->valid_bit;
    swap_input->dirty_bit=swap_output->dirty_bit;
    
    swap_output->tag=temp.tag;
    swap_output->valid_bit=temp.valid_bit;
    swap_output->dirty_bit=temp.dirty_bit;

//    cout<<"swap function called "<<"input "<<swap_input->tag<<" output tag "<<swap_output->tag<<endl;
}

//TODO sent L2 pointer to victim
void victim_cache::allocate_in_victim(unsigned int tag_requested,unsigned int address_org,bool valid_bit,bool dirty_bit)
{
    int max=0;
    int index_id=0;
    for(int i=0;i<NUM_BLOCK;i++)
    {
        if(LRU_bits_victim[i]>max){
            max=LRU_bits_victim[i];
            index_id=i;
        
       }
    }
    if(block_invictim[index_id].dirty_bit==1)
    {
        if(next_level!=NULL)
        {
            int address_evict = address[index_id];
            next_level->request_block(address_evict,'w');
        }
    
    }
#if DEBUG
    cout<<"victim allocate function called"<<endl;
#endif
    block_invictim[index_id].tag=tag_requested;
    block_invictim[index_id].valid_bit=valid_bit;
    block_invictim[index_id].dirty_bit=dirty_bit;
    address[index_id]=address_org;
    victim_LRU_increment(index_id);
#if DEBUG
    cout<<" Tag "<<hex<<tag_requested<<" address "<<address_org<<" v "<<valid_bit<<" d "<<dirty_bit<<endl;
#endif
}





//void victim_print(){

