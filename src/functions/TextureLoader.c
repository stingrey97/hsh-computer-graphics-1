// Self
#include "TextureLoader.h"

// Standard libs
#include <stdio.h>
#include <assert.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "ImageLoader.h"

GLuint loadTexture2D(void)
{
    // TODO: implement 2D texture loader
    return 0;
}

GLuint loadCubemap(const char *faces[6])
{
    assert(faces != NULL);
    
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    // Save and set unpack alignment to 1
    GLint prevUnpackAlign = 0;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpackAlign);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    stbi_set_flip_vertically_on_load(0);

    int baseW = -1, baseH = -1;
    int ok = 1;

    for (int i = 0; i < 6; ++i)
    {
        int w, h, n;
        unsigned char *data = stbi_load(faces[i], &w, &h, &n, 0);
        if (!data)
        {
            printf("Cubemap: failed to load %s\n", faces[i]);
            ok = 0;
            break;
        }

        if (baseW < 0)
        {
            baseW = w;
            baseH = h;
        }
        else if (w != baseW || h != baseH)
        {
            printf("Cubemap: mismatched face dimensions (%s)\n", faces[i]);
            stbi_image_free(data);
            ok = 0;
            break;
        }

        GLenum srcFmt = (n == 1) ? GL_RED : (n == 3) ? GL_RGB
                                                     : GL_RGBA;
        GLint intFmt = (n == 1) ? GL_R8 : (n == 3) ? GL_RGB8
                                                   : GL_RGBA8;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     intFmt, w, h, 0, srcFmt, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    // Restore unpack alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);

    if (!ok)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        if (tex)
            glDeleteTextures(1, &tex);
        return 0;
    }

    // Filtering and wrapping
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    return tex;
}
