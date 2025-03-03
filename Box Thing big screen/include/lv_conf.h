#ifndef LV_CONF_H
#define LV_CONF_H

/*******************
 * General Options
 *******************/

/* Use a custom memory allocator? 0: use the built-in allocator */
#define LV_MEM_CUSTOM      0
/* Size of the memory available for `lv_malloc` in bytes. Adjust if needed. */
#define LV_MEM_SIZE        (16U * 1024U)

/*******************
 * Graphics Options
 *******************/

/* Enable 1-bit color depth for a monochrome display */
#define LV_COLOR_DEPTH     1

/* Horizontal and vertical resolution of the display */
#define LV_HOR_RES_MAX     128
#define LV_VER_RES_MAX     64



/* Refresh period in milliseconds (can be increased to reduce CPU usage) */
#define LV_DISP_DEF_REFR_PERIOD  10

#define LV_USE_PERF_MONITOR 1

/*******************
 * Feature Usage
 *******************/

/* Enable object animations */
#define LV_USE_ANIMATION   1

/* Do not use any GPU related features for a low‐speed monochrome display */
#define LV_USE_GPU         0

/* Disable file system if not used */
#define LV_USE_FILESYSTEM  0

/*******************
 * Font Options
 *******************/

/* For a small 128×64 display a tiny monospace font is recommended.
 * The "unscii" fonts are perfect for monochrome displays. */
#define LV_FONT_DEFAULT    &lv_font_montserrat_14

#define CONFIG_LV_FONT_MONTSERRAT_8  1

#if LV_COLOR_DEPTH == 1
#define LV_COLOR_WHITE lv_color_make(1, 1, 1)
#define LV_COLOR_BLACK lv_color_make(0, 0, 0)
#endif



#endif /*LV_CONF_H*/