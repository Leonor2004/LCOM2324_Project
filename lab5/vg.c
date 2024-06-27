#include <lcom/lcf.h>

#include "vg.h"
#include <math.h>

uint8_t *video_mem;         /* frame-buffer VM address (static global variable)*/ 

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bytes_per_pixel; /* Number of VRAM bits per pixel */

unsigned int vram_base;         /* VRAM's physical addresss */
unsigned int vram_size;         /* VRAM's size, but you can use the frame-buffer size, instead */
vbe_mode_info_t mode_info;

struct minix_mem_range addr;

int r;		

int (_vg_init)(uint16_t mode) {
    memset(&mode_info, 0, sizeof(mode_info));
    if (vbe_get_mode_info(mode, &mode_info) != 0) return 1;

    h_res = mode_info.XResolution;
    v_res = mode_info.YResolution;

    bytes_per_pixel =  ((mode_info.BitsPerPixel + 7)/ 8.0);

    vram_base = mode_info.PhysBasePtr;
    vram_size = h_res * v_res * bytes_per_pixel;

    /* Allow memory mapping */
    addr.mr_base = mode_info.PhysBasePtr;	
    addr.mr_limit = addr.mr_base + vram_size;  

    if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &addr))
        panic("sys_privctl failed: %d\n", r);

    /* Map memory */

    video_mem = vm_map_phys(SELF, (void *)addr.mr_base, vram_size);

    if(video_mem == NULL)
        panic("couldn't map video memory");
    
    if (enter_video_mode(mode) != 0) return 1;

    return 0;
}

int (vg_draw_pattern)(bool indexed, uint8_t no_rectangles, uint8_t first, uint8_t step) {
    uint16_t height = mode_info.YResolution/no_rectangles;
    uint16_t width = mode_info.XResolution/no_rectangles;

    for(int r = 0; r < no_rectangles; r++) {
        for(int c = 0; c < no_rectangles; c++) {
            if (indexed) {
                vg_draw_rectangle(width*c, height*r, width, height, (first + (r * no_rectangles + c) * step) % (1 << mode_info.BitsPerPixel));
            }
            else {
                uint32_t R = ((1 << mode_info.RedMaskSize) - 1) & (first >> mode_info.RedFieldPosition);
                R = (R + c * step) % (1 << mode_info.RedMaskSize);

                uint32_t G = ((1 << mode_info.GreenMaskSize) - 1) & (first >> mode_info.GreenFieldPosition);
                G = (G + r * step) % (1 << mode_info.GreenMaskSize);

                uint32_t B = ((1 << mode_info.BlueMaskSize) - 1) & (first >> mode_info.BlueFieldPosition);
                B = (B + (r + c) * step) % (1 << mode_info.BlueMaskSize);

                uint32_t color = (R << mode_info.RedFieldPosition) | (G << mode_info.GreenFieldPosition) | (B << mode_info.BlueFieldPosition);
                vg_draw_rectangle(width*c, height*r, width, height, color);
            }
        }
    }
    return 0;
}

int (enter_video_mode)(uint16_t mode) {
    reg86_t reg;

    memset(&reg, 0, sizeof(reg)); /* zero the structure */
    reg.intno = 0x10;
    reg.ah = 0x4F;
    reg.al = 0x02;
    reg.bx = BIT(14) | mode;

    if(sys_int86(&reg) != 0 ) return 1;

    return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if(x > h_res || y > v_res || x < 0 || y < 0) return 1;

    unsigned int pos = (unsigned int)(video_mem + (y * h_res + x) * bytes_per_pixel);

    memcpy((void*)pos, &color, bytes_per_pixel);

    return 0;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (int i = 0; i < len; i++){
        vg_draw_pixel(x + i, y, color);
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (int j = 0; j < height; j++){
        vg_draw_hline(x, y + j, width, color);
    }
    return 0;
}

int (bitsPerPixel)() {
    return mode_info.BitsPerPixel;
}
