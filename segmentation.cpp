/**
 * @file segmentation.cpp
 * @author zhang junbo (zhang_junbo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 * @details
 *      compile command:  clang++ segmentation.cpp -std=c++17 -o segmentation -lgflags
 *      run command e.g.: ./segmentation -in_image_path ./lianpu_1.jpg -out_dir_path ./lianpu_1/
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

DEFINE_string(in_image_path, "lianpu.jpg", "input image file path");
DEFINE_string(out_dir_path, ".", "output dir path");

int DR[4] = {0, 1, 0, -1};
int DC[4] = {1, 0, -1, 0};

int deal(
    unsigned char *data,
    int *stencil,
    int mark,
    int w, int h, int comp,
    int r, int c
)
{
    struct Coor
    {
        int r;
        int c;
    };

    int count = 0;

    queue<Coor> q;
    q.push({r, c});
    while (!q.empty()) {
        Coor coor = q.front();
        q.pop();

        int rr = coor.r;
        int cc = coor.c;
        
        if (stencil[rr * w + cc] != 0) {
            continue;
        }

        stencil[rr * w + cc] = mark;
        count ++;
    
        for (int d = 0; d < 4; d++) {
            int rrr = rr + DR[d];
            int ccc = cc + DC[d];
            if (rrr < 0 || rrr >= h || ccc < 0 || ccc >= w) {
                continue;
            }
            if (stencil[rrr * w + ccc] != 0) {
                continue;
            }

            int acc = 0;
            for (int i = 0; i < 3; i++) {
                acc += abs(data[(rrr * w + ccc) * comp + i] - data[(rr * w + cc) * comp + i]);
            }
            if (acc > 23) {
                continue;
            }

            q.push({rrr, ccc});
        }

    }
    return count;
    
}

int main (int argc, char **argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    fs::path in_image_path = FLAGS_in_image_path;
    fs::path out_dir_path  = FLAGS_out_dir_path;

    if (!fs::exists(in_image_path) || fs::is_directory(in_image_path)) {
        cout << in_image_path << " is not exist" << endl;
        return -1;
    }

    if (!fs::exists(out_dir_path) || !fs::is_directory(out_dir_path)) {
        fs::create_directory(out_dir_path);
    }

    int w, h, comp;
    unsigned char *data = stbi_load(in_image_path.c_str(), &w, &h, &comp, 3);

    cout << "w: " << w << " h: " << h << " comp: " << comp << endl;

    int* stencil = new int[h * w];
    int mark = 1;
    memset(stencil, 0, h * w * sizeof(int));
    int count = 0;

    for (int r = 0; r < h; r += 3) {
        for (int c = 0; c < w; c += 3) {
            if (stencil[r * w + c] != 0) {
                continue;
            }
            
            int score = deal(data, stencil, mark++, w, h, comp, r, c);
            if (score < w * h * 0.005) {
                continue;
            }

            unsigned char* new_image = new unsigned char[h * w * 4];
            memset(new_image, 0x00, h * w * 4);

            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    if (stencil[i * w + j] == mark - 1) {
                        for (int k = 0; k < 3; k++) {
                            new_image[(i * w + j) * 4 + k] = data[(i * w + j) * comp + k];
                        }
                        new_image[(i * w + j) * 4 + 3] = 0xFF;
                    }
                    // else {
                    //     for (int k = 0; k < 4; k++) {
                    //         new_image[(i * w + j) * 4 + k] = 0x00; //128u;
                    //     }
                    // }
                }
            }

            ostringstream ss;
            ss << in_image_path.stem().string() << "_" << count++ << ".png";
            fs::path out_image_path = out_dir_path / ss.str();

            cout << "writing " << out_image_path << endl;
            stbi_write_png(out_image_path.c_str(), w, h, 4, new_image, 0);

            delete[] new_image;
        }
    }

    delete[] stencil;
    
    stbi_image_free(data);

    cout << "done" << endl;
    return 0;
}
