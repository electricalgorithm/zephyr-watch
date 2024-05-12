SET(SOURCES screens/ui_HomeScreen.c
    screens/ui_SettingsScreen.c
    screens/ui_CalenderScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c)

add_library(ui ${SOURCES})
