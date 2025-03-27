#ifndef PPM_H
#define PPM_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <sstream>
#include "color.h"
class ppm {
  public:
    std::string filename;
    std::string ppmtype; // P3 or P6
    int height;
    int width;
    int colorspace;
    std::vector<color> pixels;

    ppm(const std::string& filename) {
    int fd = open(filename.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Failed to open file");
        }
        
        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            close(fd);
            throw std::runtime_error("Failed to get file size");
        }
        
        size_t filesize = sb.st_size;
        char* data = static_cast<char*>(mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0));
        if (data == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Failed to map file");
        }
        close(fd);

        char* ptr = data;
        char* end = data + filesize;

        ppmtype = read_word(ptr, end);
        width = std::stoi(read_word(ptr, end));
        height = std::stoi(read_word(ptr, end));
        colorspace = std::stoi(read_word(ptr, end));

        int totalpixels = width * height;

        int logstep = totalpixels/100;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < totalpixels; ++i) {
            color pixel = next_color_p3(ptr, end, colorspace);
            pixel.to_linear();
            pixels.push_back(pixel);
            if (i % logstep == 0) {
                std::clog << "Reading, Progress: " << (i * 100 / totalpixels) << "%   \r";
            }
        }
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        std::clog << "Reading, Done (" << time << "ms)     " << std::endl;
        
        munmap(data, filesize);
    }
    void write() {
        std::ios_base::sync_with_stdio(false);

        const size_t approx_buffer_size = width * height * 12;

        std::string buffer;
        buffer.reserve(approx_buffer_size + 100);

        buffer.append(ppmtype).append("\n");
        buffer.append(std::to_string(width)).append(" ");
        buffer.append(std::to_string(height)).append("\n");
        buffer.append(std::to_string(colorspace)).append("\n");

        const int totalpixels = width * height;
        const int logstep = std::max(totalpixels / 100, 1);
        int pixelcount = 0;

        auto start = std::chrono::high_resolution_clock::now();

        for (color& pixel : pixels) {
            pixel.to_gamma();
            pixel.transform(colorspace);

            buffer.append(std::to_string(static_cast<int>(pixel.r))).append(" ");
            buffer.append(std::to_string(static_cast<int>(pixel.g))).append(" ");
            buffer.append(std::to_string(static_cast<int>(pixel.b))).append("\n");

            if (++pixelcount % logstep == 0) {
                std::clog << "Writing, Progress: " << (pixelcount * 100 / totalpixels) << "%\r";
            }
        }

        std::cout << buffer;

        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        std::clog << "Writing, Done (" << time << "ms)     ";
    }
    void apply_sepia() {
      for (color & pixel : pixels) {
        pixel.sepia();
      }
    }

    void pixelsort(double min, double max) {
        std::vector<color> onpixels;
        std::vector<color> transpixels;

        int totalpixels = width * height;

        int logstep = totalpixels/100;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < totalpixels; ++i) {
            color &pixel = pixels[i];

            if ((min <= pixel.hue() && pixel.hue() <= max) && i%width) {
                onpixels.push_back(pixel);
            } else {
              if (!onpixels.empty()) {
                    std::sort(onpixels.begin(), onpixels.end(), [](const color& a, const color& b) { return a < b;});
                  }
                transpixels.insert(transpixels.end(), onpixels.begin(), onpixels.end());
                onpixels.clear();
                transpixels.push_back(pixel);
              }
            if (i % logstep == 0) {
              std::clog << "Sorting, Progress: " << (i * 100 / totalpixels) << "%   \r";
            }
          }
      

        if (!onpixels.empty()) {
            std::sort(onpixels.begin(), onpixels.end());
            transpixels.insert(transpixels.end(), onpixels.begin(), onpixels.end());
        }

        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
        std::clog << "Sorting, Done (" << time << "ms)     " << std::endl;
        pixels = transpixels;
    }

  private:
    std::string read_word(char*& ptr, char* end) {
        while (ptr < end && std::isspace(*ptr)) ++ptr;
        char* start = ptr;
        while (ptr < end && !std::isspace(*ptr)) ++ptr;
        return std::string(start, ptr);
    }

    color next_color_p3(char*& ptr, char* end, double colorspace) {
        double r = std::stoi(read_word(ptr, end)) / colorspace;
        double g = std::stoi(read_word(ptr, end)) / colorspace;
        double b = std::stoi(read_word(ptr, end)) / colorspace;
        return color(r,g,b);
    }
};
#endif
