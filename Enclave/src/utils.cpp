#include "Enclave.h"
#include "user_types.h"
#include "sgx_tcrypto.h"
#include "utils.h"
const sgx_aes_ctr_128bit_key_t KEY_Z = \
                    {0x0F, 0x0E, 0x0D, 0x0C, \
                     0x0B, 0x0A, 0x09, 0x08, \
                     0x07, 0x06, 0x05, 0x04, \
                     0x03, 0x02, 0x01, 0x00};
