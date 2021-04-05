#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include "sgx_trts.h"
#include "sgx_urts.h"

#include "App.h"
#include "Enclave_u.h"

#define MAX_PATH FILENAME_MAX
#define SGXSSL_CTR_BITS	128
#define SHIFT_BYTE	8

#include "user_types.h"

/* Global EID */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
template<class T>
void print_error_message(T ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    
    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    sgx_misc_attribute_t misc_attr;

    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, \
                NULL, NULL, &global_eid, &misc_attr);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }

    // printf("[SGX_ATTR]%lx[ICE]\n", misc_attr.secs_attr.xfrm);

    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

double ocall_gettime(const char *name="\0", int is_end=false) {
    static std::chrono::time_point<std::chrono::high_resolution_clock> \
                            begin_time, end_time;
    static bool begin_done = false;

    if (!is_end) {
        begin_done = true;
        begin_time = std::chrono::high_resolution_clock::now();
    } else if (!begin_done) {
        printf("NOTE: begin time is not set, so elapsed time is not printed!\n");
    } else {
        end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - begin_time;
        if(TIMEPRINT) {
            printf("%s%s Time elapsed: %fms\n\n", KYEL, name, elapsed.count()*1000);
            printf("%s", KNRM);
        }
        return elapsed.count();
    }

    return 0;
}

void generate_data(DATATYPE *arr, int dim) {
    for(int i=0; i<dim;i++) 
        arr[i] = (DATATYPE)rand();
}

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);
    if(argc!=3) {
        printf("Usage: ./app DIM k\n\n");
        return 0;
    }
    double t_enclave_creatation, t_nth;
    const int dim = atoi(argv[1]);
    int k = atoi(argv[2]);
    if(k<=1) 
        k = atof(argv[2])*dim;
    srand(time(0));
    /* Initialize the enclave */
    DATATYPE* data = new DATATYPE[dim];
    DATATYPE* data_ = new DATATYPE[dim];
    DATATYPE* ret = data_+k-1;
    printf("init enclave...\n");
    ocall_gettime();
    if(initialize_enclave() < 0) {
        printf("Failed to load enclave.\n");
        goto destroy_enclave;
    }
    t_enclave_creatation = ocall_gettime("Create enclave", 1);

    printf("%s[INFO] Create Enclave: %fms%s\n",KYEL, \
            t_enclave_creatation*1000, KNRM);
    
    generate_data(data, dim);
    printf("sizeof data: %f MB\n\n", \
            1.0*dim*sizeof(DATATYPE)/(1024*1024));

    ecall_nth_element(global_eid, data, dim, k);

    ecall_min_heap(global_eid, data, dim, k);

    ocall_gettime();
    memcpy(data_, data, dim*sizeof(DATATYPE));
    std::nth_element(data, ret, data+dim, \
            std::greater<int>());
    t_nth = ocall_gettime(" ", 1);
    printf("Time of nth outof enclave: %fms\n", t_nth*1000);
    printf("nth_element: %lld\n\n\n", *ret);
destroy_enclave:
    delete[] data;
    delete[] data_;
    return 0;
}
