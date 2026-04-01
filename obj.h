#ifndef OBJ_H
#define OBJ_H
#include "tinyobj_loader_c.h"
typedef struct {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t *shapes;
    size_t num_shapes;

    tinyobj_material_t *materials;
    size_t num_materials;

    char *relative_dir[32];
} obj_data_t;

obj_data_t obj_load_from_file(const char *filename);
void obj_print_statistic(obj_data_t obj);
char *obj_texture_filename(obj_data_t obj);
void obj_destroy(obj_data_t obj);
#endif /* OBJ_H */
