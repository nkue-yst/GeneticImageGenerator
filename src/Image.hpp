#pragma once
#include "Color.hpp"

#include <iostream>
#include <vector>

#include <SDL.h>

/* 8ビットx4（RGBA）をUint32型で表現するためのシフトする幅 */
#define RSHIFT (0)
#define GSHIFT (8)
#define BSHIFT (16)
#define ASHIFT (24)

class Image
{
public:
    /* 初期化時には横幅・高さを指定し，黒で塗りつぶす */
    Image(uint32_t width, uint32_t height)
        : w(width)
        , h(height)
    {
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                this->pixels.push_back(Color());
            }
        }
    }

    /* SDL_Surfaceに変換する処理 */
    SDL_Surface* convertToSurface()
    {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, this->w, this->h, 32, 0, 0, 0, 0);
        Uint32* pixels = (Uint32*)surface->pixels;

        for (uint32_t y = 0; y < this->h; y++)
        {
            for (uint32_t x = 0; x < this->w; x++)
            {
                Color c = this->pixels[y * this->w + x];
                Uint32 c_value = (c.r << RSHIFT) | (c.g << GSHIFT) | (c.b << BSHIFT) | (255 << ASHIFT);

                pixels[y * surface->w + x] = c_value;
            }
        }

        return surface;
    }

    /* SDL_Textureに変換する処理 */
    SDL_Texture* convertToTexture(SDL_Renderer* renderer)
    {
        SDL_Surface* surface = this->convertToSurface();
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);
        return texture;
    }

    /* 2枚の画像間のスコアを計算する */
    double calcScore(const Image* img)
    {
        double score = 0.0;

        for (uint32_t y = 0; y < this->h; y++)
        {
            for (uint32_t x = 0; x < this->w; x++)
            {
                score += calcEuclid(this->pixels[y * this->w + x], img->pixels[y * img->w + x]);
            }
        }

        return score;
    }

    uint32_t w;    // 横幅
    uint32_t h;    // 高さ

    std::vector<Color> pixels;    // 色情報
};
