#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "domain.h"

namespace domain {
/**
 * @brief The size of the compressed data (in bytes)
 * 
 * When calling @ref compress() with the @p slice, 
 * this is how much space will be used in @p buff
 * 
 * @param[in] slice 
 * @return std::size_t 
 */
std::size_t getCompressedSize(const fields::Helper<svec::SVector> &slice);


/**
 * @brief Compress the data in the @p slice
 * 
 * @param[out] buff The buffer to fill with the compressed data
 * @param[in] slice The data to compress
 */
void compress(void* const buff, const fields::Helper<svec::SVector> &slice);

/**
 * @brief Decompress the data in the @p buff
 * 
 * @param[in] buff The buffer with the compressed data
 * @param[out] slice The slice to fill
 */
void decompress(const void* const buff, const fields::Helper<svec::SVector> &slice);

}

#endif