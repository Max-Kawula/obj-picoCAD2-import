#include "cJSON.h"
#include "obj.h"
#include "pico.h"
#include <stdio.h>
#include <string.h>

cJSON *marshal_pico_mesh(pico_mesh_t p_mesh);
cJSON *marshal_pico_texture(pico_texture_t p_texture);
pico_mesh_t obj_to_pico_mesh(obj_data_t obj, const char *name, int color);

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s FILE.obj [TEXTURE.png]\n", argv[0]);
        return 1;
    }

    /* check if we can write to the file */
    FILE *output_file;
    output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        fprintf(stderr, "ERROR: Failed to open output file. Exiting.\n");
        return 1;
    }

    const char *obj_file = argv[1];
    const char *png_file = NULL;
    if (argc == 3) {
        png_file = argv[2];
    }

    obj_data_t obj_data = obj_load_from_file(obj_file);

    if (obj_data.attrib.num_vertices == 0) {
        fprintf(stderr, "ERROR: Obj did not load\n");
        return 1;
    }

    cJSON *scene = cJSON_Parse(pico_empty_scene);
    if (scene == NULL) {
        fprintf(stderr, "Failed to parse scene JSON.\n");
        return 1;
    }

    /* we obtain the scene graph and place an empty folder named "obj" */
    cJSON *graph = cJSON_GetObjectItem(scene, "graph");
    cJSON *graph_children = cJSON_GetObjectItem(graph, "children");
    /* FIXME we should look for the item with "name": "obj", not the first one */
    cJSON *obj_folder = cJSON_GetArrayItem(graph_children, 0);
    cJSON *obj_folder_children = cJSON_GetObjectItem(obj_folder, "children");

    /* to add the mesh, we load in an emtpy object.
     * this just contains some object properties and transforms. */
    cJSON *base_object = cJSON_Parse(pico_empty_object);
    if (base_object == NULL) {
        fprintf(stderr, "Failed to parse object JSON.\n");
        return 1;
    }
    cJSON_AddStringToObject(base_object, "name", "base");

    /* create the mesh. we should probably change it from "cat" */
    pico_mesh_t mesh = obj_to_pico_mesh(obj_data, "cat", 5);
    cJSON *json_mesh = marshal_pico_mesh(mesh);
    cJSON_AddItemToObject(base_object, "mesh", json_mesh);
    cJSON_AddItemToArray(obj_folder_children, base_object);

    /* Add the texture. First, we see if the obj had one.
     * If the user supplies one as an argument we use that one instead.
     * If user does not supply one at all, we just use the default picoCAD 2 texture. */
    char *obj_texture_name = obj_texture_filename(obj_data);
    cJSON *json_texture = NULL;
    pico_texture_t pico_texture = { 0 };
    if (png_file) {
        printf("INFO: External texture located.\n");
        pico_texture = pico_texture_from_png(png_file);
    } else if (obj_texture_name) {
        printf("INFO: Obj texture located.\n");
        pico_texture = pico_texture_from_png(obj_texture_name);
    } else {
        printf("INFO: No texture supplied. Using default texture.\n");
    }

    if (pico_texture.pixels) {
        cJSON_DeleteItemFromObject(scene, "texture");
        json_texture = marshal_pico_texture(pico_texture);
        cJSON_AddItemToObject(scene, "texture", json_texture);
    }

    /* output final json string */
    char *output_string = cJSON_Print(scene);
    fprintf(output_file, output_string);
    fclose(output_file);
    printf("INFO: File saved to: ./output.txt\n");

    obj_destroy(obj_data);
    pico_mesh_destroy(&mesh);
    pico_texture_destroy(&pico_texture);
    return 0;
}

cJSON *marshal_pico_mesh(pico_mesh_t p_mesh)
{
    cJSON *mesh = NULL;
    cJSON *vertices = NULL;
    cJSON *faces = NULL;

    mesh = cJSON_CreateObject();
    cJSON_AddStringToObject(mesh, "name", p_mesh.name);

    vertices = cJSON_CreateFloatArray(p_mesh.vertices, p_mesh.num_vertices*3);
    cJSON_AddItemToObject(mesh, "vertices", vertices);

    faces = cJSON_CreateArray();

    for (int i = 0; i < p_mesh.num_faces; ++i) {
        pico_face_t *p_face = p_mesh.faces + i;
        cJSON *face = cJSON_CreateObject();
        cJSON *vertex_ids = cJSON_CreateArray();
        cJSON *uvs = cJSON_CreateArray();
        for (int j = 0; j < p_face->num_indices; ++j) {
            cJSON *index = cJSON_CreateNumber(p_face->vertex_ids[j]);
            cJSON *uv0 = cJSON_CreateNumber(p_face->uvs[j*2 + 0]);
            cJSON *uv1 = cJSON_CreateNumber(p_face->uvs[j*2 + 1]);
            cJSON_AddItemToArray(vertex_ids, index);
            cJSON_AddItemToArray(uvs, uv0);
            cJSON_AddItemToArray(uvs, uv1);
        }
        cJSON_AddItemToObject(face, "vertex_ids", vertex_ids);
        cJSON_AddItemToObject(face, "uvs", uvs);
        cJSON_AddNumberToObject(face, "color", p_face->color);
        cJSON_AddItemToArray(faces, face);
    }
    cJSON_AddItemToObject(mesh, "faces", faces);
    return mesh;
}

cJSON *marshal_pico_texture(pico_texture_t p_texture)
{
                    /* "texture": { */
    cJSON *texture;     /* "pixels": "001122abcdef..." */
                        /* "background_color:" int */
                        /* "transparent_color:" int */
    cJSON *colors;      /* "colors": [ [0.0, 0.0, 0.0], [n = 16] ] */
    cJSON *shade_pal_1; /* "shade_pal_1": [16] */
    cJSON *shade_pal_2; /* "shade_pal_2": [16] */
                    /* } */

    texture = cJSON_CreateObject();
    cJSON_AddStringToObject(texture, "pixels", (const char *)p_texture.pixels);
    cJSON_AddNumberToObject(texture, "background_color", 0);
    cJSON_AddNumberToObject(texture, "transparent_color", 0);

    colors = cJSON_CreateArray();
    for (int i = 0; i < 16; ++i) {
        cJSON *color;
        float values[3];
        values[0] = p_texture.colors[i].r;
        values[1] = p_texture.colors[i].g;
        values[2] = p_texture.colors[i].b;
        color = cJSON_CreateFloatArray(values, 3);
        cJSON_AddItemToArray(colors, color);
    }
    cJSON_AddItemToObject(texture, "colors", colors);
    shade_pal_1 = cJSON_CreateIntArray(p_texture.shade_pal_1, 16);
    shade_pal_2 = cJSON_CreateIntArray(p_texture.shade_pal_2, 16);

    cJSON_AddItemToObject(texture, "shade_pal_1", shade_pal_1);
    cJSON_AddItemToObject(texture, "shade_pal_2", shade_pal_2);

    return texture;
}

pico_mesh_t obj_to_pico_mesh(obj_data_t obj, const char *name, int color)
{
    pico_mesh_t p_mesh = { 0 };
    color %= 16;

    /* vertices */
    float *position = obj.attrib.vertices;
    int vertex_count = obj.attrib.num_vertices;

    /* faces */
    float *texcoords = obj.attrib.texcoords;
    int face_count = obj.attrib.num_face_num_verts;
    int *face_vert_count = obj.attrib.face_num_verts; /* ind count per face */
    int index_count = obj.attrib.num_faces; /* number of the v0/vt0/vn0 entries */

    /* { int v_idx, vt_idx, vn_idx; } */
    tinyobj_vertex_index_t *obj_faces = obj.attrib.faces;

    /* initialize an empty mesh */
    p_mesh = pico_mesh_init(name, vertex_count, face_count, index_count);
    if (p_mesh.data == NULL) {
        return p_mesh;
    }

    /* TODO apply transforms here? */
    memcpy(p_mesh.vertices, position, sizeof(float)*3 * vertex_count);

    /* for each in p_mesh.faces */
    int index_offset = 0;
    for (int i = 0; i < face_count; ++i) {
        pico_face_t *face = p_mesh.faces + i;
        face->color = color;
        face->vertex_ids = face->vertex_ids + index_offset;
        face->uvs = face->uvs + index_offset*2;
        face->num_indices = face_vert_count[i];
        for (int j = 0; j < face->num_indices; ++j) {
            tinyobj_vertex_index_t obj_face = obj_faces[j + index_offset];
            face->vertex_ids[j] = obj_face.v_idx + 1; /* lua forced me to add one */
            /* uvs are inverted from obj format */
            /* FIXME uvs appeared inverted on a recent test.
             * am i screwing it up? y needs flip, x is fine? */
            face->uvs[j*2 + 0] = 0.0 + texcoords[obj_face.vt_idx*2 + 0];
            face->uvs[j*2 + 1] = 1.0 - texcoords[obj_face.vt_idx*2 + 1];
        }
        index_offset += face->num_indices;
    }

    return p_mesh;
}
