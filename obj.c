#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "obj.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

static inline void print_error(int error)
{
    switch(error) {
        case TINYOBJ_ERROR_EMPTY:
            fprintf(stderr,"Error loading OBJ: (-1) File empty.\n");
            break;
        case TINYOBJ_ERROR_INVALID_PARAMETER:
            fprintf(stderr,"Error loading OBJ: (-2) Invalid parameter.\n");
            break;
        case TINYOBJ_ERROR_FILE_OPERATION:
            fprintf(stderr,"Error loading OBJ: (-3) File operation.\n");
            break;
        default:
            fprintf(stderr,"Invalid error\n.");
    }
}

/* function used from example in tinyobjloader in C. https://github.com/syoyo/tinyobjloader-c */
static void load_file(void* ctx, const char* filename, const int is_mtl,
               const char* obj_filename, char** data, size_t* len)
{
    /* stupid compiler flags */
    (void)ctx;
    (void)is_mtl;
    (void)obj_filename;

    if(!filename) {
        fprintf(stderr, "ERROR: No file name provided.\n");
        (*data) = NULL;
        (*len) = 0;
        return;
    }

    struct stat sb;
    char *p;
    int fd;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        return;
    }

    if (!S_ISREG(sb.st_mode)) {
        fprintf(stderr, "%s is not a file\n", filename);
        return;
    }

    p = (char*)mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if (p == MAP_FAILED) {
        perror("mmap");
        return;
    }

    if (close(fd) == -1) {
        perror("close");
        return;
    }

    (*data) = p;
    (*len) = sb.st_size;
}


obj_data_t obj_load_from_file(const char *filename)
{
    obj_data_t obj = { 0 };

    unsigned int flags = 0;
    int ret =
        tinyobj_parse_obj(&obj.attrib, &obj.shapes, &obj.num_shapes, &obj.materials,
                          &obj.num_materials, filename, load_file, NULL, flags);
    if (ret != TINYOBJ_SUCCESS) {
        print_error(ret);
        return (obj_data_t){0};
    }
    return obj;
}

/* printf debugging in one function
 * this is what happens when i can't print generics in C */
void obj_print_attrib_statistic(tinyobj_attrib_t attrib)
{
    /* read attrib */
    printf("num_vertices = %u\n", attrib.num_vertices);
    printf("num_normals = %u\n", attrib.num_normals);
    printf("num_texcoords = %u\n", attrib.num_texcoords);
    printf("num_faces = %u\n", attrib.num_faces);
    printf("num_face_num_verts = %u\n", attrib.num_face_num_verts);
    printf("\n");

    /* the rest of this just peeps the first member of each element */
    if (attrib.num_vertices != 0) {
        printf("*vertices = { ");
        for (int i = 0; i < 3; ++i) {
            printf("%g, ", attrib.vertices[i]);
        }
        printf("... }\n");
    }

    if (attrib.num_texcoords != 0) {
        printf("*texcoords = { ");
        for (int i = 0; i < 2; ++i) {
            printf("%g, ", attrib.texcoords[i]);
        }
        printf("... }\n");
    }

    if (attrib.num_normals != 0) {
        printf("*normals = { ");
        for (int i = 0; i < 3; ++i) {
            printf("%g, ", attrib.normals[i]);
        }
        printf("... }\n");
    }

    if (attrib.num_faces != 0) {
        tinyobj_vertex_index_t f = attrib.faces[0];
        printf("*faces = { ");
        printf("{ v_idx = %d, vt_idx = %d, vn_idx = %d }", f.v_idx, f.vt_idx, f.vn_idx);
        printf(", ... }\n");

        printf("*face_num_verts = { %d, ... }\n", attrib.face_num_verts[0]);
        /* random assumption that materials are based on faces
         * i will assume correct unless segfault */
        printf("*material_ids = { %d, ... }\n", attrib.material_ids[0]);
    }
}

/* info is good, but it seems a bit printf-debugy */
char *obj_texture_filename(obj_data_t obj)
{
    if (!obj.num_materials) {
        printf("INFO: No materials found in OBJ.\n");
        return NULL;
    }

    if (obj.num_materials > 1) {
        printf("INFO: More than one material present.\n");
    }
    printf("OBJ material found: '%s'\n", obj.materials[0].name);

    char *texname = NULL;
    if (obj.materials[0].diffuse_texname) { 
        texname = obj.materials[0].diffuse_texname;
        printf("OBJ Texture found: %s\n", texname);
    } else { 
        printf("INFO: No diffuse texture present in material.\n");
    }

    return texname;
}

void obj_destroy(obj_data_t obj)
{
    tinyobj_attrib_free(&obj.attrib);
    tinyobj_shapes_free(obj.shapes, obj.num_shapes);
    tinyobj_materials_free(obj.materials, obj.num_materials);
}
    

