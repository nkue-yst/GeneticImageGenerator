#pragma once
#include "Image.hpp"

#include <random>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

class GeneticImageGenerator
{
public:
    GeneticImageGenerator(uint32_t width, uint32_t height, uint32_t num_per_generation, float mutation_rate);
    ~GeneticImageGenerator();

    /* ウィンドウ・レンダラの作成 */
    void createWindowAndRenderer();

    /* 元画像を読み込む */
    void loadOriginalImage(std::string name);

    /* Imageに対して指定座標を指定色で塗る処理 */
    inline void drawPixel(Image* img, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b)
    {
        img->pixels.at(y * img->w + x) = Color(r, g, b);
    }

    /* 各ピクセルがランダム色で塗られた画像(Surface)を作成する */
    Image* createRandomImage();

    /* 初期画像を指定個数作成する */
    void createFirstGen();

    /* 次の世代の画像を生成する */
    void generateNextGen();

    uint32_t w;              // 生成する画像の横幅
    uint32_t h;              // 生成する画像の高さ
    uint32_t num_per_gen;    // 世代ごとに生成する画像の数
    float mutation_rate;     // 突然変異の確率

    std::vector<Image*> generated_img_list;    // 生成した画像リスト

    SDL_Window* window;        // メインウィンドウ
    SDL_Renderer* renderer;    // メインウィンドウ用レンダラ

    Image* original_img;    // 元画像データ
};
