#ifndef DIALOGUE_H
#define DIALOGUE_H

enum {
    DIALOGUE_RESET =        (0 << 0),
    DIALOGUE_OBJ_VALID =    (1 << 0),
    DIALOGUE_PNG_VALID =    (1 << 1),
    DIALOGUE_EXPORT_VALID = (1 << 2) 
};

void dialogue_file_select(void);
#endif /* DIALOGUE_H */
