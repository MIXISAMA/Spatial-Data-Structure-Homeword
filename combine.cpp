/**
 * @file combine.cpp
 * @author zhang junbo (zhang_junbo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 * @details
 *      compile command:  clang++ combine.cpp -std=c++17 -o cb -lgflags 
 *      run command e.g.: ./cb -p ./path.txt -o ./combine.png
 */

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <gflags/gflags.h>

DEFINE_string(p, "./path.txt" ,       "image path list file");
DEFINE_string(o, "./combine.jpg", "output image file path");

int main (int argc, char **argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    fs::path path_list_file = FLAGS_p;
    fs::path out_image_path = FLAGS_o;

    if (!fs::exists(path_list_file) || fs::is_directory(path_list_file)) {
        cout << path_list_file << " is not exist" << endl;
        return -1;
    }

    ifstream input_file(path_list_file);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - " << path_list_file << endl;
        return -1;
    }

    string line;
    int w = 0, h = 0, comp = 0;
    unsigned char* new_image = nullptr;

    while (getline(input_file, line)){
        if (line == "") {
            continue;
        }
        fs::path image_path = line;
        if (!fs::exists(image_path) || fs::is_directory(image_path)) {
            cerr << image_path << " is not exist" << endl;
            return -1;
        }

        int ww, hh, ccomp;
        unsigned char *image = stbi_load(image_path.c_str(), &ww, &hh, &ccomp, 4);
        if (w == 0) {
            w = ww;
            h = hh;
            comp = ccomp;
            cout << "w: " << w << " h: " << h << " comp: " << comp << endl;
            new_image = new unsigned char[h * w * 4];
            memset(new_image, 0x00, h * w * 4);
        }
        if (w != ww || h != hh || comp != ccomp) {
            cout << "size are different: "
                 << "(" << w  << ", " << h  << ", " << comp  << ") "
                 << "(" << ww << ", " << hh << ", " << ccomp << ") " << endl;
            stbi_image_free(image);
            continue;
        }

        cout << "drawing " << image_path << endl;


        for (int r = 0; r < h; r++) {
            for (int c = 0; c < w; c++) {
                if (image[(r * w + c) * comp + 3] == 0x00) {
                    continue;
                }
                for (int k = 0; k < comp; k++) {
                    new_image[(r * w + c) * 4 + k] = image[(r * w + c) * comp + k];
                }
            }
        }

        stbi_image_free(image);
    }

    cout << "writing " << out_image_path << endl;
    stbi_write_png(out_image_path.c_str(), w, h, 4, new_image, 0);

    delete[] new_image;

    cout << "done" << endl;
    return 0;
}
