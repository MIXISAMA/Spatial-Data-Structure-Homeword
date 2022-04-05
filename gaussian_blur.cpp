/**
 * @file gaussian_blur.cpp
 * @author zhang junbo (zhang_junbo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 * @details
 *      compile command:  clang++ gaussian_blur.cpp -std=c++17 -o gb -lgflags 
 *      run command e.g.: ./gb -i ./lianpu_1_sub.jpg -o ./lianpu_1_gb_5x5.jpg -5x5
 *                        ./gb -i ./lianpu_1_sub.jpg -o ./lianpu_1_gb_3x3.jpg
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

DEFINE_string(i, "./lianpu.jpg" ,       "input image file path");
DEFINE_string(o, "./lianpu_blur.jpg",   "output image file path");
DEFINE_bool(5x5, false,                 "false 3x3; true 5x5");

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
    fs::path in_image_path  = FLAGS_i;
    fs::path out_image_path = FLAGS_o;
    bool is5x5 = FLAGS_5x5;

    if (!fs::exists(in_image_path) || fs::is_directory(in_image_path)) {
        cout << in_image_path << " is not exist" << endl;
        return -1;
    }

    int w, h, comp;
    unsigned char *image = stbi_load(in_image_path.c_str(), &w, &h, &comp, 3);

    cout << "w: " << w << " h: " << h << " comp: " << comp << " is5x5: " << is5x5 << endl;

    unsigned char* new_image = new unsigned char[h * w * 3];
    memset(new_image, 0xFF, h * w * 3);

    int core_size  = is5x5 ? 5 : 3;
    int *core      = is5x5 ? core5x5 : core3x3;
    int core_total = is5x5 ? total5x5 : total3x3;

    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            for (int k = 0; k < 3; k++) {
                int total = 0;
                for (int i = 0; i < core_size; i++) {
                    for (int j = 0; j < core_size; j++) {
                        int rr = r + i - core_size / 2;
                        int cc = c + j - core_size / 2;
                        if (rr < 0 || rr >= h || cc < 0 || cc >= w) {
                            continue;
                        }
                        total += image[(rr * w + cc) * comp + k] * core[i * core_size + j];
                    }
                }
                new_image[(r * w + c) * 3 + k] = total / core_total;
            }
        }
    }

    cout << "writing " << out_image_path << endl;
    stbi_write_png(out_image_path.c_str(), w, h, 3, new_image, 0);

    delete[] new_image;
    stbi_image_free(image);

    cout << "done" << endl;
    return 0;
}
