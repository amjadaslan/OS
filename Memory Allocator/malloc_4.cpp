#include <iostream>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>





#define MAX_SIZE 100000000
#define MMAP_SIZE 128*1024
#define KB 1024



struct MallocMetadata {
    size_t size;
    bool is_free;
    MallocMetadata* next_in_list;
    MallocMetadata* prev_in_list;
    MallocMetadata* next_in_hist;
    MallocMetadata* prev_in_hist;
};

class double_list{
public:
    MallocMetadata* head;
    MallocMetadata* tail;
    int list_size;
    double_list():head(nullptr),tail(nullptr),list_size(0){}
    ~double_list(){};

};

size_t alignSize(size_t size) {
    if(size % 8 == 0)
        return size;
    return (size+(8-(size%8)));
}

double_list main_list;
double_list mmap_list;
double_list free_blocks_hist[128];

size_t _num_free_blocks(){
    size_t counter=0;
    MallocMetadata * ptr=main_list.head;
    while(ptr!= nullptr){
        if(ptr->is_free){
            counter++;
        }
        ptr=ptr->next_in_list;
    }
    return counter;
}

size_t _num_free_bytes(){
    size_t counter=0;
    MallocMetadata * ptr=main_list.head;
    while(ptr!= nullptr){
        if(ptr->is_free){
            counter +=ptr->size;
        }
        ptr=ptr->next_in_list;
    }
    return counter;
}

size_t _num_allocated_blocks(){
    size_t counter=0;
    MallocMetadata * ptr=main_list.head;
    while(ptr!= nullptr){
        counter++;
        ptr=ptr->next_in_list;
    }
    ptr=mmap_list.head;
    while(ptr!= nullptr){
        counter++;
        ptr=ptr->next_in_list;
    }
    return counter;
}

size_t _num_allocated_bytes(){
    size_t counter=0;
    MallocMetadata * ptr=main_list.head;
    while(ptr!= nullptr){
        counter += ptr->size;
        ptr=ptr->next_in_list;
    }
    ptr=mmap_list.head;
    while(ptr!= nullptr){
        counter+=ptr->size;
        ptr=ptr->next_in_list;
    }
    return counter;
}

size_t _size_meta_data(){
    return sizeof(MallocMetadata);
}

size_t _num_meta_data_bytes(){
    return (_num_allocated_blocks()*_size_meta_data());
}

void remove_from_hist(int index, MallocMetadata* node){
    MallocMetadata* ptr=free_blocks_hist[index].head;
    node->is_free= false;

    if(ptr==node && free_blocks_hist[index].list_size==1){ //there is only one node in the list
        free_blocks_hist[index].head= nullptr;
        free_blocks_hist[index].tail= nullptr;
        free_blocks_hist[index].list_size--;
        return;
    }
    if(ptr==node && free_blocks_hist[index].list_size>1){ //node is head
        free_blocks_hist[index].head= free_blocks_hist[index].head->next_in_hist;
        free_blocks_hist[index].head->prev_in_hist= nullptr;
        node->prev_in_hist= nullptr;
        node->next_in_hist=nullptr;
        free_blocks_hist[index].list_size--;
        return;
    }
    
    while(ptr!= nullptr){
        if(ptr==node){
            if(ptr->next_in_hist== nullptr){ //ptr is tail;
                free_blocks_hist[index].tail=ptr->prev_in_hist;
                free_blocks_hist[index].tail->next_in_hist= nullptr;
                node->prev_in_hist= nullptr;
                node->next_in_hist=nullptr;

                free_blocks_hist[index].list_size--;
                return;
            }
            ptr->next_in_hist->prev_in_hist=ptr->prev_in_hist;
            ptr->prev_in_hist->next_in_hist=ptr->prev_in_hist;
            ptr->prev_in_hist= nullptr;
            ptr->next_in_hist= nullptr;
            free_blocks_hist[index].list_size--;
            return;
        }
        ptr=ptr->next_in_hist;
    }
}

void insert_to_hist(int index,MallocMetadata* node){
    node->is_free=true;
    if(free_blocks_hist[index].list_size==0){
        free_blocks_hist[index].list_size++;
        node->prev_in_hist = nullptr;
        free_blocks_hist[index].head = node;
        free_blocks_hist[index].tail= node;
        return;
    }
    MallocMetadata* last= free_blocks_hist[index].tail;
    last->next_in_hist=node;
    node->prev_in_hist=last;
    free_blocks_hist[index].tail=node;
    free_blocks_hist[index].list_size++;
}

void* find_alloc(size_t size) {
    int index = size / KB;
    MallocMetadata *node_to_find = nullptr;
    bool flag = false;
    int num_index;
    for (int i = index; i < 128; i++) {
        if (free_blocks_hist[i].list_size != 0) {
            MallocMetadata *ptr = free_blocks_hist[i].head;
            while (ptr != nullptr) {
                if (ptr->size >= size && ptr->is_free==true) {
                    node_to_find = ptr;
                    num_index = i;
                    flag = true;
                }
                if (flag) {
                    break;
                }
                ptr=ptr->next_in_hist;
            }
            break;
        }
    }
    if (node_to_find == nullptr) return nullptr;
    else {
        remove_from_hist(num_index, node_to_find);
    }
    node_to_find->is_free=false;
    return node_to_find;
}

MallocMetadata* merge(MallocMetadata* node) {
    MallocMetadata *new_node = node;
    int index= node->size/KB;
    if (node->next_in_list) {
        if (node->next_in_list->is_free) {
            int next_index= node->next_in_list->size/KB;
            remove_from_hist(index, node);
            remove_from_hist(next_index,node->next_in_list);
            main_list.list_size--;
            node->size += node->next_in_list->size + _size_meta_data();
            node->next_in_list = node->next_in_list->next_in_list;
            if (node->next_in_list) {
                node->next_in_list->prev_in_list = node;
            }
            if (main_list.tail == node->next_in_list) {
                main_list.tail = node;
                node->next_in_list= nullptr;
            }
            index = node->size / KB;
            insert_to_hist(index, node);


        }
    }

    if (node->prev_in_list) {
        if (node->prev_in_list->is_free) {
            int prev_index= node->prev_in_list->size/KB;
            remove_from_hist(index, node);
            remove_from_hist(prev_index,node->prev_in_list);
            main_list.list_size--;
            node->prev_in_list->size+=node->size+_size_meta_data();
            node->prev_in_list->next_in_list= node->next_in_list;
            if(node->prev_in_list->next_in_list){
                node->prev_in_list->next_in_list->prev_in_list = node->prev_in_list;
            }
            if(main_list.tail == node){
                main_list.tail = node->prev_in_list;
            }
            new_node = node->prev_in_list;
            index= new_node->size/KB;
            insert_to_hist(index,new_node);
        }
    }
    return new_node;
}

void split(MallocMetadata* node, size_t size){
    int notUsed = node->size-(size + _size_meta_data());
    if(node->size -size >=128 + _size_meta_data()){
        node->size=size;
        MallocMetadata* new_node=(MallocMetadata*)((char*)node + _size_meta_data() + size);
        new_node->size =  notUsed;
        new_node->is_free=true;
        new_node->next_in_list=node->next_in_list;
        if(node->next_in_list){
            node->next_in_list->prev_in_list=new_node;
            if(node->next_in_list->is_free)
				merge(node->next_in_list);
        }
        node->next_in_list=new_node;
        new_node->prev_in_list=node;
        if(main_list.tail==node){
            main_list.tail=new_node;
        }
        main_list.list_size++;
        int new_index=new_node->size/KB;
        insert_to_hist(new_index,new_node);
    }
}


void* enlarge_wilderness(size_t size){
    size_t s = size - main_list.tail->size ;
    void* ptr=sbrk(s);
    if(ptr == (void *)-1){
        return nullptr;
    }
    int index= main_list.tail->size/KB;
    remove_from_hist(index, main_list.tail);
    main_list.tail->size = size;
    main_list.tail->is_free = false;


    return main_list.tail;
}

void remove_from_mmap_list(MallocMetadata* free_addr) {

    if(mmap_list.head==mmap_list.tail){
        mmap_list.head= nullptr;
        mmap_list.tail= nullptr;
        mmap_list.list_size--;
        return;
    }
    else if(free_addr==mmap_list.head){
        mmap_list.head=free_addr->next_in_list;
        mmap_list.head->prev_in_list= nullptr;
        mmap_list.list_size--;
        return;
    }
    else if(free_addr==mmap_list.tail){
        mmap_list.tail=mmap_list.tail->prev_in_list;
        mmap_list.tail->next_in_list= nullptr;
        mmap_list.list_size--;
        return;
    }else{
        free_addr->prev_in_list->next_in_list=free_addr->next_in_list;
        free_addr->next_in_list->prev_in_list=free_addr->prev_in_list;
        mmap_list.list_size--;
    }
}



void* merge_left(MallocMetadata* left_node,MallocMetadata* node, size_t size, bool flag) {
    int left_index = left_node->size / KB;
    int node_index = node->size / KB;
    remove_from_hist(left_index, left_node);
    remove_from_hist(node_index, node);
    left_node->size += node->size + _size_meta_data();
    left_node->next_in_list = node->next_in_list;
    if (left_node->next_in_list) {
        left_node->next_in_list->prev_in_list = left_node;
    }
    if (main_list.tail == node) {
        main_list.tail = left_node;
        main_list.tail->next_in_list= nullptr;
    }
    main_list.list_size--;
    if (!flag) {
        left_node->is_free = false;
        split(left_node, size);
        memmove((char *) left_node + _size_meta_data(), (char *) node + _size_meta_data(),node->size);        
       
    }
    return (char *) left_node + _size_meta_data();
}

void* merge_right(MallocMetadata* node, MallocMetadata* right_node, size_t size, bool flag){
    int node_index= node->size/KB;
    int right_index=right_node->size/KB;
    remove_from_hist(node_index,node);
    remove_from_hist(right_index,right_node);
    node->size+=right_node->size+_size_meta_data();
    node->next_in_list = right_node->next_in_list;
    if(node->next_in_list){
        node->next_in_list->prev_in_list =node;
        merge(node->next_in_list);
    }
    if(main_list.tail == right_node){
        main_list.tail =node;
    }
    main_list.list_size--;
    if(!flag) {
        node->is_free = false;
        split(node, size);
    }
    return (char *)node + _size_meta_data();
}




//////////////////////Actual Functions////////////////////////////

void* smalloc(size_t size){
    if(size==0 || size>=MAX_SIZE) {
        return nullptr;
    }

	size = alignSize(size);

    if(size<MMAP_SIZE) {

        if (main_list.list_size == 0) {  //node is first in list (first data block allocated)
            void *addr = sbrk(size + _size_meta_data());
            //sbrk failed
            if (addr == (void *) -1) {
                return nullptr;
            }
            MallocMetadata* alloc_addr = (MallocMetadata *) addr;
            alloc_addr->size = size;
            alloc_addr->is_free = false;
            alloc_addr->next_in_list = nullptr;
            alloc_addr->prev_in_list = nullptr;
            main_list.head = alloc_addr;
            main_list.tail = alloc_addr;
            main_list.list_size++;
            return (char *) addr + _size_meta_data();
        }
        else{ // there's more data allocated
            MallocMetadata *node = (MallocMetadata*)find_alloc(size);
            if (node == nullptr) {
                if (main_list.tail->is_free) {
                    if (!enlarge_wilderness(size)) {
                        return nullptr;
                    }
                    return (char *) main_list.tail + _size_meta_data();
                }
                void *node2 = sbrk(size + _size_meta_data());
                if (node2 == (void *) -1) {
                    return nullptr;
                }
                MallocMetadata* new_block=(MallocMetadata*)node2;
                new_block->size=size;
                new_block->is_free= false;
                new_block->next_in_list= nullptr;
                new_block->prev_in_list= main_list.tail;
                main_list.tail->next_in_list=new_block;
                main_list.tail=new_block;
                main_list.list_size++;
                return (char*)node2 + _size_meta_data();
            }

            split(node, size);
            node->is_free= false;
            return (char*)node + _size_meta_data();
        }
    }
    else if(size>=MMAP_SIZE){

        void* mmap_addr = mmap(NULL, size+_size_meta_data(),PROT_READ | PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE,-1, 0);
        //memory mapping faield
        if(mmap_addr == (void*)-1){
            return nullptr;
        }
        MallocMetadata * mmap_node = (MallocMetadata*) mmap_addr;
        mmap_node->is_free = false;
        mmap_node->size = size;
        mmap_node->next_in_list= nullptr;
        mmap_node->prev_in_list = mmap_list.tail;
        mmap_list.tail = mmap_node;
        if(mmap_list.list_size==0){
            mmap_list.head = mmap_node;
            mmap_list.tail = mmap_node;
        }
        else{
            mmap_list.tail->prev_in_list->next_in_list=mmap_node;
        }
        mmap_list.list_size++;
        return (char *)mmap_node + _size_meta_data();
    }

}

void* scalloc(size_t num, size_t size) {
    if (size == 0 || size >= MAX_SIZE) {
        return nullptr;
    }
    

    void *alloc_addr = smalloc(num * size);
    //smalloc failed
    if (alloc_addr ==nullptr) {
        return nullptr;
    }
    //set bits to 0
    return memset(alloc_addr,0,size*num);
}

void sfree(void* p){

    if(p==nullptr){
        return;
    }
    //get block to free
    MallocMetadata*  free_addr= (MallocMetadata *)(((char*)p)-_size_meta_data());
    if(free_addr->size < MMAP_SIZE){
        if(free_addr->is_free)
            return;
        int index = free_addr->size/KB;
        insert_to_hist(index,free_addr);
        free_addr->is_free=true;
        merge(free_addr);
    }

    else{
        remove_from_mmap_list(free_addr);
        munmap(free_addr, free_addr->size + _size_meta_data());
        return;
    }
}

void* srealloc(void* oldp, size_t size) {
    if (size == 0 || size >= MAX_SIZE) 
        return nullptr;
    
    size = alignSize(size);
    
    if (oldp == nullptr)
        return smalloc(size);

    MallocMetadata *oldp_node = (MallocMetadata *) ((char *) oldp - _size_meta_data());
    
    if (oldp_node->size < MMAP_SIZE) {
        bool flag = false;
        //reuse without any merging
        if (oldp_node->size >= size) {
            split(oldp_node, size);
            oldp_node->is_free=false;
			merge(oldp_node->next_in_list);	
            return (char*)oldp_node + _size_meta_data();
        }
        
        //reuse with lower merging
        else if (oldp_node->prev_in_list
            && oldp_node->prev_in_list->is_free
            && (oldp_node->size + oldp_node->prev_in_list->size + _size_meta_data() >= size)) {
                return merge_left(oldp_node->prev_in_list, oldp_node, size, flag);
				
        }

		//reuse with upper merging
        else if (oldp_node->next_in_list
            && oldp_node->next_in_list->is_free
            && (oldp_node->size + oldp_node->next_in_list->size + _size_meta_data() >= size)) {
               return merge_right(oldp_node, oldp_node->next_in_list, size, flag);	
        }
        //reuse with 3 adjacent blocks
        else if (oldp_node->next_in_list && oldp_node->prev_in_list) {
            if (oldp_node->prev_in_list->is_free && oldp_node->next_in_list->is_free &&
                oldp_node->size + oldp_node->next_in_list->size + _size_meta_data() + oldp_node->prev_in_list->size +
                _size_meta_data() >= size) {
                flag = true;
                MallocMetadata* tmp_prev = oldp_node->prev_in_list;
                
                
                merge_left(oldp_node->prev_in_list, oldp_node, size, flag);
                merge_right(oldp_node->prev_in_list, oldp_node->next_in_list, size, flag);
                oldp_node->prev_in_list->is_free = false;
                split(oldp_node->prev_in_list, size);
				memmove((char *) oldp_node->prev_in_list + _size_meta_data(), (char *) oldp_node + _size_meta_data(),
                        oldp_node->size);
                return (char *) tmp_prev + _size_meta_data();
            }
        }
		
		//enlarge wilderness
        if ( oldp_node==main_list.tail && main_list.tail->size<size) {
			if(oldp_node->prev_in_list->is_free)
				merge_left(oldp_node->prev_in_list,oldp_node,oldp_node->size+oldp_node->prev_in_list->size,false);
            enlarge_wilderness(size);
            return (char *) main_list.tail + _size_meta_data();
        }
		
        void *new_block = smalloc(size);
        if (new_block == nullptr) {
            return nullptr;
        }
        memmove(new_block, oldp, oldp_node->size);
        sfree(oldp);
        return new_block;
    }
    else if (oldp_node->size >= MMAP_SIZE) {
        size_t oldp_size = oldp_node->size;
        void *mmap_alloc = smalloc(size);
        MallocMetadata* new_mmap_alloc=(MallocMetadata*)(((char*)mmap_alloc)-_size_meta_data());
        if(size>=oldp_size){
            memmove((char*)new_mmap_alloc+_size_meta_data(),(char*)oldp_node+_size_meta_data(),oldp_size );
        }
        else{
            memmove((char*)new_mmap_alloc+_size_meta_data(),(char*)oldp_node+_size_meta_data(),size);
        }

        sfree(oldp);
        return mmap_alloc;
    }
}
