// Copyright (c) 2020
// Commonwealth Scientific and Industrial Research Organisation (CSIRO)
// ABN 41 687 119 230
//
// Author: Thomas Lowe
#include "raylib/raycloud.h"
#include "raylib/raymesh.h"
#include "raylib/rayply.h"
#include "raylib/raydebugdraw.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
using namespace Eigen;
using namespace ray;

void usage(int exit_code = 0)
{
  cout << "Combines multiple ray clouds. Clouds are not moved but rays are omitted in the combined cloud according to "
          "the merge type specified."
       << endl;
  cout << "Outputs the combined cloud and the residual cloud of differences." << endl;
  cout << "usage:" << endl;
  cout << "raycombine min raycloud1 raycloud2 ... raycloudN 20 rays - combines into one cloud with minimal objects at "
          "differences"
       << endl;
  cout << "                                                           20 is the number of pass through rays to define "
          "a difference"
       << endl;
  cout
    << "           max    - maximal objects included. This is a form of volume intersection (rather than min: union)."
    << endl;
  cout << "           oldest - keeps the oldest geometry when there is a difference in later ray clouds." << endl;
  cout << "           newest - uses the newest geometry when there is a difference in newer ray clouds." << endl;
  cout << "           all    - combines as a simple concatenation, with all rays remaining (don't include 'xx rays')."
       << endl;
  exit(exit_code);
}

// Decimates the ray cloud, spatially or in time
int main(int argc, char *argv[])
{
  DebugDraw::init(argc, argv, "raycombine");
  if (argc < 4)
    usage();
  string merge_type = argv[1];
  bool concatenate = merge_type == "all";
  double num_rays = 0;
  if (concatenate)
  {
    if (string(argv[argc - 1]) == "rays")
      usage();
  }
  else
  {
    if (argc < 6 || string(argv[argc - 1]) != "rays")
      usage();
    num_rays = stod(argv[argc - 2]);
    argc -= 2;
  }

  vector<string> files;
  for (int i = 2; i < argc; i++)
  {
    files.push_back(string(argv[i]));
    ifstream f(files.back().c_str());
    if (!f.good())
    {
      cout << "could not open file: " << files.back() << endl;
      usage();
    }
  }

  string file_stub = files[0];
  if (file_stub.substr(file_stub.length() - 4) == ".ply")
    file_stub = file_stub.substr(0, file_stub.length() - 4);

  vector<Cloud> clouds(files.size());
  for (int i = 0; i < (int)files.size(); i++) clouds[i].load(files[i]);

  Cloud combined;
  if (concatenate)
  {
    for (auto &cloud : clouds)
    {
      combined.starts.insert(combined.starts.end(), cloud.starts.begin(), cloud.starts.end());
      combined.ends.insert(combined.ends.end(), cloud.ends.begin(), cloud.ends.end());
      combined.times.insert(combined.times.end(), cloud.times.begin(), cloud.times.end());
      combined.colours.insert(combined.colours.end(), cloud.colours.begin(), cloud.colours.end());
    }
  }
  else
  {
    Cloud differences;
    combined.combine(clouds, differences, merge_type, num_rays);
    differences.save(file_stub + "_differences.ply");
  }
  combined.save(file_stub + "_combined.ply");
  return true;
}