#include "dynamic_memory.h"
#include <stdio.h>
#include <unistd.h>

#define HEAP_SIZE 4096
#define ENTRY_SIZE sizeof ( node )
#define allign_by_4(x) ((((x-1)>>2)<<2)+4)

typedef enum {false =0, true} bool;

typedef enum 
{
    HEAP_MALLOC_INSUFFICIENT,
    HEAP_FREE_REDUNDANT,
    HEAP_FREE_NOT_ALLOCATED,
    HEAP_FREE_MIDDLE
}heap_error;

typedef struct memory_entry
{
    struct memory_entry *next;
    
    char *last_file;

    int size;

    int free_call_line_no;

    bool free;


    struct memory_entry *prev;    
}node;

void print_error(heap_error error, char *file, int line);
void split_block(node* temp, int size);
node* extend_heap(node* last, int size);

node* base = NULL;
void* modified_malloc(int size, char* calling_file, int calling_line)
{
    int alligned_size = allign_by_4(size);
    node *temp, *last;
    if(!base)
    {
        printf("Malloc Called!\n");
        void* ptr = sbrk(alligned_size + ENTRY_SIZE);
        temp = (node*) ptr;
        temp->next = NULL;
        temp->size = ENTRY_SIZE + alligned_size;
        temp->prev = NULL;
        //ptr = (void*) ((char*)ptr + ENTRY_SIZE);
    }

    while(temp != NULL)
    {
        last = temp;
        if(temp->free)
        {
            if (2*ENTRY_SIZE + alligned_size + 4 <= temp->size)
            {
                split_block(temp, alligned_size);
                temp->free = false;
                temp = (void *)((char *) temp + ENTRY_SIZE);
                return temp;
            }
            if(ENTRY_SIZE + alligned_size <= temp->size)
            {
                temp->free = false;
                temp = (void *)((char *) temp + ENTRY_SIZE);
                return temp;
            }
        }
    
        
        temp = temp->next;

    }

    temp = extend_heap(last, alligned_size);
    if (temp != NULL)
    {
       return temp;
    }
    print_error(HEAP_MALLOC_INSUFFICIENT, calling_file, calling_line);
    return NULL;

}

void split_block(node* temp, int size)
{
    node *ptr = (void *)((char *)temp + (int)size + ENTRY_SIZE);
    ptr->next = temp->next;
    temp->next = ptr;
    ptr->prev = temp;
    ptr->size = temp->size - 2*ENTRY_SIZE - size;
}

node* extend_heap(node* last, int size)
{
    node* ptr = sbrk(size + ENTRY_SIZE);
    
    last->next = ptr;

    ptr->size = ENTRY_SIZE;
    ptr->prev = last;
    ptr->next = NULL;
    ptr->free = 0;

    return ptr;
}

void modified_free(void *address,char *calling_file, int calling_line)
{
    node *temp = base;
    
    while ( temp != NULL )
    {   
        node *ptr = (node *)address;
        void *allocated = (void *)((char *) temp + ENTRY_SIZE);
        if(allocated == ptr)
        {
            
            if(ptr->free == false)
            {
                print_error(HEAP_FREE_REDUNDANT, NULL, 0);
                return ;

            }
            else
            {
                ptr->free = true;
                if(ptr->prev != NULL && (ptr->prev)->free)
                {
                    node *new = ptr->prev;
                    new->next = ptr->next;
                    new->size = new->size + ENTRY_SIZE + ptr->size;
                    return ;

                }

            }
        }else if (false)//((char *)allocated < (char *)ptr && (char *) ptr < (char *) temp->next)
        {
            printf("p = %p value = %d\n", p, *p);
            return ;
        }
        temp = temp->next;
    }
    return ;
}

//TO DO Implement the print_error function
void print_error(heap_error error, char *file, int line)
{
    return ;
}
