/* iso_alloc thread_tests.c
 * Copyright 2020 - chris.rohlf@gmail.com */

#include "iso_alloc.h"
#include "iso_alloc_internal.h"

uint32_t allocation_sizes[] = {ZONE_16, ZONE_32, ZONE_64, ZONE_128,
                               ZONE_256, ZONE_512, ZONE_1024,
                               ZONE_2048, ZONE_4096, ZONE_8192};

uint32_t array_sizes[] = {16, 32, 64, 128, 256, 512, 1024,
                          2048, 4096, 8192, 16384, 32768, 65536};

void *allocate() {
    size_t array_size;
    size_t allocation_size;
    int32_t alloc_count = 0;

    for(int i = 0; i < sizeof(array_sizes) / sizeof(uint32_t); i++) {
        for(int z = 0; z < sizeof(allocation_sizes) / sizeof(uint32_t); z++) {
            array_size = array_sizes[i];
            allocation_size = allocation_sizes[z];
            void *p[array_size];
            memset(p, 0x0, array_size);

            for(int i = 0; i < array_size; i++) {
                if(allocation_size == 0) {
                    allocation_size = allocation_sizes[(rand() % sizeof(allocation_sizes) / sizeof(uint32_t))] + (rand() % 32);
                }

                p[i] = iso_alloc(allocation_size);

                if(p[i] == NULL) {
                    LOG_AND_ABORT("Failed to allocate %ld bytes after %d total allocations", allocation_size, alloc_count);
                }

                alloc_count++;

                memset(p[i], 0x41, allocation_size);

                /* Randomly free some allocations */
                if((rand() % 5) > 1) {
                    iso_free(p[i]);
                    p[i] = NULL;
                }
            }

            /* Free the remaining allocations */
            for(int i = 0; i < array_size; i++) {
                if(p[i] != NULL) {
                    iso_free(p[i]);
                }
            }
        }
    }

    return OK;
}

int main(int argc, char *argv[]) {
#if THREAD_SUPPORT
    pthread_t to;
    pthread_t tt;
    pthread_create(&to, NULL, allocate, NULL);
    pthread_create(&tt, NULL, allocate, NULL);
    pthread_join(tt, NULL);
    pthread_exit(NULL);

    iso_verify_zones();
#endif

    return OK;
}
