#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
    int width, height, bpp;

    uint8_t* rgb_image = stbi_load("5.jpg", &width, &height, &bpp, 3);

	int i;
	for(i = 0 ; i < 1000; i++)
		printf("%d ", rgb_image[i]);

    stbi_image_free(rgb_image);

    return 0;
}
