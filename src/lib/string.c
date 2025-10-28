#include <libc/string.h>


char *strcpy(char *dest, const char *src) {
    char *ptr = dest;

    if (dest == NULL || src == NULL) {
        return NULL;
    }

    while (*src != EOS) {
        *ptr++ = *src++;
    }

    *ptr = EOS;

    return dest;
}


char *strcat(char *dest, const char *src) {
    char *ptr = dest;

    while (*ptr != EOS) {
        ptr++;
    }

    while (*src != EOS) {
        *ptr++ = *src++;
    }

    *ptr = EOS;

    return dest;
}


size_t strlen(const char *str) {
    size_t len = 0;

    while (*str != EOS) {
        str++;
        len++;
    }

    return len;
}


int strcmp(const char *lhs, const char *rhs) {
    while (*lhs == *rhs && *lhs != EOS) {
        lhs++;
        rhs++;
    }

    return (int)*lhs - (int)*rhs;
}


char *strchr(const char *str, int ch) {
    char c = (char)ch; 
    
    // Loop until the null terminator is reached
    while (*str != '\0') {
        if (*str == c) {
            return (char *)str; // Found the character
        }
        str++;
    }
    
    // Check for the null terminator itself
    if (c == '\0') {
        return (char *)str; 
    }

    return NULL; // Not found
}


char *strrchr(const char *str, int ch) {
    char c = (char)ch;
    char *last = NULL; // Pointer to store the last found position
    
    // Loop through the entire string, including the null terminator
    while (1) {
        if (*str == c) {
            last = (char *)str; // Update the last position
        }

        if (*str == '\0') {
            break; // End of string, stop loop
        }
        str++;
    }

    return last; // Return the last recorded position
}


int memcmp(const void *lhs, const void *rhs, size_t count) {
    // Cast to unsigned char* for byte-by-byte comparison
    const unsigned char *p1 = (const unsigned char *)lhs;
    const unsigned char *p2 = (const unsigned char *)rhs;

    for (size_t i = 0; i < count; i++) {
        if (p1[i] < p2[i]) {
            return -1; // lhs is less than rhs
        } else if (p1[i] > p2[i]) {
            return 1; // lhs is greater than rhs
        }
    }

    return 0; // Memory blocks are equal
}


void *memset(void *dest, int ch, size_t count) {
    u8 *d = (u8 *)dest;
    u8 val = (u8)ch;

    // ch = 0xAB -> val32 = 0xABABABAB
    u32 val32 = (val << 24) | (val << 16) | (val << 8) | val;

    // head 
    while (count > 0 && ((u32)d & 0x3) != 0) {
        *d++ = val;
        count--;
    }

    // body
    u32 *d32 = (u32 *)d;
    while (count >= 4) {
        *d32++ = val32;
        count -= 4;
    }

    // tail
    d = (u8 *)d32;
    while (count > 0) {
        *d++ = val;
        count--;
    }

    return dest;
}


void *memcpy(void *dest, const void *src, size_t count) {
    u8* d = (u8 *)dest;
    const u8* s = (const u8 *)src;

    // dest and src are not aligned
    if (((u32)d & 0x3) != ((u32)s & 0x3)) {
        while (count--) {
            *d++ = *s++;
        }
        return dest;
    }

    // dest and src are aligned

    // head
    while (count > 0 && ((u32)d & 0x3) != 0) {
        *d++ = *s++;
        count--;
    }

    // body
    u32 *d32 = (u32 *)d;
    const u32 *s32 = (const u32 *)s;
    while (count >= 4) {
        *d32++ = *s32++;
        count -= 4;
    }

    // tail
    d = (u8 *)d32;
    s = (const u8 *)s32;
    while (count > 0) {
        *d++ = *s++;
        count--;
    }

    return dest;
}


void *memchr(const void *ptr, int ch, size_t count) {
    const unsigned char *p = (const unsigned char *)ptr;
    unsigned char c = (unsigned char)ch;

    for (size_t i = 0; i < count; i++) {
        if (p[i] == c) {
            return (void *)&p[i]; // Found the character
        }
    }

    return NULL; // Not found within the specified count
}