#include "pico.h"
#include "pico_templates.c"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

pico_mesh_t pico_mesh_init(const char *name, int num_vertices, int num_faces,
                            int num_indices)
{
    pico_mesh_t mesh = { 0 };
    if (name == NULL || num_vertices < 1 || num_faces < 1 || num_indices < 1) {
        return mesh;
    }

    size_t name_size = (strlen(name) + 1);
    size_t vertices_size = (sizeof(float)* 3 * num_vertices);
    size_t faces_size = (sizeof(pico_face_t) * num_faces);
    size_t vertex_ids_size = (sizeof(int) * num_indices);
    size_t uvs_size = (sizeof(float) * 2 * num_indices);

    int name_offset = 0;
    int vertices_offset = name_offset + name_size;
    int faces_offset = vertices_offset + vertices_size;
    int vertex_ids_offset = faces_offset + faces_size;
    int uvs_offset = vertex_ids_offset + vertex_ids_size;


    size_t total_size = name_size + vertices_size +
                       faces_size + vertex_ids_size + uvs_size;

    mesh.data = malloc(total_size);
    if (mesh.data == NULL) {
        fprintf(stderr, "Failed to allocate memory for pico_mesh.\n");
        return mesh;
    }
    mesh.name = (const char *)(mesh.data + name_offset);
    strcpy((char *)mesh.name, name); /* weird compiler stuff */
    mesh.vertices = (float *)(mesh.data + vertices_offset);
    mesh.num_vertices = num_vertices;
    mesh.faces = (pico_face_t *)(mesh.data + faces_offset);
    mesh.num_faces = num_faces;
    for (int i = 0; i < num_faces; ++i) {
        mesh.faces[i].color = 0; /* does lua freak out if i do this? */
        mesh.faces[i].uvs = (float *)(mesh.data + uvs_offset);
        mesh.faces[i].vertex_ids = (int *)(mesh.data + vertex_ids_offset);
        mesh.faces[i].num_indices = -1;
    }

    return mesh;
}

void pico_mesh_destroy(pico_mesh_t *mesh)
{
    free(mesh->data);
    mesh->name = NULL;
    mesh->vertices = NULL;
    mesh->faces = NULL;
}

pico_texture_t pico_texture_init()
{
    pico_texture_t texture = { 0 };
    texture.pixels = calloc(PICO_TEXTURE_SIZE + 1, sizeof(unsigned char));
    if (texture.pixels == NULL) {
        fprintf(stderr, "Failed to allocate memory for pico texture.\n");
        return texture;
    }
    texture.pixels[PICO_TEXTURE_SIZE] = '\0'; /* cJSON REQUIRES it */
    texture.colors = (pico_color_t *)(texture.pixels + 0x4001);
    texture.shade_pal_1 = (int *)(texture.pixels + 0x40C1);
    texture.shade_pal_2 = (int *)(texture.pixels + 0x4101);
    return texture;
}

void pico_texture_destroy(pico_texture_t *texture)
{
    free(texture->pixels);
}
