/**
 * @file difference.cpp
 * @author zhang junbo (zhang_junbo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 * @details
 *      compile command:  clang++ difference.cpp -std=c++17 -o df -lgflags 
 *      run command e.g.: ./df -i3 ./lianpu_1_gb_3x3.jpg -i5 ./lianpu_1_gb_5x5.jpg -o ./lianpu_1_df.jpg
 */

#include <iostream>
#include <sstream>
#include <queue>
#include <cmath>

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <gflags/gflags.h>

DEFINE_string(i3, "./lianpu_1_gb_3x3.jpg" ,  "input 3x3core gaussian bur image file path");
DEFINE_string(i5, "./lianpu_1_gb_5x5.jpg",   "input 5x5core gaussian bur image file path");
DEFINE_string(o,  "./lianpu_1_df.jpg",       "output image file path");

int core3x3[3*3] = {
    1, 2, 1,
    2, 4, 2,
    1, 2, 1,
};

int total3x3 = 16;

int core5x5[5*5] = {
    1,  4,  7,  4, 1,
    4, 16, 26, 16, 4,
    7, 26, 41, 26, 7,
    4, 16, 26, 16, 4,
    1,  4,  7,  4, 1,
};

int total5x5 = 273;

int main (int argc, char **argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    fs::path in_image_path_3 = FLAGS_i3;
    fs::path in_image_path_5 = FLAGS_i5;
    fs::path out_image_path  = FLAGS_o;

    if (!fs::exists(in_image_path_3) || fs::is_directory(in_image_path_3)) {
        cout << in_image_path_3 << " is not exist" << endl;
        return -1;
    }

    if (!fs::exists(in_image_path_5) || fs::is_directory(in_image_path_5)) {
        cout << in_image_path_5 << " is not exist" << endl;
        return -1;
    }

    int w3, h3, comp3, w5, h5, comp5;
    unsigned char *image_3 = stbi_load(in_image_path_3.c_str(), &w3, &h3, &comp3, 3);
    unsigned char *image_5 = stbi_load(in_image_path_5.c_str(), &w5, &h5, &comp5, 3);
    if (w3 != w5 || h3 != h5 || comp3 != comp5) {
        cout << "size are different: "
             << "(" << w3 << ", " << h3 << ", " << comp3 << ") "
             << "(" << w5 << ", " << h5 << ", " << comp5 << ") " << endl;
        return -1;
    }
    int h = h3, w = w3, comp = comp3;

    unsigned char* new_image = new unsigned char[h * w * 3];
    memset(new_image, 0xFF, h * w * 3);

    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            for (int k = 0; k < 3; k++) {
                int val = (int)image_3[(r * w + c) * comp + k] - (int)image_5[(r * w + c) * comp + k];
                new_image[(r * w + c) * 3 + k] = abs(val);
            }
        }
    }

    cout << "writing " << out_image_path << endl;
    stbi_write_png(out_image_path.c_str(), w, h, 3, new_image, 0);

    delete[] new_image;
    stbi_image_free(image_3);
    stbi_image_free(image_5);

    cout << "done" << endl;
    return 0;
}
