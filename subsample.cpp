/**
 * @file subsample.cpp
 * @author zhang junbo (zhang_junbo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 * @details
 *      compile command:  clang++ subsample.cpp -std=c++17 -o sub -lgflags 
 *      run command e.g.: ./sub -i ./lianpu_1.jpg -o ./lianpu_1_sub.jpg -s 5
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

DEFINE_string(i, "./lianpu_1.jpg" ,     "input  image file path");
DEFINE_string(o, "./lianpu_1_sub.jpg",  "output image file path");
DEFINE_int32(s, 5, "subsample");


int main (int argc, char **argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    fs::path in_image_path  = FLAGS_i;
    fs::path out_image_path = FLAGS_o;
    int sub = FLAGS_s;

    if (!fs::exists(in_image_path) || fs::is_directory(in_image_path)) {
        cout << in_image_path << " is not exist" << endl;
        return -1;
    }

    int w, h, comp;
    unsigned char *image = stbi_load(in_image_path.c_str(), &w, &h, &comp, 3);

    int sub_w = w / sub;
    int sub_h = h / sub;
    unsigned char *new_image = new unsigned char[sub_w * sub_h * 3];
    memset(new_image, 0xFF, sub_w * sub_h * 3);

    for (int r = 0; r < sub_h; r++) {
        for (int c = 0; c < sub_w; c++) {
            for (int k = 0; k < 3 ; k++) {
                new_image[(r * sub_w + c) * 3 + k] = image[(r * sub * sub_w * sub + c * sub) * comp + k];
            }
        }
    }

    cout << "writing " << out_image_path << endl;
    stbi_write_png(out_image_path.c_str(), sub_w, sub_h, 3, new_image, 0);

    delete[] new_image;
    stbi_image_free(image);

    cout << "done" << endl;
    return 0;
}
