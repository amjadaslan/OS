#include <unistd.h>
#include <string.h>

#define MAX_SIZE 100000000

size_t allocated_blocks = 0;
size_t allocated_bytes = 0;

class MallocMetadata{
public:
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
    void* alloc_addr;
    MallocMetadata(size_t size);
    ~MallocMetadata();
};

MallocMetadata::MallocMetadata(size_t size) :
 is_free(false),next(nullptr),prev(nullptr),alloc_addr(nullptr) {}

MallocMetadata::~MallocMetadata() {};


class double_list {
public:
    MallocMetadata head;
    MallocMetadata tail;
    int list_size; 
    double_list();
    ~double_list();
    void insert(MallocMetadata* node);
    void* find_alloc(size_t size);
    class iterator;
    iterator begin() const;
    iterator end();
};

// class iterator
class double_list::iterator {
public:
    MallocMetadata* node;
    iterator(MallocMetadata* node);
    ~iterator();
    iterator(const iterator& iter);
    iterator& operator++();
    iterator& operator--();
    bool operator==(const MallocMetadata* node);
    bool operator!=(const iterator& iter);
};

double_list::iterator::iterator(MallocMetadata* node) {
    this->node = node;
}

double_list::iterator::~iterator() {}

double_list::iterator::iterator(const iterator& iter) : node(iter.node) {}

double_list::iterator& double_list::iterator::operator++() {
    node = node->next;
    return *this;
}

double_list::iterator& double_list::iterator::operator--() {
    node = node->prev;
    return *this;
}

bool double_list::iterator::operator==(const MallocMetadata* node) {
    return this->node == node;
}

bool double_list::iterator::operator!=(const iterator& iter) {
    return (this->node != iter.node);
}

//class double_list
double_list::double_list() : head(0), tail(0) {
    head.next = &tail;
    tail.prev = &head;
    list_size = 0;
   
}

double_list::~double_list() {
    sbrk(-(allocated_bytes + allocated_blocks *sizeof(MallocMetadata)));
}

void double_list::insert(MallocMetadata* node) {
        list_size++;              
        node->is_free = true;     
    if (list_size == 0) {
        head.next = node;
        tail.prev = node;
        node->prev = &head;
        node->next = &tail;
          return;  
    }
    if (list_size != 0) {
        for (double_list::iterator iter = begin(); iter != end(); ++iter) {
            if (iter.node->alloc_addr > node->alloc_addr) {
                MallocMetadata *tmp = iter.node->prev;
                iter.node->prev = node;
                tmp->next = node;
                node->next = iter.node;
                node->prev = tmp;
                return;
            }
        }
                MallocMetadata *tmp = tail.prev;
                tail.prev = node;
                tmp->next = node;
                node->next = &tail;
                node->prev = tmp;
                 return;
        }
    }

void* double_list::find_alloc(size_t size)  {
    for(double_list::iterator iter = begin(); iter != end(); ++iter) {
        if(iter.node->size >= size) {
            iter.node->next->prev = iter.node->prev;
            iter.node->prev->next = iter.node->next;
            iter.node->prev = nullptr;
            iter.node->next = nullptr;
            list_size--;
            iter.node->is_free = false;
            return iter.node->alloc_addr;
        }
    }
    return nullptr;
}



double_list::iterator double_list::begin() const {
    return iterator(head.next);
}

double_list::iterator double_list::end() {
    return iterator(&tail);
}

///////////// functions implementation //////////////

double_list free_list;
void* smalloc(size_t size) {
    if(size == 0 || size > MAX_SIZE)
        return nullptr;
    void* ptr = free_list.find_alloc(size);
    if(ptr != nullptr)
        return ptr;
    void* prog_break = sbrk(sizeof(MallocMetadata)+ size);
    if(prog_break == (void*)(-1))
        return nullptr;
    allocated_blocks++;
    allocated_bytes += (size);
    ((MallocMetadata*)prog_break)->size = size;
    ((MallocMetadata*)prog_break)->prev = nullptr;
    ((MallocMetadata*)prog_break)->next = nullptr;
    ((MallocMetadata*)prog_break)->alloc_addr = (char*)prog_break + sizeof (MallocMetadata);
    return ((char*)prog_break + sizeof(MallocMetadata)); 
}

void* scalloc(size_t num, size_t size) {
    if(size == 0 || size*num > MAX_SIZE)
        return nullptr;
    void* alloc = smalloc(size*num);
    if(alloc == nullptr)
        return nullptr;
    memset(alloc, 0, size*num);
    return alloc;
}

void sfree(void* p) {
    if(p == nullptr)
        return;
    MallocMetadata* free_addr= (MallocMetadata*)((char*)p - sizeof (MallocMetadata));
    if(free_addr->is_free)
        return;
   free_list.insert(free_addr);
    return;
}

  void* srealloc(void* oldp, size_t size) {
      if (size == 0 || size > MAX_SIZE)
          return nullptr;

      if (oldp == nullptr)
          return smalloc(size);
      MallocMetadata *addr = (MallocMetadata *) ((char *) oldp - sizeof(MallocMetadata));
      if (addr->size >= size) {
          return oldp;
      }
      void *ptr = smalloc(size);
      if (ptr == nullptr) {
          return nullptr;
      }
      memcpy(ptr, oldp, addr->size);
      sfree(oldp);
      return ptr;
  }

  size_t _num_free_blocks() {
    return free_list.list_size;
}

size_t _num_free_bytes() {
    size_t data = 0;
    for (double_list::iterator iter = free_list.begin(); iter != free_list.end(); ++iter) {
        data = data + iter.node->size;
    }
    return data;
}

size_t _num_allocated_blocks() {
    return allocated_blocks;
}

size_t _num_allocated_bytes() {
    return allocated_bytes;
}

size_t _num_meta_data_bytes() {
    return allocated_blocks * sizeof(MallocMetadata);
}

size_t _size_meta_data() {
    return sizeof (MallocMetadata);
}
