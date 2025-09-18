#include <xjos/xjos.h>
#include <xjos/types.h>
#include <xjos/io.h>
#include <xjos/string.h>
#include <xjos/console.h>


char message[] = "Hello, world!\n";

void kernel_init()
{
    console_init();

    u32 i = 10000;

    while (i--) {
        console_write(message, strlen(message));
    }

    return;
}
