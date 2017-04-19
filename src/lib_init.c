#include <libmpm.h>

static void __attribute__((constructor)) mpm_init(void) {
    sodium_init();
}
