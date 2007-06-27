#ifndef __GETCHUNK_D
#define __GETCHUNK_D

void *newchunk(FILE*,char*);
char *getchunk(void*,int*);
void freechunk(void*);

#endif/*__GETCHUNK_D*/
