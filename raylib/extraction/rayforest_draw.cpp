// Copyright (c) 2020
// Commonwealth Scientific and Industrial Research Organisation (CSIRO)
// ABN 41 687 119 230
//
// Author: Thomas Lowe
#include "rayforest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "raylib/imagewrite.h"

namespace ray
{
struct Col
{
  Col(){}
  Col(uint8_t shade) : r(shade), g(shade), b(shade), a(255) {}
  Col(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : r(red), g(green), b(blue), a(alpha) {}
  void operator +=(const Col &col)
  {
    r = (uint8_t)std::min((int)r + (int)col.r, 255);
    g = (uint8_t)std::min((int)g + (int)col.g, 255);
    b = (uint8_t)std::min((int)b + (int)col.b, 255);
    a = (uint8_t)std::min((int)a + (int)col.a, 255);
  }
  uint8_t r, g, b, a;
};

void Forest::drawHeightField(const std::string &filename, const Eigen::ArrayXXd &heightfield)
{
  if (!verbose)
    return;
  double max_height = -1e10; 
  double min_height = 1e10;
  for (int i = 0; i<heightfield.rows(); i++)
  {
    for (int j = 0; j<heightfield.cols(); j++)
    {
      if (heightfield(i,j) > -10000.0)
        min_height = std::min(min_height, heightfield(i,j));
      if (heightfield(i,j) < 10000.0)
        max_height = std::max(max_height, heightfield(i,j));
    }
  }

  Field2D<Col> pixels((int)heightfield.rows(), (int)heightfield.cols());
  for (int x = 0; x < pixels.dims[0]; x++)
    for (int y = 0; y < pixels.dims[1]; y++)
      pixels(x, y) = Col((uint8_t)(255.0 * (heightfield(x, y) - min_height)/(max_height - min_height)));
  stbi_write_png(filename.c_str(), pixels.dims[0], pixels.dims[1], 4, (void *)&pixels.data[0], 4 * pixels.dims[0]);
}

void Grid2D::draw(const std::string &filename)
{
  Field2D<Col> pixels(dims_[0], dims_[1]);
  for (int x = 0; x < pixels.dims[0]; x++)
    for (int y = 0; y < pixels.dims[1]; y++)
    {
      double shade = pixel(Eigen::Vector3i(x, y, 0)).density();
      pixels(x, y) = Col((uint8_t)(255.0 * shade));
    }
  stbi_write_png(filename.c_str(), pixels.dims[0], pixels.dims[1], 4, (void *)&pixels.data[0], 4 * pixels.dims[0]);
}

void Forest::drawTrees(const std::string &filename, const std::vector<Forest::Result> &results, int width, int height)
{
  double max_height = 0.0;
  double min_height = 1e10;
  for (auto &res: results)
  {
    max_height = std::max(max_height, res.base[2] + res.height);
    min_height = std::min(min_height, res.base[2]);
  }

  // I should probably draw the result
  if (!verbose)
    return;
  Field2D<Col> pixels(width, height);
  for (auto &c: pixels.data)
    c = Col(0); 
  for (auto &result: results)
  {
    Eigen::Vector3d pos = (result.base - min_bounds_) / voxel_width_;
    double curvature = -0.05;
    double crown_radius = result.radius * 5.0;
    double radius_pixels = crown_radius / voxel_width_;
    for (int x = (int)(pos[0] - radius_pixels); x<= (int)(pos[0]+radius_pixels); x++)
    {
      for (int y = (int)(pos[1] - radius_pixels); y<= (int)(pos[1]+radius_pixels); y++)
      {
        if (x < 0 || x >= width || y<0 || y>=height)
          continue;
        double X = ((double)x - pos[0]) * voxel_width_;
        double Y = ((double)y - pos[1]) * voxel_width_;
        double mag2 = (double)(X*X + Y*Y);
        if (mag2 <= crown_radius*crown_radius)
        {
          double height = result.base[2] + result.height + mag2 * curvature;
          double shade = std::max(0.0, std::min((height - min_height)/(max_height - min_height), 1.0)); // clamp because curvature can conceivably negative sometimes
          Col col(uint8_t(255.0*shade));
          if (pixels(x, y).r < col.r)
            pixels(x, y) = col;
        }
      }
    }
  }    
  stbi_write_png(filename.c_str(), pixels.dims[0], pixels.dims[1], 4, (void *)&pixels.data[0], 4 * pixels.dims[0]);
}
} // namespace ray