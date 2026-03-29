#ifndef OBJ_H
#define OBJ_H
#include "tinyobj_loader_c.h"
typedef struct {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t *shapes;
    size_t num_shapes;
} obj_data_t;

obj_data_t load_obj(const char *filename);
#endif /* OBJ_H */
