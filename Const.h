#ifndef CONST_H
#define CONST_H

/*******************************CONST***********************************/
#define NUM_PROCESS 5

/**VIRTUAL MEMORY**/
//#define SIZE_V  //Virtual Memory(MB)
#define SIZE_Page 8192  //Page size(B)

/**PHYSICAL MEMORY**/
//#define SIZE_P 64   //Physical Memory(MB)
#define SIZE_PageFrame 8192  //Page Frame size(B)
#define NUM_PageFrame 8192  //The number of page frame is 64M/8K = 2^13 = 8192

/**PAGE TABLE**/
#define SIZE_PTfirst 64  //The size of first level page table is 2^6 = 64
#define SIZE_PTsecond 8192  //The size of second level page table is 2^13 = 8192

/**TLB**/
#define SIZE_TLB 16   //The number of TLB entries

/**RUN**/
#define RANDOMROUND 50   //Total round of random access
#define NUM_ADDR_PR 10   //The number of address generated per round
#define EXECUTIMES 3   //Total frequency of execution

/**FILE**/
#define FILE_ADDR ".\\outputs\\addr_seq_"   //File Title of address sequences
#define FILE_VISIT ".\\outputs\\visit_seq_"    //File Title of access info
#define FILE_PT ".\\outputs\\page_table_"    //File Title of page table info
#define FILE_SUFFIX ".txt"    // File suffix of all files

/**OTHERS**/
#define INVALID -1
#define UNUSED 0
#define USED 1


/********************************ALIAS**************************************/
typedef long long ll;


#endif