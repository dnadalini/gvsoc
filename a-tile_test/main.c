#include <stdint.h>
# include "tinyprintf.h"
#include "pcm_hwpe_archi.h"

#define VECTOR_SIZE 512
#define N_VECTORS 1
#define N_ELEMENTS VECTOR_SIZE*N_VECTORS
#define STRIDE 64
#define N_TRANSFERS N_ELEMENTS/STRIDE

int main() {

    // Set PCM parameters (all LA, sector 1)
    //*(volatile uint32_t *)(0x41004) = 0x10FFFF01;

    // Write one register and read back the value
    uint32_t volatile target_addr = PCM_HWPE_CFG_DIM;
    uint32_t volatile value = 0xABBACDDC;

    *(uint32_t *)target_addr = value;

    uint32_t volatile read_value = *(uint32_t *)target_addr;

    if(read_value != value){
        printf("Read %x instead of %x\n", read_value, value);
        return -1;
    } else
    {
        printf("Write-read RF test passed!!\n");
    }

    // Fake initialization of Xi buffer
    int8_t Xi[N_ELEMENTS];
    for (uint32_t i = 0; i < 4*N_VECTORS; i++)
    {
        for (uint32_t j = 0; j < 128; j++)
        {
            Xi[i*128+j] = 1;
            //*(uint32_t *)(0x00020000 + i*128 + j) = 1;
        }
        
    }

    // Declare output vector
    volatile int8_t Yi[N_ELEMENTS];
    
    // Streamer configuration
    printf("Xi address: %p\n", Xi);
    printf("Yi address: %p\n", Yi);
    *(uint32_t *)(PCM_HWPE_JOB_SRC_ADDR) = Xi;
    //*(uint32_t *)(PCM_HWPE_JOB_SRC_ADDR) = 0x00020000;
    *(uint32_t *)(PCM_HWPE_D0_LENGTH) = 8;
    *(uint32_t *)(PCM_HWPE_D0_STRIDE) = STRIDE; // N_INP_BURST
    *(uint32_t *)(PCM_HWPE_D1_LENGTH) = N_VECTORS;  // N_VECTORS
    *(uint32_t *)(PCM_HWPE_D1_STRIDE) = VECTOR_SIZE;
    *(uint32_t *)(PCM_HWPE_TOTAL_LENGTH) = N_TRANSFERS; // N_ELEMENTS/64
    *(uint32_t *)(PCM_HWPE_JOB_DST_ADDR) = Yi;
    //*(uint32_t *)(PCM_HWPE_JOB_DST_ADDR) = 0x00020600;
    *(uint32_t *)(PCM_HWPE_OUT_D0_LENGTH) = 8;
    *(uint32_t *)(PCM_HWPE_OUT_D0_STRIDE) = STRIDE;
    *(uint32_t *)(PCM_HWPE_OUT_D1_LENGTH) = N_VECTORS;
    *(uint32_t *)(PCM_HWPE_OUT_D1_STRIDE) = VECTOR_SIZE;
    *(uint32_t *)(PCM_HWPE_OUT_TOTAL_LENGTH) = N_TRANSFERS; // N_ELEMENTS/64

    *(uint32_t *)(PCM_HWPE_TRIG) = 0x000000001;

    // Wait end of computation (IRQ not implemented yet)
    volatile uint32_t status = *(uint32_t *)(PCM_HWPE_STATUS);
    while(status != 0x00000001) {
        for (uint32_t i=0; i<1000; i++){
            ;;
        }

        status = *(uint32_t *)(PCM_HWPE_STATUS);
    }

    printf("HWPE finished computation - checking results...\n");

    uint32_t err = 0;

    for (uint32_t i=0; i<N_ELEMENTS; i++) {
        if(Yi[i] != 127) {
        //if(*(int32_t *)(0x00020600 + i) != 127) {
            err++;
            printf("Yi[%d] = %d!!\n", i, Yi[i]);
        }
    }

    if(err == 0)
        printf("TEST PASSED!!\n");
    else
        printf("TEST FAILED!!\n");

    return err;
}