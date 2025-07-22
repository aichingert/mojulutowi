#ifndef LU_TTF_H
#define LU_TTF_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "lu.h"
#include "window.h"

Vertex *lu_extract_glyph_from_font(Arena *, String, u16, size_t *);

#endif /* LU_TTF_H */
