#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "obj.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

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


obj_data_t load_obj(const char *filename)
{
    obj_data_t obj = { 0 };
    /* materials are discarded but still required.
     * in the future we will try to pull the image from materials. */
    tinyobj_material_t *materials = NULL;
    size_t num_materials;

    unsigned int flags = 0;
    int ret =
        tinyobj_parse_obj(&obj.attrib, &obj.shapes, &obj.num_shapes, &materials,
                          &num_materials, filename, load_file, NULL, flags);
    if (ret != TINYOBJ_SUCCESS) {
        return (obj_data_t){0};
    }
    return obj;
}
