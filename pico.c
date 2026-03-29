#include "pico.h"
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

const char *pico_empty_object = {
    "{\n"
    //  "name": "string"  /* add these two here */
    //  "mesh": {}
    "  \"children\":[],\n"
    "  \"motions\":{\n"
    "    \"tracks\":[[],[],[],[]]\n"
    "  },\n"
    "  \"open\":false,\n"
    "  \"transform\":{\n"
    "    \"scale\":{\n"
    "      \"y\":1,\n"
    "      \"z\":1,\n"
    "      \"x\":1\n"
    "    },\n"
    "    \"rot\":{\n"
    "      \"y\":0,\n"
    "      \"z\":0,\n"
    "      \"x\":0\n"
    "    },\n"
    "    \"pos\":{\n"
    "      \"y\":0,\n"
    "      \"z\":0,\n"
    "      \"x\":0\n"
    "    }\n"
    "  },\n"
    "  \"visible\":true,\n"
    "  \"locked\":false\n"
    "}"
};

/* https://tomeko.net/online_tools/cpp_text_escape.php */
/* is missing:
 * graph.children["obj"].children[] */
const char *pico_empty_scene = {
    "{\n"
    "  \"metadata\":{\n"
    "    \"shading_mode\":0,\n"
    "    \"face_mode\":0,\n"
    "    \"export_settings\":{\n"
    "      \"speed\":5,\n"
    "      \"scanline_color\":0,\n"
    "      \"watermark\":\"#picoCAD2\",\n"
    "      \"watermark2\":\"\",\n"
    "      \"size\":128,\n"
    "      \"anim\":\"spin\",\n"
    "      \"dir\":-1,\n"
    "      \"scanlines\":false,\n"
    "      \"outline_color\":0,\n"
    "      \"outline_size\":0,\n"
    "      \"scale\":3\n"
    "    },\n"
    "    \"camera\":{\n"
    "      \"bookmark\":{\n"
    "        \"pos\":{\n"
    "          \"y\":0,\n"
    "          \"z\":0,\n"
    "          \"x\":0\n"
    "        },\n"
    "        \"target\":{\n"
    "          \"y\":0,\n"
    "          \"z\":0,\n"
    "          \"x\":0\n"
    "        },\n"
    "        \"omega\":0.3,\n"
    "        \"theta\":0.5,\n"
    "        \"distance_to_target\":10\n"
    "      },\n"
    "      \"pos\":{\n"
    "        \"y\":4.794255386042,\n"
    "        \"z\":2.5934338005223,\n"
    "        \"x\":8.383866435942\n"
    "      },\n"
    "      \"target\":{\n"
    "        \"y\":0,\n"
    "        \"z\":0,\n"
    "        \"x\":0\n"
    "      },\n"
    "      \"omega\":0.3,\n"
    "      \"theta\":0.5,\n"
    "      \"distance_to_target\":10\n"
    "    },\n"
    "    \"version\":\"2.0\",\n"
    "    \"motion_duration\":8\n"
    "  },\n"
    "  \"texture\":{\n"
    "    \"shade_pal_2\":[0,0,0,0,1,1,1,1,3,2,2,3,3,4,4,6],\n"
    "    \"transparent_color\":0,\n"
    "    \"pixels\":\"ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ffffffffcccccccc9999999977777777eeeeeeeeaaaaaaaabbbbbbbb555555558888888866666666dddddddd99999999aaaaaaaa444444446666666633333333ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666ccccccccffffffff7777777799999999aaaaaaaaeeeeeeee55555555bbbbbbbb666666668888888899999999dddddddd44444444aaaaaaaa3333333366666666cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfccccccccccccccccccccccccccccccfffffccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfffffcccccccccccccccccccccccccccfffffffccccccffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffccccccccccccccccfffffffcccccccccccccccccccccccccfff333fffccccfff33ffffffffffffffffffffffffffffffffffffffffffffffffff3ffff3fffffffcccccccccccccccfcf9fcfcccccccccccccccccccccccccfff333fffccccff3fff33fff33ff33ff3fffff3ffff33f3fff3ff3f3ff33ff3fffff33ff333f33fffcccccfccfcfccccfcf9fcfcccccccccccccccccccccccccf3333333fccccfff3ff3f3f3f3f3fff333fff333fff3f3f3f3f3f3f3f3fff333ffff3f3ff3ff3f3ffccccfcfcffcccccfffffffcccccccccccccccccccccccccff33333ffccccffff3f3f3f3f3f3fff3ffffff3ffff3f3f3f3f3f3f3fff3f3ffffff3f3ff3ff3f3ffccccfcfcfccccccfffffffcccccccccccccccccccccccccfff333fffccccff33ff33fff33ff33ff3ffffffffff3f3f3ff3fff33f33fff3fffff33ffff3f3f3ffcccccfccfccccccfffffffccccccccccccccccccccccccccfff3fffccccccfffff3ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffccccccccccccccccfffffffcccccccccccccccccccccccccccfffffccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfffffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfffffffffffffffffffccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccff3f3fffff3f3fffff3ffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccff3f3ff3ff3f3ff3ff3ffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccff333f333f3f3f3f3f3ffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccff3f3f3fff3f3f3f3ffffccccccccffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffcccccccccccccccccccccccccff3f3ff3ff3f3ff3ff3ffcccccccff3fff3fffffffffffffffffffffff3fffffffffff3ffff3ffffffffffffffffffffffffffffcccccccccccccccccccccccccfffffffffffffffffffccccccccff33f33ff3ff3f3ff33ff3ffff3f3f33fff3fff3ff3fff333ff3ffff33ff3fff3ff33f3fffffcccccccccccccccccccccccccccffcccccccccccccccccccccccff3f3f3f3f3f3f3f3fff333fff3f3f3f3f333f333f3ffff3ff3f3ffff3f3f3f3f3f3f3f3ffffcccccccccccccccccffcffcccccffcccccccccccccccccccccccff3fff3f3f3f3f3fff3f3fffff333f3f3f3fff3fff3ffff3ff3f3fff3ff3f3f3f3f3f3f3ffffcccccccccccccccccffcffccccffccccccccccccccccccccccccff3fff3ff3fff33f33fff3ffff3f3f3f3ff3fff3ff3fffff3ff3ffff33ff3fff3ff3f3f3f3ffcccccccccccccccccffcffcccccccccccccccccccccccccccccccffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffccccccccccccccccccfffffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffccccccccccccccccccccccccccf1f999ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffccccccffcffcccccccccccccccfff999cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffccccffcffccccccccccccccccfffffccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfccfcccccccccccccccccfccfccccccccccccccccccccffffffffffffffffffffffffffffffffccccccccccccccccccccccccccccccccccccccccccccccccccfffffccccccccccccccccccccccccccccccccccccccccff3f3ffffffffffffffff3fffffffff3ffcccccccccccccccccccccccccccccccccccccccccccccccc999f1fccccccccccccccccccccccccccccccccccccccccff3f3ff33f3f3ff3ffff3ff3f3f33ff3ffcccccccccccccccccccccccccccccccccccccccccccccccc999fffccccccccccccccccccccccccccccccccccccccccff333f3f3f3f3f333fff33f3f3f3f3f3ffcccccccccccccccccccccccccccccccccccccccccccccccccfffffccccccccccccccccccccccccccccccccccccccccff3f3f3f3f3f3f3fffff3ff3f3f3f3ffffccccccccccccccccccccccccccccccccccccccccccccccccccfccfccccccccccccccccccccccccccccccccccccccccff3f3ff33ff3fff3ffff3fff33f3f3f3ffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffffffffffffffffffffffffffffffffccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffcccffcccffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccffffcffffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfcfffffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfcf1f999ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfff999ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfffffcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccfccfccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc\",\n"
    "    \"colors\":[[0,0,0],[0.11372549019608,0.16862745098039,0.32549019607843],[0.49411764705882,0.14509803921569,0.32549019607843],[0.37254901960784,0.34117647058824,0.30980392156863],[0.67058823529412,0.32156862745098,0.21176470588235],[0,0.52941176470588,0.31764705882353],[0.51372549019608,0.46274509803922,0.61176470588235],[1,0,0.30196078431373],[0.16078431372549,0.67843137254902,1],[1,0.46666666666667,0.65882352941176],[1,0.63921568627451,0],[0,0.89411764705882,0.21176470588235],[0.76078431372549,0.76470588235294,0.78039215686275],[1,0.8,0.66666666666667],[1,0.92549019607843,0.15294117647059],[1,0.94509803921569,0.90980392156863]],\n"
    "    \"background_color\":1,\n"
    "    \"shade_pal_1\":[0,0,1,1,2,3,3,2,6,7,4,5,6,10,10,12]\n"
    "  },\n"
    "  \"graph\":{\n"
    "    \"children\":[{\n"
    "        \"children\":[],\n" /* insert object into this array */
    "        \"name\":\"obj\",\n"
    "        \"motions\":{\n"
    "          \"tracks\":[[],[],[],[]]\n"
    "        },\n"
    "        \"folder\":true,\n"
    "        \"open\":false,\n"
    "        \"transform\":{\n"
    "          \"scale\":{\n"
    "            \"y\":1,\n"
    "            \"z\":1,\n"
    "            \"x\":1\n"
    "          },\n"
    "          \"rot\":{\n"
    "            \"y\":0,\n"
    "            \"z\":0,\n"
    "            \"x\":0\n"
    "          },\n"
    "          \"pos\":{\n"
    "            \"y\":0,\n"
    "            \"z\":0,\n"
    "            \"x\":0\n"
    "          }\n"
    "        },\n"
    "        \"visible\":true,\n"
    "        \"locked\":false\n"
    "      }],\n"
    "    \"name\":\"root\",\n"
    "    \"motions\":{\n"
    "      \"tracks\":[[],[],[],[]]\n"
    "    },\n"
    "    \"open\":false,\n"
    "    \"transform\":{\n"
    "      \"scale\":{\n"
    "        \"y\":1,\n"
    "        \"z\":1,\n"
    "        \"x\":1\n"
    "      },\n"
    "      \"rot\":{\n"
    "        \"y\":0,\n"
    "        \"z\":0,\n"
    "        \"x\":0\n"
    "      },\n"
    "      \"pos\":{\n"
    "        \"y\":0,\n"
    "        \"z\":0,\n"
    "        \"x\":0\n"
    "      }\n"
    "    },\n"
    "    \"visible\":true,\n"
    "    \"locked\":false\n"
    "  }\n"
    "}"
};


const char *pico_default_transform = {
    "{\n"
    "  \"pos\": {\n"
    "    \"x\": 0,\n"
    "    \"y\": 0,\n"
    "    \"z\": 0\n"
    "  },\n"
    "  \"rot\": {\n"
    "    \"x\": 0,\n"
    "    \"y\": 0,\n"
    "    \"z\": 0\n"
    "  },\n"
    "  \"scale\": {\n"
    "    \"x\": 1,\n"
    "    \"y\": 1,\n"
    "    \"z\": 1\n"
    "  }\n"
    "}"
};

const char *pico_default_motions = {
    "{\n"
    "  \"tracks\": [\n"
    "    [],\n"
    "    [],\n"
    "    [],\n"
    "    []\n"
    "  ]\n"
    "}"
};
