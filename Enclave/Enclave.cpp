#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>
#include <algorithm>
#include <functional>
#include "sgx_tcrypto.h"
#include "minHeap.h"
#include "user_types.h"
extern "C"
int puts(const char *str)
{
    printf(str);
    printf("\n");
    return 0;
}

extern "C"
int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

void ecall_nth_element(DATATYPE *data_org, uint32_t dim, uint32_t k) {
    double t_nth;
    DATATYPE *data_tmp = new DATATYPE[dim];
    DATATYPE *data = new DATATYPE[dim];
    // Move into the enclave
    memcpy(data_tmp, data_org, dim*sizeof(DATATYPE));
    // One more copy as std::nth_element will change the location
    ocall_gettime(&t_nth, "nth_element", 0);
    memcpy(data, data_tmp, dim*sizeof(DATATYPE));
    DATATYPE *ret = data + k-1;
    std::nth_element(data, ret, data+dim, \
                std::greater<int>());
    ocall_gettime(&t_nth, "nth_element", 1);
    
    printf("Time of nth_element: %fms\n", t_nth*1000);
    printf("nth_element: %lld\n\n", *ret);
    delete[] data;
    delete[] data_tmp;
}

void ecall_min_heap(DATATYPE *data_org, uint32_t dim, uint32_t k) {
    double t_nth, t_nth_tot = 0;
    // Approximately 93 MB, and we also need to reserve 
    // some space for temporary variables.
    int loadedDim = 93*1000*1000 / sizeof(DATATYPE)
                - k;
    if(loadedDim>dim-k) 
        loadedDim = dim-k;
    else if(loadedDim<0) 
        loadedDim = 45 * 1000 * 1000 \
                    / sizeof(DATATYPE);
    printf("Loaded Dim: %d\n", loadedDim);
    DATATYPE *data = new DATATYPE[loadedDim];
    DATATYPE *heap = new DATATYPE[k];
    memcpy(data, data_org, k*sizeof(DATATYPE));
    ocall_gettime(&t_nth, "min_heap", 0);
    init(heap, data, k);
    ocall_gettime(&t_nth, "min_heap", 1);
    t_nth_tot += t_nth;
    for(int i=k;i<dim;i+=loadedDim) {
        const int lDim = dim-i<loadedDim?\
                dim-i:loadedDim;
        // Move into the enclave
        memcpy(data, data_org+i, lDim*sizeof(DATATYPE));
        ocall_gettime(&t_nth, "min_heap", 0);
        for(int j=0;j<lDim;j++)
            insert(heap, k, data[j]);
        ocall_gettime(&t_nth, "min_heap", 1);
        t_nth_tot += t_nth;
    }

    printf("Time of min_heap: %fms\n", t_nth_tot*1000);
    printf("nth_element: %lld\n\n", heap[0]);
    delete[] data;
    delete[] heap;
}