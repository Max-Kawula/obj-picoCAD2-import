#include <stddef.h>
#include <string.h>
#include "raylib.h"
#include "rlgl.h"
#include "microui.h"

/*
 * Microui state.
 * User does not interact directly with the gui, rather calls
 * the three main processes:
 *  - Poll inputs
 *  - Process gui events
 *  - Draw gui
 */
static mu_Context Context = {0};
mu_Context *GuiCtx = &Context;
static float GuiFontSpacing = 1.0f;
static int GuiFontSize = 12;
static Font GuiFont = {0};

/* ================ *
 * STATIC FUNCTIONS *
 * ================ */
static int text_width(mu_Font font, const char *text, int len) {
    /* XXX major assumption 1.0f spacing is the default */
    Vector2 size = MeasureTextEx(GuiFont, text, GuiFontSize, GuiFontSpacing);
    return (int)size.x;
}

static int text_height(mu_Font font) {
    return GuiFontSize;
}

static void gui_draw_text(const char *text, mu_Vec2 position, mu_Color color)
{
    Color text_color = { color.r, color.g, color.b, color.a };
    Vector2 text_position = {position.x, position.y};
    DrawTextEx(GuiFont, text, text_position, GuiFontSize, GuiFontSpacing, text_color);
}

static void gui_draw_rect(mu_Rect rect, mu_Color color)
{
    Color rect_color = { color.r, color.g, color.b, color.a };
    DrawRectangle(rect.x, rect.y, rect.w, rect.h, rect_color);
}


static void gui_clip_rect(mu_Rect rect)
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    //gui_flush(width, height);
    rlScissor(rect.x, height - (rect.y + rect.h), rect.w, rect.h);
}

/* ==================== *
 * CUSTOM GUI GOES HERE *
 * ==================== */
void gui_test_window(void)
{
    mu_begin(GuiCtx);
    if (mu_begin_window(GuiCtx, "My Window", mu_rect(10, 10, 300, 400))) {
        if (mu_button(GuiCtx, "My Button")) {
            TraceLog(LOG_INFO, "'My Button' was pressed\n");
        }
        mu_end_window(GuiCtx);
    }
    mu_end(GuiCtx);
}

/* ================ *
 * PUBLIC FUNCTIONS *
 * ================ */
void gui_init(void)
{
    mu_init(GuiCtx);
    GuiCtx->text_width = text_width;
    GuiCtx->text_height = text_height;
}

int gui_set_font_size(int size)
{
    GuiFontSize = size;
}

void gui_poll_input(void)
{
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();

    mu_input_mousemove(GuiCtx, mouse_x, mouse_y);

    int mouse_button_down = 0;
    int mouse_button_up = 0;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))   { mouse_button_down |= MU_MOUSE_LEFT; }
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))  { mouse_button_down |= MU_MOUSE_RIGHT; }
    if(IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) { mouse_button_down |= MU_MOUSE_MIDDLE; }
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))     { mouse_button_up |= MU_MOUSE_LEFT; }
    if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))    { mouse_button_up |= MU_MOUSE_RIGHT; }
    if(IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))   { mouse_button_up |= MU_MOUSE_MIDDLE; }
    mu_input_mousedown(GuiCtx, mouse_x, mouse_y, mouse_button_down);
    mu_input_mouseup(GuiCtx, mouse_x, mouse_y, mouse_button_up);

    Vector2 mouse_wheel_delta = GetMouseWheelMoveV();
    mu_input_scroll(GuiCtx, (int)mouse_wheel_delta.x, (int)mouse_wheel_delta.y);

    int key_down = 0;
    int key_up = 0;
    if(IsKeyPressed(KEY_LEFT_SHIFT))        { key_down |= MU_KEY_SHIFT; }
    if(IsKeyPressed(KEY_RIGHT_SHIFT))       { key_down |= MU_KEY_SHIFT; }
    if(IsKeyPressed(KEY_LEFT_CONTROL))      { key_down |= MU_KEY_CTRL; }
    if(IsKeyPressed(KEY_RIGHT_CONTROL))     { key_down |= MU_KEY_CTRL; }
    if(IsKeyPressed(KEY_LEFT_ALT))          { key_down |= MU_KEY_ALT; }
    if(IsKeyPressed(KEY_RIGHT_ALT))         { key_down |= MU_KEY_ALT; }
    if(IsKeyPressed(KEY_ENTER))             { key_down |= MU_KEY_RETURN; }
    if(IsKeyPressed(KEY_BACKSPACE))         { key_down |= MU_KEY_BACKSPACE; }

    if(IsKeyReleased(KEY_LEFT_SHIFT))       { key_up |= MU_KEY_SHIFT; }
    if(IsKeyReleased(KEY_RIGHT_SHIFT))      { key_up |= MU_KEY_SHIFT; }
    if(IsKeyReleased(KEY_LEFT_CONTROL))     { key_up |= MU_KEY_CTRL; }
    if(IsKeyReleased(KEY_RIGHT_CONTROL))    { key_up |= MU_KEY_CTRL; }
    if(IsKeyReleased(KEY_LEFT_ALT))         { key_up |= MU_KEY_ALT; }
    if(IsKeyReleased(KEY_RIGHT_ALT))        { key_up |= MU_KEY_ALT; }
    if(IsKeyReleased(KEY_ENTER))            { key_up |= MU_KEY_RETURN; }
    if(IsKeyReleased(KEY_BACKSPACE))        { key_up |= MU_KEY_BACKSPACE; }
    mu_input_keydown(GuiCtx, key_down);
    mu_input_keyup(GuiCtx, key_up);
    
    const int max_keys_pressed = 8;
    int num_keys_pressed = 0;
    char keys_pressed[max_keys_pressed + 1];
    for (int i = 0; i < max_keys_pressed + 1; ++i) {
        keys_pressed[i] = 0;
    }
    int key = 0;
    while ((key = GetCharPressed()) && (num_keys_pressed < max_keys_pressed)) {
        keys_pressed[num_keys_pressed++] = key;
    }
    mu_input_text(GuiCtx, keys_pressed);
}

void gui_process()
{
}

void gui_render(void)
{
    mu_Command *cmd = NULL;
    while (mu_next_command(GuiCtx, &cmd)) {
        switch(cmd->type) {
            case MU_COMMAND_JUMP:
                break; /* I think this is internally used for something but idk... */
            case MU_COMMAND_CLIP:
                gui_clip_rect(cmd->rect.rect); break;
            case MU_COMMAND_RECT:
                gui_draw_rect(cmd->rect.rect, cmd->rect.color); break;
            case MU_COMMAND_TEXT:
                gui_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
            case MU_COMMAND_ICON:
                break;
            default:
                break;
        }
    }
}

/* not sure if needed since raylib cleansup between draw calls */
#if 0
static void gui_flush(int width, int height)
{
    rlViewport(0, 0, width, height); // Set viewport area
    rlMatrixMode(RL_PROJECTION);    // select matrix to be transformed
    rlPushMatrix(); // Push current matrix into stack XXX rlgl stack and not internal?
    rlLoadIdentity(); // reset current matrix
    rlOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    rlMatrixMode(RL_MODELVIEW);
    rlPushMatrix();
    rlLoadIdentity();

    // glTexCoordPointer()
    // glVertexPointer()
    // glColorPointer()
    // glDrawElements()

    rlMatrixMode(RL_MODELVIEW);
    rlPopMatrix();
    rlMatrixMode(RL_PROJECTION);
    rlPopMatrix();
}
#endif
