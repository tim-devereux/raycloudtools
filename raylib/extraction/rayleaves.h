#ifndef RAYLIB_RAYLEAVES_H
#define RAYLIB_RAYLEAVES_H

#include "raylib/raylibconfig.h"
#include "raylib/rayutils.h"

namespace ray
{
bool generateLeaves(const std::string &cloud_stub, const std::string &trees_file, const std::string &leaf_file, double leaf_area, double droop, bool stalks);
}  // namespace ray
#endif  // RAYLIB_RAYLEAVES_H
