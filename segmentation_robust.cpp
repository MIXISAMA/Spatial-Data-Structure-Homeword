/**
 * @file segmentation_robust.cpp
 * @author zhang junbo (zhang_junbo@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 * @details
 *      compile command:  clang++ segmentation_robust.cpp -std=c++17 -o sr -lgflags
 *      run command e.g.: ./sr -i ./lianpu_1_sub.jpg -f ./lianpu_1_df.jpg -d ./lianpu_1_n/ -acc 50 -eff 0.2 -score 0.01 -mp 0.01
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

DEFINE_string(i, "./lianpu.jpg", "input image file path");
DEFINE_string(f, "./lianpu_df.jpg", "input difference image file path");
DEFINE_string(d, ".", "output dir path");
DEFINE_int32(acc, 20, "max rgb-channel cumulative threshold");
DEFINE_double(eff, 0.1, "difference image impact factor");
DEFINE_double(score, 0.005, "minimum block area [0-1]");
DEFINE_double(mp, 0.01, "max block inner area [0-1]");


int DR4[4] = {0, 1, 0, -1};
int DC4[4] = {1, 0, -1, 0};
int DR8[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
int DC8[8] = {-1,  0,  1, -1, 1, -1, 0, 1};

struct Coor
{
    int r;
    int c;
};


int bfs_stencil (
    unsigned char *image,
    unsigned char *df_image,
    int *stencil,
    int mark,
    int w, int h, int comp,
    int r, int c
)
{
    int max_acc = FLAGS_acc;
    int effect = FLAGS_eff;
    int score = 0;

    queue<Coor> q;
    q.push({r, c});
    stencil[r * w + c] = mark;
    while (!q.empty()) {
        Coor coor = q.front();
        q.pop();

        int rr = coor.r;
        int cc = coor.c;

        score++;
    
        for (int d = 0; d < 4; d++) {
            int rrr = rr + DR4[d];
            int ccc = cc + DC4[d];
            if (rrr < 0 || rrr >= h || ccc < 0 || ccc >= w) {
                continue;
            }
            if (stencil[rrr * w + ccc] > 0) {
                continue;
            }

            int acc = 0;
            for (int i = 0; i < 3; i++) {
                acc += abs(
                    image[(rrr * w + ccc) * comp + i] - image[(rr * w + cc) * comp + i]
                ) + df_image[(rr * w + cc) * comp + i] * effect;
            }
            if (acc > max_acc) {
                continue;
            }

            q.push({rrr, ccc});
            stencil[rrr * w + ccc] = mark;
        }

    }

    return score;
}

bool is_edge (
    int *stencil,
    int mark,
    int w, int h,
    int r, int c
) {
    int p = r * w + c;

    if (stencil[p] == mark) {
        return false;
    }

    for (int i = 0; i < 8; i++) {
        int rr = r + DR8[i];
        int cc = c + DC8[i];
        int pp = rr * w + cc;
        if (pp < 0 || pp >= w * h) {
            continue;
        }
        if (stencil[pp] == mark) {
            return true;
        }
    }
    return false;
}

void bfs_edge (
    int *stencil,
    int mark,
    int w, int h,
    int *edge,
    int r, int c
) {

    queue<Coor> q;
    q.push({r, c});
    edge[r * w + c] = mark;
    while (!q.empty()) {
        Coor coor = q.front();
        q.pop();

        int rr = coor.r;
        int cc = coor.c;

        for (int i = 0; i < 4; i++) {
            int rrr = DR4[i] + rr;
            int ccc = DC4[i] + cc;
            int ppp = rrr * w + ccc;

            if (rrr < 0 || rrr >= h || ccc < 0 || ccc >= w) {
                continue;
            }
            if (edge[ppp] == mark) {
                continue;
            }
            if (!is_edge(stencil, mark, w, h, rrr, ccc)) {
                continue;
            }
            
            q.push({rrr, ccc});
            edge[ppp] = mark;
        }
    }
}

void bfs_robust_stencil (
    int *stencil,
    int *edge,
    int mark,
    int w, int h,
    int r, int c,
    int max_pixel
) {
    queue<Coor> q;
    q.push({r, c});
    edge[r * w + c] = mark;
    int pixel = 0;
    while (!q.empty()) {
        Coor coor = q.front();
        q.pop();

        int rr = coor.r;
        int cc = coor.c;
        pixel++;

        for (int i = 0; i < 4; i++) {
            int rrr = DR4[i] + rr;
            int ccc = DC4[i] + cc;
            int ppp = rrr * w + ccc;

            if (rrr < 0 || rrr >= h || ccc < 0 || ccc >= w) {
                continue;
            }
            if (edge[ppp] == mark || stencil[ppp] == mark) {
                continue;
            }
            
            q.push({rrr, ccc});
            edge[ppp] = mark;
        }
    }

    if (pixel >= max_pixel) {
        return;
    }

    queue<Coor> q2;
    stencil[r * w + c] = mark;
    q2.push({r, c});
    while (!q2.empty()) {
        Coor coor = q2.front();
        q2.pop();

        int rr = coor.r;
        int cc = coor.c;

        for (int i = 0; i < 4; i++) {
            int rrr = DR4[i] + rr;
            int ccc = DC4[i] + cc;
            if (rrr < 0 || rrr >= h || ccc < 0 || ccc >= w) {
                continue;
            }
            if (stencil[rrr * w + ccc] == mark) {
                continue;
            }
            
            q2.push({rrr, ccc});
            stencil[rrr * w + ccc] = mark;
        }
    }

}

void robust_stencil (
    int *stencil,
    int mark,
    int w, int h,
    int *edge
) {

    int max_pixel = FLAGS_mp * w * h;

    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            int p = r * w + c;
            if (edge[p] == mark || stencil[p] == mark) {
                continue;
            }
            // bfs_edge(stencil, mark, w, h, edge, r, c);
            bfs_robust_stencil(stencil, edge, mark, w, h, r, c, max_pixel);
        }
    }
}

int main (int argc, char **argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    fs::path in_image_path = FLAGS_i;
    fs::path df_image_path = FLAGS_f;
    fs::path out_dir_path  = FLAGS_d;
    double min_score = FLAGS_score;

    if (!fs::exists(in_image_path) || fs::is_directory(in_image_path)) {
        cout << in_image_path << " is not exist" << endl;
        return -1;
    }

    if (!fs::exists(df_image_path) || fs::is_directory(df_image_path)) {
        cout << df_image_path << " is not exist" << endl;
        return -1;
    }

    if (!fs::exists(out_dir_path) || !fs::is_directory(out_dir_path)) {
        fs::create_directory(out_dir_path);
    }

    int w1, h1, comp1, w2, h2, comp2;
    unsigned char *image = stbi_load(in_image_path.c_str(), &w1, &h1, &comp1, 3);
    unsigned char *df_image = stbi_load(df_image_path.c_str(), &w2, &h2, &comp2, 3);

    if (w1 != w2 || h1 != h2 || comp1 != comp2) {
        cout << "size are different: "
             << "(" << w1 << ", " << h1 << ", " << comp1 << ") "
             << "(" << w2 << ", " << h2 << ", " << comp2 << ") " << endl;
        return -1;
    }
    int h = h1, w = w1, comp = comp1;

    cout << "w: " << w << " h: " << h << " comp: " << comp << endl;

    int* stencil     = new int[h * w];
    int* edge_buffer = new int[h * w];
    memset(stencil,     0, h * w * sizeof(int));
    memset(edge_buffer, 0, h * w * sizeof(int));
    int mark = 1;
    int count = 0;

    for (int r = 0; r < h; r ++) {
        for (int c = 0; c < w; c ++) {
            if (stencil[r * w + c] != 0) {
                continue;
            }
            
            int score = bfs_stencil(image, df_image, stencil, mark++, w, h, comp, r, c);
            if (score < w * h * 0.005) {
                continue;
            }
            robust_stencil(stencil, mark - 1, w, h, edge_buffer);

            unsigned char* new_image = new unsigned char[h * w * 4];
            memset(new_image, 0x00, h * w * 4);

            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    if (stencil[i * w + j] == mark - 1) {
                        for (int k = 0; k < 3; k++) {
                            new_image[(i * w + j) * 4 + k] = image[(i * w + j) * comp + k];
                        }
                        new_image[(i * w + j) * 4 + 3] = 0xFF;
                    }
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
    delete[] edge_buffer;
    
    stbi_image_free(image);

    cout << "done" << endl;
    return 0;
}
