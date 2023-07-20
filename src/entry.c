#include <stdlib.h>
#include <limine.h>

#include "debug.h"
#include "int.h"
#include "phys.h"
#include "pic.h"

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_bootloader_info_request info_request = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0
};

void _start(void)
{
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    for (size_t i = 0; i < 100; i++) {
        uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }

    debug_init();
    debug_write_string("booted with ");
    debug_write_string(info_request.response->name);
    debug_write_string(" ");
    debug_write_string(info_request.response->version);
    debug_write_string(", information revision 0x");
    debug_write_uint64(info_request.response->revision);
    debug_write_byte('\n');

    gdt_init();
    phys_init(memmap_request.response);
    int_init();
    pic_init();

    while (1) {
        __asm__("hlt");
    }
}
