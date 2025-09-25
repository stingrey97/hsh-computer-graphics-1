// Self
#include "TextureLoader.h"

// Standard libs
#include <stdio.h>
#include <assert.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "ImageLoader.h"

GLuint loadTexture2D(const char *path, int useRGB)
{
    int textureWidth = 0;
    int textureHeight = 0;
    int bitsPerPixel = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *textureBuffer = stbi_load(path, &textureWidth, &textureHeight, &bitsPerPixel, 4);
    if (!textureBuffer)
    {
        printf("stbi_load FAILED for '%s' : %s\n", path, stbi_failure_reason());
        return 0;
    }
    printf("Loaded '%s'  %dx%d  channels=%d\n", path, textureWidth, textureHeight, bitsPerPixel);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLint internalFmt;
    if (useRGB)
    {
        internalFmt = GL_SRGB8_ALPHA8;
    }
    else
    {
        internalFmt = GL_RGBA8;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (textureBuffer)
    {
        stbi_image_free(textureBuffer);
    }

    return textureID;
}

GLuint loadCubemap(const char *faces[6])
{
    assert(faces != NULL);
    
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

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

    glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpackAlign);

    if (!ok)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        if (tex)
            glDeleteTextures(1, &tex);
        return 0;
    }

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
