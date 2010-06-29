#ifndef STB_IMAGE_H
#define STB_IMAGE_H

/**
 * This is a small header to avoid having to include the
 * stb_image.c file that results in c++ compiler warnings.
 */
typedef unsigned char stbi_uc;

stbi_uc*
stbi_load_from_memory(stbi_uc const *buffer,
                      int len, int *x, int *y,
                      int *comp, int req_comp);

stbi_uc*
stbi_load_from_file(FILE *f,
                    int *x, int *y,
                    int *comp, int req_comp);


#endif /* STB_IMAGE_H */
