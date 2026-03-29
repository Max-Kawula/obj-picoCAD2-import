//#define STR_ARENA_IMPLEMENTAION /* something very broken in this file? */
//#include "str_arena.h"
#include "cJSON.h"
#include "obj.h"
#include "pico.h"
#include <stdio.h>
#include <string.h>

/* REFERENCE FOR MAX
 * broad strokes of this operation:
 *  - import obj and pass the obj_type to main.c
 *  - serialize the obj_type to the pico_type
 *  - marshal pico_type to a cJSON object.
 *  - either generate a new save and append or append to existing save
 */

/* seems i already forgot the structure. it all starts with a graph child.
 * graph.children[0].mesh */

/* idiot max is like "why should i have to put an arbitrary amount of bytes
 * into a struct for the names of things? can we make an arena?
 *
 * okay you idiot. yes you can. */
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
            face->uvs[j*2 + 0] = 1.0 - texcoords[obj_face.vt_idx*2 + 0];
            face->uvs[j*2 + 1] = 1.0 - texcoords[obj_face.vt_idx*2 + 1];
        }
        index_offset += face->num_indices;
    }

    return p_mesh;
}

/* need to write down our pre parsing strategy
 * im going to load in a scene with basically an empty folder
 * named obj.
 *
 * obj_folder = graph.children[0];
 *
 * we will be placing the object into the child
 *
 * new_object = graph.children[0].children[0]
 *
 * lastly, make sure to attach the mesh to the object
 *
 * new_mesh = graph.children[0].children[0].mesh
*/

cJSON *marshal_pico_mesh(pico_mesh_t *p_mesh)
{
    cJSON *mesh = NULL;
    cJSON *vertices = NULL;
    cJSON *faces = NULL;

    mesh = cJSON_CreateObject();
    cJSON_AddStringToObject(mesh, "name", p_mesh->name);

    vertices = cJSON_CreateFloatArray(p_mesh->vertices, p_mesh->num_vertices*3);
    cJSON_AddItemToObject(mesh, "vertices", vertices);

    faces = cJSON_CreateArray();

    for (int i = 0; i < p_mesh->num_faces; ++i) {
        pico_face_t *p_face = p_mesh->faces + i;
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


int main(int argc, char **argv)
{
    const char *obj_file = NULL;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s FILE.obj\n", argv[0]);
        return 1;
    }

    obj_file = argv[1];
    obj_data_t obj_data = load_obj(obj_file);

    if (obj_data.attrib.num_vertices == 0) {
        fprintf(stderr, "ERROR: Obj did not load\n");
        return 1;
    }

    cJSON *scene = cJSON_Parse(pico_empty_scene);
    if (scene == NULL) {
        fprintf(stderr, "Failed to parse scene JSON.\n");
        return 1;
    }
    cJSON *graph = cJSON_GetObjectItem(scene, "graph");
    cJSON *graph_children = cJSON_GetObjectItem(graph, "children");
    /* FIXME we should look for the item with "name": "obj", not the first one */
    cJSON *obj_folder = cJSON_GetArrayItem(graph_children, 0);
    cJSON *obj_folder_children = cJSON_GetObjectItem(obj_folder, "children");

    cJSON *base_object = cJSON_Parse(pico_empty_object);
    if (base_object == NULL) {
        fprintf(stderr, "Failed to parse object JSON.\n");
        return 1;
    }

    pico_mesh_t mesh = obj_to_pico_mesh(obj_data, "cat", 5);
    cJSON *json_mesh = marshal_pico_mesh(&mesh);
    cJSON_AddItemToObject(base_object, "mesh", json_mesh);
    cJSON_AddItemToArray(obj_folder_children, base_object);

    char *output_string = cJSON_Print(scene);
    printf(output_string);

    pico_mesh_destroy(&mesh);
    return 0;
}

/********************/
/* DEPRECATION ZONE */
/********************/
cJSON *create_pico_object(const char* object_name, obj_data_t obj, int color)
{
    float *position = obj.attrib.vertices;
    int vertex_count = obj.attrib.num_vertices;

    float *texcoords = obj.attrib.texcoords;
    //int texcoord_count = obj.attrib.num_texcoords; /* unused */
    int face_count = obj.attrib.num_face_num_verts;
    int *face_vert_count = obj.attrib.face_num_verts; 

    /* typedef struct { int v_idx, vt_idx, vn_idx; } tinyobj_vertex_index_t; */
    tinyobj_vertex_index_t *obj_faces = obj.attrib.faces;

    cJSON *mesh = NULL;
    cJSON *vertices = NULL;
    cJSON *faces = NULL;

    cJSON *pico_object = cJSON_CreateObject();
    if (pico_object == NULL) {
        return NULL;
    }

    cJSON_AddStringToObject(pico_object, "name", object_name);
    cJSON_AddBoolToObject(pico_object, "visible", 1);
    cJSON_AddBoolToObject(pico_object, "locked", 0);
    cJSON_AddBoolToObject(pico_object, "open", 0);

    /* specified in pico.c to save me some effort */
    cJSON_AddRawToObject(pico_object, "transform", pico_default_transform);
    cJSON_AddRawToObject(pico_object, "motions", pico_default_motions);
    cJSON_AddArrayToObject(pico_object, "children"); /* purposefully empty */

    /* now we generate a mesh */
    mesh = cJSON_CreateObject();

    cJSON_AddStringToObject(mesh, "name", object_name); /* is the same as the object, yes. */

    vertices = cJSON_CreateFloatArray(position, vertex_count*3);
    cJSON_AddItemToObject(mesh, "vertices", vertices);

    faces = cJSON_CreateArray();
    int offset = 0;

    for (int i = 0; i < face_count; ++i) {
        cJSON *face = cJSON_CreateObject();
        cJSON_AddNumberToObject(face, "color", color);
        cJSON *vertex_ids = cJSON_CreateArray();
        cJSON *uvs = cJSON_CreateArray();

        for (int j = 0; j < face_vert_count[i]; ++j) {
            // add indices
            tinyobj_vertex_index_t obj_face = obj_faces[j + offset];
            cJSON *index = cJSON_CreateNumber(obj_face.v_idx + 1); /* FUCKING LUA PROGRAMMERS! */
            cJSON_AddItemToArray(vertex_ids, index);
            // add uvs, dereferenced!
            cJSON *uv0 = cJSON_CreateNumber(1.0f - texcoords[obj_face.vt_idx*2 + 0]);
            cJSON *uv1 = cJSON_CreateNumber(1.0f - texcoords[obj_face.vt_idx*2 + 1]);
            cJSON_AddItemToArray(uvs, uv0);
            cJSON_AddItemToArray(uvs, uv1);
        }

        offset = offset + face_vert_count[i];
        cJSON_AddItemToObject(face, "vertex_ids", vertex_ids);
        cJSON_AddItemToObject(face, "uvs", uvs);
        cJSON_AddItemToArray(faces, face);
    }

    cJSON_AddItemToObject(mesh, "faces", faces);


    /* add mesh and you are set */
    cJSON_AddItemToObject(pico_object, "mesh", mesh);
    return pico_object;
}
