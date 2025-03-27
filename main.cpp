#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include "color.h"
#include "ppm.h"

int main(int argc, char *argv[]) {
  auto start = std::chrono::high_resolution_clock::now();
  ppm test = ppm(argv[1]);
  test.pixelsort(0.05,0.9);
  test.write();
  std::clog << std::endl;
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
  std::clog << "Total: " << time << "ms" << std::endl;
}
