#include "lvgl.h"
#include "display/display.h"


void display_init() {
    lv_disp_t *display = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(
        display,
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_RED),
        true,
        LV_FONT_DEFAULT
    );
    lv_disp_set_theme(display, theme);
    home_screen_init();
    lv_disp_load_scr(home_screen);
}
