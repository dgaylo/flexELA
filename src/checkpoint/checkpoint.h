#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

#include "../domain/domain.h"

namespace checkpoint {
constexpr std::uint8_t CURRENT_CHECKPOINT_VERSION_NUMBER = 1;

void create(const char* filename, const domain::Domain& dom);

void load(const char* filename, const domain::Domain& dom);
} // namespace checkpoint

#endif