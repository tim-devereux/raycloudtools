// Copyright (c) 2020
// Commonwealth Scientific and Industrial Research Organisation (CSIRO)
// ABN 41 687 119 230
//
// Author: Thomas Lowe
#ifndef RAYLIB_RAYFORESTGEN_H
#define RAYLIB_RAYFORESTGEN_H

#include "raylib/raylibconfig.h"
#include "rayforeststructure.h"
#include "raytreegen.h"
#include "rayutils.h"

namespace ray
{
struct ForestParams : TreeParams
{
  ForestParams() : field_width(20.0), max_tree_radius(0.2), dimension(2.0), adult_tree_density(0.01) {}
  double field_width;
  double max_tree_radius;
  double dimension;           // number of trees = radius^-d
  double adult_tree_density;  // number of trees per m^2
};

/// Forest ray cloud generation class. This generates a realistic random distribution of trees, each of which
/// has the attributes of a ray cloud. The random distribution can be seeded using @c srand()
class RAYLIB_EXPORT ForestGen 
{
public:
  /// makes the forest geometry
  void make(const ForestParams &params = ForestParams());
  /// make a forest from a file of base locations and radii
  bool makeFromFile(const std::string &filename, const TreeParams &params);
  /// converts the forest geometry into a set of rays, for a chosen @c ray_density
  void generateRays(double ray_density);

  /// returns just the leaf points of the forest
  std::vector<Eigen::Vector3d> getCanopy();

  /// returns the point cloud generated by @c generateRays
  std::vector<Eigen::Vector3d> getPointCloud();

  std::vector<TreeGen> &trees(){ return trees_; };

  /// Convert the forest generator to a base forest structure
  void toForestStructure(ForestStructure &forest)
  {
    forest.trees.resize(trees_.size());
    for (size_t i = 0; i<trees_.size(); i++)
    {
      trees_[i].toTreeStructure(forest.trees[i]);
    }
  }
private:
  std::vector<TreeGen> trees_;
};
}  // namespace ray

#endif  // RAYLIB_RAYFORESTGEN_H
