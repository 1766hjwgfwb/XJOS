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
    unsigned char *p = (unsigned char *)dest;
    unsigned char val = (unsigned char)ch;

    for (size_t i = 0; i < count; i++) {
        p[i] = val; // Set byte value
    }

    return dest;
}


void *memcpy(void *dest, const void *src, size_t count) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    for (size_t i = 0; i < count; i++) {
        d[i] = s[i]; // Copy byte by byte
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