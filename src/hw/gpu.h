#ifndef GPU_H
#define GPU_H

#include "libc/stdint.h"

#define GPU_GP0 0x9f801810
#define GPU_GP1 0x9f801814

static uint32_t xy;
static uint32_t h;
static int h_shift;
static uint8_t attr;
static uint8_t prev_attr;

void gpu_init(const uint32_t* font, int height);
void gpu_scroll_down(void);
void gpu_putchar(int c);
void gpu_set_attribute(uint8_t c);
void gpu_restore_attribute(void);
void gpu_clear(void);
uint32_t gpu_get_xpos(void);
uint32_t gpu_get_ypos(void);
uint32_t gpu_get_pos(void);
void gpu_set_xpos(uint32_t pos);
void gpu_set_ypos(uint32_t pos);
void gpu_set_pos(uint32_t pos);

static uint32_t g_clut[] = {
    0x63180421, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000,
    0x00000000, 0x00000000
};

static const uint16_t rgb555_palette[] = {
    0x0421, 0x0010, 0x0200, 0x0210,
    0x4000, 0x4010, 0x4200, 0x6318,
    0x4210, 0x001f, 0x03e0, 0x03ff,
    0x7c00, 0x7c1f, 0x7fe0, 0x7fff,
};

static const uint32_t clut_palette[] = {
    0x04210421, 0x04210010, 0x04210200, 0x04210210, 0x04214000, 0x04214010, 0x04214200, 0x04216318,
    0x04214210, 0x0421001f, 0x042103e0, 0x042103ff, 0x04217c00, 0x04217c1f, 0x04217fe0, 0x04217fff,
    0x00100421, 0x00100010, 0x00100200, 0x00100210, 0x00104000, 0x00104010, 0x00104200, 0x00106318,
    0x00104210, 0x0010001f, 0x001003e0, 0x001003ff, 0x00107c00, 0x00107c1f, 0x00107fe0, 0x00107fff,
    0x02000421, 0x02000010, 0x02000200, 0x02000210, 0x02004000, 0x02004010, 0x02004200, 0x02006318,
    0x02004210, 0x0200001f, 0x020003e0, 0x020003ff, 0x02007c00, 0x02007c1f, 0x02007fe0, 0x02007fff,
    0x02100421, 0x02100010, 0x02100200, 0x02100210, 0x02104000, 0x02104010, 0x02104200, 0x02106318,
    0x02104210, 0x0210001f, 0x021003e0, 0x021003ff, 0x02107c00, 0x02107c1f, 0x02107fe0, 0x02107fff,
    0x40000421, 0x40000010, 0x40000200, 0x40000210, 0x40004000, 0x40004010, 0x40004200, 0x40006318,
    0x40004210, 0x4000001f, 0x400003e0, 0x400003ff, 0x40007c00, 0x40007c1f, 0x40007fe0, 0x40007fff,
    0x40100421, 0x40100010, 0x40100200, 0x40100210, 0x40104000, 0x40104010, 0x40104200, 0x40106318,
    0x40104210, 0x4010001f, 0x401003e0, 0x401003ff, 0x40107c00, 0x40107c1f, 0x40107fe0, 0x40107fff,
    0x42000421, 0x42000010, 0x42000200, 0x42000210, 0x42004000, 0x42004010, 0x42004200, 0x42006318,
    0x42004210, 0x4200001f, 0x420003e0, 0x420003ff, 0x42007c00, 0x42007c1f, 0x42007fe0, 0x42007fff,
    0x63180421, 0x63180010, 0x63180200, 0x63180210, 0x63184000, 0x63184010, 0x63184200, 0x63186318,
    0x63184210, 0x6318001f, 0x631803e0, 0x631803ff, 0x63187c00, 0x63187c1f, 0x63187fe0, 0x63187fff,
    0x42100421, 0x42100010, 0x42100200, 0x42100210, 0x42104000, 0x42104010, 0x42104200, 0x42106318,
    0x42104210, 0x4210001f, 0x421003e0, 0x421003ff, 0x42107c00, 0x42107c1f, 0x42107fe0, 0x42107fff,
    0x001f0421, 0x001f0010, 0x001f0200, 0x001f0210, 0x001f4000, 0x001f4010, 0x001f4200, 0x001f6318,
    0x001f4210, 0x001f001f, 0x001f03e0, 0x001f03ff, 0x001f7c00, 0x001f7c1f, 0x001f7fe0, 0x001f7fff,
    0x03e00421, 0x03e00010, 0x03e00200, 0x03e00210, 0x03e04000, 0x03e04010, 0x03e04200, 0x03e06318,
    0x03e04210, 0x03e0001f, 0x03e003e0, 0x03e003ff, 0x03e07c00, 0x03e07c1f, 0x03e07fe0, 0x03e07fff,
    0x03ff0421, 0x03ff0010, 0x03ff0200, 0x03ff0210, 0x03ff4000, 0x03ff4010, 0x03ff4200, 0x03ff6318,
    0x03ff4210, 0x03ff001f, 0x03ff03e0, 0x03ff03ff, 0x03ff7c00, 0x03ff7c1f, 0x03ff7fe0, 0x03ff7fff,
    0x7c000421, 0x7c000010, 0x7c000200, 0x7c000210, 0x7c004000, 0x7c004010, 0x7c004200, 0x7c006318,
    0x7c004210, 0x7c00001f, 0x7c0003e0, 0x7c0003ff, 0x7c007c00, 0x7c007c1f, 0x7c007fe0, 0x7c007fff,
    0x7c1f0421, 0x7c1f0010, 0x7c1f0200, 0x7c1f0210, 0x7c1f4000, 0x7c1f4010, 0x7c1f4200, 0x7c1f6318,
    0x7c1f4210, 0x7c1f001f, 0x7c1f03e0, 0x7c1f03ff, 0x7c1f7c00, 0x7c1f7c1f, 0x7c1f7fe0, 0x7c1f7fff,
    0x7fe00421, 0x7fe00010, 0x7fe00200, 0x7fe00210, 0x7fe04000, 0x7fe04010, 0x7fe04200, 0x7fe06318,
    0x7fe04210, 0x7fe0001f, 0x7fe003e0, 0x7fe003ff, 0x7fe07c00, 0x7fe07c1f, 0x7fe07fe0, 0x7fe07fff,
    0x7fff0421, 0x7fff0010, 0x7fff0200, 0x7fff0210, 0x7fff4000, 0x7fff4010, 0x7fff4200, 0x7fff6318,
    0x7fff4210, 0x7fff001f, 0x7fff03e0, 0x7fff03ff, 0x7fff7c00, 0x7fff7c1f, 0x7fff7fe0, 0x7fff7fff,
};

#endif