#ifndef LU_TTF_H
#define LU_TTF_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "lu.h"
#include "string.h"
#include "array.h"
#include "window.h"

ArrayVec2 lu_extract_glyph_from_font(Arena *, String, u16);

#endif /* LU_TTF_H */
