#define STBI_ONLY_PNG
#define STBI_MAX_DIMENSIONS 8192
#define STBI_NO_HDR
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "libimagequant.h"
#include "pico.h"

#include <stdio.h>

pico_texture_t pico_texture_from_png(const char *png_filename)
{
    /* create the pico texture */
    pico_texture_t texture = pico_texture_init();
    /* TODO alpha and background are initialized to zero. is that fine? */

    /* load png and resize it with stb libraries */
    int width, height;
    int no_channels;
    unsigned char *png_data, *small_png_data;
    png_data = stbi_load(png_filename, &width, &height, &no_channels, 4);
    if (png_data == NULL) {
        fprintf(stderr, "Failed to load image.\n");
        return texture;
    }
    small_png_data = stbir_resize_uint8_linear(png_data, width, height, 0, NULL, 128, 128, 0, STBIR_RGBA);
    if (small_png_data == NULL) {
        fprintf(stderr, "Failed to resize image.\n");
        return texture;
    }

    /* Specify image quanitzation parameters */
    liq_result *quantization_result;
    liq_attr *handle = liq_attr_create();
    liq_set_max_colors(handle, 15); /* i need to reserve one for mandatory alpha channel */
    liq_set_quality(handle, 0, 100);
    liq_image *input_image = liq_image_create_rgba(handle, small_png_data, 128, 128, 0);

    /* create the palette */
    if (liq_image_quantize(input_image, handle, &quantization_result) != LIQ_OK) {
        fprintf(stderr, "Quantization failed.\n", quantization_result);
        return texture;
    }

    /* we write the indices of our generated palette into texture.pixels */
    liq_write_remapped_image(quantization_result, input_image, texture.pixels, 128*128);
    const liq_palette *palette = liq_get_palette(quantization_result);
    
    /* map the indices to the ascii encoding */
    const char map[] = { '0','1','2','3','4','5','6','7',
                         '8','9','a','b','c','d','e','f' };
    /* wait, you did the map right, but each pixel needs to be bumped up an index. */
    for (int i = 0; i < 128*128; ++i) {
        texture.pixels[i] = map[texture.pixels[i] + 1];
    }

    /* store palette as colors
     * we skip 1 because this will be set to the alpha anyways.
     * if i don't pick alpha, we'll end up losing a random color. */

    /* off by one error. 
     * can we think? we have a total of 15 palette entries yeah?
     * i don't think we can get the 16th element */
    for (int i = 1; i < 16; ++i) {
        texture.colors[i].r = (float)(palette->entries[i - 1].r)/255.0f;
        texture.colors[i].g = (float)(palette->entries[i - 1].g)/255.0f;
        texture.colors[i].b = (float)(palette->entries[i - 1].b)/255.0f;
    }

    /* i'm not sure what to set the shade palettes.
     * you should probably just hit the robot button after this process */
    for (int i = 0; i < 16; ++i) {
        texture.shade_pal_1[i] = i;
        texture.shade_pal_2[i] = i;
    }

    /* cleanup */
    liq_result_destroy(quantization_result);
    liq_image_destroy(input_image);
    liq_attr_destroy(handle);

    stbi_image_free(png_data);
    free(small_png_data);

    return texture;
}

