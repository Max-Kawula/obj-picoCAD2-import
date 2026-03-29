#ifndef PICO_H
#define PICO_H
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

typedef struct {
    unsigned char *data;
    const char* name;
    float *vertices;
    int num_vertices;
    pico_face_t *faces;
    int num_faces;
} pico_mesh_t;

pico_mesh_t pico_mesh_init(const char *name, int num_vertices, int num_faces,
                            int num_indices);
void pico_mesh_destroy(pico_mesh_t *mesh);
#endif /* PICO_H */
