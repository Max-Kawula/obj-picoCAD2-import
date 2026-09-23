#define GUI_CUSTOM
#include "gui.h"

static int DialogueState = 0;

void dialogue_file_select(void)
{
    mu_begin(GuiCtx);
    if (mu_begin_window(GuiCtx, "Obj2PicoCAD2", mu_rect(40, 40, 300, 450))) {
        static char obj_path[128];
        if (mu_textbox(GuiCtx, obj_path, sizeof(obj_path)) & MU_RES_SUBMIT) {
            obj_path[0] = '\0';
        }
        mu_end_window(GuiCtx);
    }
    mu_end(GuiCtx);
}

