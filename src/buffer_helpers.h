#ifndef OS_BUFFER_HELPERS_H
#define OS_BUFFER_HELPERS_H

void os_memcpy( void* out, const void* in, uint32_t len )
{
    const uint8_t* in8 = in;
    uint8_t* out8 = out;
    while ( len-- )
        *out8++ = *in8++;
}

#endif //OS_BUFFER_HELPERS_H
