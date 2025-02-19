#ifndef LV_CONF_H
#define LV_CONF_H

/* Enable/Disable features */
#define LV_COLOR_DEPTH     1  // or use 1 for a strictly monochrome configuration
#define LV_HOR_RES_MAX     128
#define LV_VER_RES_MAX     64

/* Enable anti-aliasing if needed (usually disabled for monochrome) */
#define LV_ANTIALIAS       0

/* Memory size for LVGL's internal allocations */
#define LV_MEM_SIZE        (16 * 1024)  

/* Debugging options */
#define LV_USE_DEBUG       1

// Additional configuration options...

#endif /*LV_CONF_H*/
