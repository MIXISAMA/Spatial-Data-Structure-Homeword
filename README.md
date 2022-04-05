# 空间数据结构作业

针对多个复杂图像分割后的图像块进行拼接，组装生成新图像。如多个脸谱图像的分割和不同脸谱部位组装。

## 效果

..Todo

## 依赖

* c++17
* gflags
* stb_image

您需要手动把`stb_image.h`和`stb_image_write.h`放到根目录。

## 命令

所有的命令都放置在[script.txt](https://github.com/MIXISAMA/Spatial-Data-Structure-Homeword/blob/master/script.txt)里面了。

编译命令

```shell
clang++ subsample.cpp           -std=c++17 -o sub -lgflags
clang++ gaussian_blur.cpp       -std=c++17 -o gb  -lgflags
clang++ difference.cpp          -std=c++17 -o df  -lgflags
clang++ segmentation_robust.cpp -std=c++17 -o sr  -lgflags
clang++ combine.cpp             -std=c++17 -o cb  -lgflags 
```

运行命令

```shell
# 下采样: 每5个像素采样一次
./sub -i ./lianpu_1/lianpu_1.jpg -o ./lianpu_1/lianpu_1_sub.jpg -s 5
# 高斯模糊 3x3
./gb -i ./lianpu_1/lianpu_1_sub.jpg -o ./lianpu_1/lianpu_1_gb_3x3.jpg
# 高斯模糊 5x5
./gb -i ./lianpu_1/lianpu_1_sub.jpg -o ./lianpu_1/lianpu_1_gb_5x5.jpg -5x5
# 做差提取边缘
./df -i3 ./lianpu_1/lianpu_1_gb_3x3.jpg -i5 ./lianpu_1/lianpu_1_gb_5x5.jpg -o ./lianpu_1/lianpu_1_df.jpg
# 分割 acc:总阀值 eff:边缘系数 score:色块忽略阀值 mp:内部孔洞修复阀值
./sr -i ./lianpu_1/lianpu_1_sub.jpg -f ./lianpu_1/lianpu_1_df.jpg -d ./lianpu_1/sr/ -acc 50 -eff 0.2 -score 0.01 -mp 0.01
# 拼接
./cb -p ./path.txt -o ./combine.png
```

拼接是使用[path.txt](https://github.com/MIXISAMA/Spatial-Data-Structure-Homeword/blob/master/script.txt)中的定义的图片路径，按顺序放置在新的画布上生成的。

## 原理

最开始是模仿photoshop的魔棒工具，对下采样后的点进行四联通bfs搜索，搜索依据相邻点之间rgb三维值的曼哈顿距离不超过阀值。但是这样做效果不佳，会出现分不开，内部空洞，锯齿边等现象。于是提取边缘，把边缘值也作为判断联通的条件之一。然后依旧bfs四联通搜索即可。

相邻点距离 = 相邻点rgb曼哈顿距离和 + 边缘值 * 边缘系数

最后在便利看一下有没有内部空洞，有就补上。

## 其他

MIT协议不多逼逼，帮到您了给咱加个星吧。

仓库名应该是Homework，但是写成Homeword了, 索性就这样吧不改了。
