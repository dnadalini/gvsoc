//#include <stdio.h>
#include <stdint.h>

int main() {

    // Set PCM parameters (all LA, sector 1)
    *(volatile uint32_t *)(0x41004) = 0x10FFFF01;

    // Write Xis
    /* TO BE IMPLEMENTED */

    // Start PCM computation
    *(volatile uint32_t *)(0x41004) = 0x00FFFF01;

    // Read results
    for (uint32_t i=0; i<512; i++) {
        printf("Y[%d] = %d\n", i, *(volatile int8_t *)(0x41100 + i));
    }

    return 0;
}