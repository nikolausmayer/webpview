/**
 * Author: Nikolaus Mayer, 2018  (mayern@informatik.uni-freiburg.de)
 */

/// System/STL
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
/// Local files
#include <webp/decode.h>
#include "CImg.h"

using namespace cimg_library;


/**
 * @brief Decompress a WebP-encoded image
 * @param in_out Compressed input data/decompressed output data (must be large enough to hold the decompressed image)
 * @param compressed_size Number of data bytes in compressed blob
 */
CImg<unsigned char> decodeWebP(uint8_t* const in,
                               unsigned int compressed_size)
{
  int width, height;
  uint8_t* decoded_data = WebPDecodeRGB(in,
                                        compressed_size,
                                        &width,
                                        &height);
  CImg<unsigned char> out(width, height, 1, 3);
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      for (int c = 0; c < 3; ++c) {
        out(x,y,c) = decoded_data[(y*width+x)*3+c];
      }
    }
  }
  free(decoded_data);

  return out;
}


/**
 * @brief Test whether string B is a suffix of string A
 * @param a String A
 * @param b String B
 * @returns TRUE IFF string B is a suffix of string A
 */
bool endsWith(const std::string& a, const std::string& b)
{
  if (a.size() < b.size()) return false;

  const size_t offset{a.size()-b.size()};
  for (int i = b.size()-1; i >= 0; i--)
    if (a[offset+i] != b[i]) 
      return false;
  
  return true;
}


/**
 * MAIN
 */
int main( int argc, char** argv ) {

  if ( argc < 2 ) {
    std::cout << "Usage: " << argv[0] << " <.webp> [more files...]\n";
    return EXIT_FAILURE;
  }

  std::vector<std::string> filenames;
  for (int i = 1; i < argc; ++i) {
    filenames.push_back(argv[i]);
  }
  std::vector<CImg<unsigned char>> images;

  size_t decode_idx{0};
  for (const auto& imgfile: filenames) {
    ++decode_idx;
    /// Test file existence
    {
      std::ifstream test(imgfile);
      if (not test.is_open() or test.bad())
        throw std::runtime_error("Could not read file: "+imgfile);
      test.close();
    }

    {
      CImg<unsigned char> img;
      if (endsWith(imgfile, ".webp")) {

        std::cout << "Decoding " << std::setw(3) << decode_idx
                  << "/" << filenames.size() << '\r' << std::flush;

        std::ifstream raw(imgfile, std::ifstream::ate | std::ifstream::binary);
        const unsigned int size{static_cast<unsigned int>(raw.tellg())};
        uint8_t* raw_data = new uint8_t[size];
        raw.seekg(0, std::ios::beg);
        raw.read(reinterpret_cast<char*>(raw_data), size);
        raw.close();
        img = decodeWebP(raw_data, size);
        delete[] raw_data;
      } else {
        std::cout << std::endl << "! Unknown format: " << imgfile << std::endl;
      }

      images.push_back(img);
    }
  }
  std::cout << std::endl;

  int current_index = 0;
  CImg<unsigned char> img(images[current_index]);
  float wh_ratio = img.width()/(float)img.height();
  CImgDisplay disp(img, filenames[current_index].c_str());
  while (!disp.is_closed()) 
  {
    /// Change image
    bool change_flag = false;
    if (disp.is_keyARROWRIGHT() or disp.is_keyPAGEDOWN() or disp.wheel()<0) {
      disp.set_wheel();
      current_index += 1;
      if (current_index >= static_cast<int>(images.size()))
        current_index = 0;
      change_flag = true;
    }
    if (disp.is_keyARROWLEFT() or disp.is_keyPAGEUP() or disp.wheel()>0) {
      disp.set_wheel();
      current_index -= 1;
      if (current_index < 0)
        current_index = images.size()-1;
      change_flag = true;
    }
    if (disp.is_keyHOME()) {
      current_index = 0;
      change_flag = true;
    }
    if (disp.is_keyEND()) {
      current_index = images.size()-1;
      change_flag = true;
    }

    if (change_flag) {
      img = CImg<unsigned char>(images[current_index]);
      disp.display(img);
      disp.set_title(filenames[current_index].c_str());
    }

    /// Handle window resizing events
    if (disp.is_resized())
    {
      /// Reload the original image, else repeated resizing
      /// obliterates the data
      disp.display(img);
      /// Keep original width/height ratio
      uint new_w = disp._window_width;
      uint new_h = disp._window_height;
      if (new_w/wh_ratio <= new_h)
        disp.resize(new_w, new_w/wh_ratio);
      else
        disp.resize(new_h*wh_ratio, new_h);
    }

    /// Print pixel values under cursor
    int mx = disp.mouse_x();
    int my = disp.mouse_y();
    if (mx >= 0 and mx < disp.width() and 
        my >= 0 and my < disp.height()) 
    {
      static std::ostringstream oss;
      oss.str("");
      oss << "(" << mx << "," << my << "): (";
      for (int c = 0; c < img.spectrum()-1; ++c)
        oss << static_cast<int>(img(mx,my,c)) << ", ";
      oss << static_cast<int>(img(mx,my,img.spectrum()-1)) << ")\n";
      std::cout << oss.str();
    }
    
    /// Close window if ESC is pressed
    if (disp.is_keyESC())
      break;

    disp.wait();
  }

  /// Bye!
  return EXIT_SUCCESS;
}

