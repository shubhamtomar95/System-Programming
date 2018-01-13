#ifndef HEAP_H
#define HEAP_H

#define malloc(x) modified_malloc(x, __FILE__, __LINE__);
#define free(x) modified_free(x, __FILE__, __LINE__);

void* modified_malloc(int , char*, int);
void modified_free(void*, char *, int);
#endif