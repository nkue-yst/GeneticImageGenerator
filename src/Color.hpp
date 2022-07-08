#pragma once
#include <cstdint>
#include <cmath>

class Color
{
public:
    Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
        : r(red)
        , g(green)
        , b(blue)
    {
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

/* 2色間のユークリッド距離を計算する */
static double calcEuclid(const Color& c1, const Color& c2)
{
    double dist = std::sqrt(std::pow(c1.r - c2.r, 2) + pow(c1.g - c2.g, 2) + pow(c1.b - c2.b, 2));
    return dist;
}
