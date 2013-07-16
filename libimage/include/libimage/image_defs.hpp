#ifndef TRIK_LIBIMAGE_IMAGE_DEFS_HPP_
#define TRIK_LIBIMAGE_IMAGE_DEFS_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <stdint.h>
#include <libimage/stdcpp.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {

typedef int_fast16_t ImageDim;  // height, width, row index, col index
typedef int_fast32_t ImageSize; // image size, offset, line length

typedef float  ImageDimFactor; // height/width or dimension/dimension factor
typedef float  ImageDimFract;  // fractional part 0..1

typedef int_fast16_t ImageColorComponentIntegral; // single color component integral type


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_DEFS_HPP_
