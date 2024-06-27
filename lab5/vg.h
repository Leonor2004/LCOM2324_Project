#ifndef _LCOM_GRAPHICSCARD_H_
#define _LCOM_GRAPHICSCARD_H_

int (isESC)(void);
int (_vg_init)(uint16_t mode);
int (enter_video_mode)(uint16_t mode);
int (vg_draw_pattern)(bool indexed, uint8_t no_rectangles, uint8_t first, uint8_t step);
uint32_t (get_color)(int x, int y, uint8_t step, uint8_t no_rectangles);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (bitsPerPixel)();

#endif
