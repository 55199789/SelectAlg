#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_ecall_nth_element_t {
	DATATYPE* ms_data_org;
	uint32_t ms_dim;
	uint32_t ms_k;
} ms_ecall_nth_element_t;

typedef struct ms_ecall_min_heap_t {
	DATATYPE* ms_data_org;
	uint32_t ms_dim;
	uint32_t ms_k;
} ms_ecall_min_heap_t;

typedef struct ms_ocall_print_string_t {
	const char* ms_str;
} ms_ocall_print_string_t;

typedef struct ms_ocall_gettime_t {
	double ms_retval;
	const char* ms_name;
	int ms_is_end;
} ms_ocall_gettime_t;

static sgx_status_t SGX_CDECL Enclave_ocall_print_string(void* pms)
{
	ms_ocall_print_string_t* ms = SGX_CAST(ms_ocall_print_string_t*, pms);
	ocall_print_string(ms->ms_str);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_gettime(void* pms)
{
	ms_ocall_gettime_t* ms = SGX_CAST(ms_ocall_gettime_t*, pms);
	ms->ms_retval = ocall_gettime(ms->ms_name, ms->ms_is_end);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[2];
} ocall_table_Enclave = {
	2,
	{
		(void*)Enclave_ocall_print_string,
		(void*)Enclave_ocall_gettime,
	}
};
sgx_status_t ecall_nth_element(sgx_enclave_id_t eid, DATATYPE* data_org, uint32_t dim, uint32_t k)
{
	sgx_status_t status;
	ms_ecall_nth_element_t ms;
	ms.ms_data_org = data_org;
	ms.ms_dim = dim;
	ms.ms_k = k;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t ecall_min_heap(sgx_enclave_id_t eid, DATATYPE* data_org, uint32_t dim, uint32_t k)
{
	sgx_status_t status;
	ms_ecall_min_heap_t ms;
	ms.ms_data_org = data_org;
	ms.ms_dim = dim;
	ms.ms_k = k;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

