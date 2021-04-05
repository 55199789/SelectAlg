#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

void ecall_nth_element(DATATYPE* data_org, uint32_t dim, uint32_t k);
void ecall_min_heap(DATATYPE* data_org, uint32_t dim, uint32_t k);

sgx_status_t SGX_CDECL ocall_print_string(const char* str);
sgx_status_t SGX_CDECL ocall_gettime(double* retval, const char* name, int is_end);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
