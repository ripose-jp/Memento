#ifndef XJDXGEN_H
#define XJDXGEN_H

#include <stdint.h>

/*====== prototypes=================================================*/
int __indexer_start(int argc, char **argv);
void jqsort(int32_t i, int32_t j);
int Kstrcmp(uint32_t lhs, uint32_t rhs);
int alphaoreuc(unsigned char x);
unsigned char* readDictionary(const char* dictName,uint32_t *filesize);
uint32_t buildIndex(unsigned char* dict, uint32_t dictLength);

#endif