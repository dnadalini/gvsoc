//#include <stdio.h>
#include <stdint.h>
#include "pcm_hwpe_archi.h"

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
    int8_t Xi[512];
    for (uint32_t i = 0; i < 4; i++)
    {
        for (uint32_t j = 0; j < 128; j++)
        {
            //Xi[i*128+j] = (int8_t)j;
            Xi[i*128+j] = 1;
        }
        
    }

    // Declare output vector
    int8_t Yi[512];
    
    /* for (uint32_t i=0; i<512; i++){
        Xi[i] = (int8_t) i;
    } */

    // Streamer configuration
    printf("Xi address: 0x%x\n", Xi);
    *(uint32_t *)(PCM_HWPE_JOB_SRC_ADDR) = Xi;
    *(uint32_t *)(PCM_HWPE_D0_LENGTH) = 8;
    *(uint32_t *)(PCM_HWPE_D0_STRIDE) = 64;
    *(uint32_t *)(PCM_HWPE_TOTAL_LENGTH) = 8;
    *(uint32_t *)(PCM_HWPE_JOB_DST_ADDR) = Yi;
    *(uint32_t *)(PCM_HWPE_OUT_D0_LENGTH) = 8;
    *(uint32_t *)(PCM_HWPE_OUT_D0_STRIDE) = 64;
    *(uint32_t *)(PCM_HWPE_OUT_TOTAL_LENGTH) = 8;

    *(uint32_t *)(PCM_HWPE_TRIG) = 0x000000001;

    uint32_t err = 0;

    for (uint32_t i=0; i<512; i++) {
        if(Yi[i] != 127) {
            err++;
            printf("Yi[%d] = %d!!\n", i, Yi[i]);
        }
    }

    if(err == 0)
        printf("TEST PASSED!!\n");
    else
        printf("TEST FAILED!!\n");

    // Trigger a second time
    *(uint32_t *)(PCM_HWPE_TRIG) = 0x000000001;

    return err;
}