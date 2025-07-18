#ifndef LU_TTF_H
#define LU_TTF_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "lu.h"
#include "window.h"

Vertex *lu_extract_glyph_from_font(const char *font, u16 code_point, size_t *size);

#endif /* LU_TTF_H */
