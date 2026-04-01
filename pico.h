#ifndef PICO_H
#define PICO_H

#define PICO_TEXTURE_SIZE 0x4141

extern const char *pico_empty_object;
extern const char *pico_empty_scene;
extern const char *pico_default_transform;
extern const char *pico_default_motions;

typedef struct {
    int color;
    int *vertex_ids;
    float *uvs;
    int num_indices;
} pico_face_t;

/* data is a generic buffer for storing all the data
 * in a contigous format. i would not use it directly
 * TODO just assign the buffer to name :) */
typedef struct {
    unsigned char *data;
    const char* name;
    float *vertices;
    int num_vertices;
    pico_face_t *faces;
    int num_faces;
} pico_mesh_t;

/* picoCAD uses floats for the color palette's base colors */
typedef struct {
    float r, g, b;
} pico_color_t;

/* texture is a fixed size, but i still want to heap
 * allocate because i'm not sure how much stack size we'll have
 * i also found out that we need the pixels to be a null-terminated
 * string for cJSON to accept it */
typedef struct {               /* offsets */
    unsigned char *pixels;     /* 0x0 */
    unsigned char background;
    unsigned char alpha;
    pico_color_t *colors;      /* 0x4001 */
    int *shade_pal_1;          /* 0x40C1 */
    int *shade_pal_2;          /* 0x4101 *//* if my math is wrong im fucked */
} pico_texture_t;

pico_mesh_t pico_mesh_init(const char *name, int num_vertices, int num_faces,
                            int num_indices);
void pico_mesh_destroy(pico_mesh_t *mesh);

pico_texture_t pico_texture_init();
void pico_texture_destroy(pico_texture_t *texture);
pico_texture_t pico_texture_from_png(const char *png_filename);
#endif /* PICO_H */
