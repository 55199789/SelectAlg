#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OCALL_PRINT_STRING_DEFINED__
#define OCALL_PRINT_STRING_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_string, (const char* str));
#endif
#ifndef OCALL_GETTIME_DEFINED__
#define OCALL_GETTIME_DEFINED__
double SGX_UBRIDGE(SGX_NOCONVENTION, ocall_gettime, (const char* name, int is_end));
#endif

sgx_status_t ecall_nth_element(sgx_enclave_id_t eid, DATATYPE* data_org, uint32_t dim, uint32_t k);
sgx_status_t ecall_min_heap(sgx_enclave_id_t eid, DATATYPE* data_org, uint32_t dim, uint32_t k);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
