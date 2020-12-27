#ifndef VULNERABILITIES_H
#define VULNERABILITIES_H


// ==== Vulnerabilities ==== 
#define CRASH             0x1
#define HEAP_OVERFLOW     0x2
#define INFO_LEAK         0x3
#define BUFFER_OVERFLOW   0x4
#define USE_AFTER_FREE    0x5
#define INTEGER_OVERFLOW  0x6
#define DOUBLE_FETCH      0x7

errno_t trigger_crash(uint32_t a);
errno_t trigger_double_fetch(void *data, size_t data_len);
errno_t trigger_heap_overflow(void *data, size_t data_len);
errno_t trigger_arbitrary_write(void *data, size_t data_len);
errno_t trigger_buffer_overflow(void *data, size_t data_len);
errno_t trigger_integer_overflow(void *data, size_t data_len);
errno_t trigger_info_leak(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, int opt, void *data, size_t data_len);
errno_t trigger_use_after_free(kern_ctl_ref ctlref, uint32_t unit, void *unitinfo, int opt, void *data, size_t data_len);

#endif
