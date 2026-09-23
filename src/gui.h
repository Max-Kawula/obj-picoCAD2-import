#ifndef GUI_H
#define GUI_H

/* Optionally expose the context if you wish to use. */
#ifdef GUI_CUSTOM
#include "microui.h"
extern mu_Context *GuiCtx;
#endif /* GUI_CUSTOM */

void gui_init(void);
void gui_set_font_size(int);
void gui_poll_input(void);
void gui_test_window(void);
void gui_render(void);


#endif /* GUI_H */
