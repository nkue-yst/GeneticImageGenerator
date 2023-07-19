#pragma once

#include <cstdint>
#include <cmath>
#include <random>

#include "color-util/RGB_to_XYZ.hpp"
#include "color-util/XYZ_to_Lab.hpp"
#include "color-util/CIEDE2000.hpp"

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

/////////////////////////////////////////////
///// 2色間のユークリッド距離を計算する /////
/////////////////////////////////////////////
static double calcEuclid(const Color& c1, const Color& c2)
{
    return std::sqrt(std::pow(c1.r - c2.r, 2) + pow(c1.g - c2.g, 2) + pow(c1.b - c2.b, 2));
}

//////////////////////////////////////////////////
///// 2色間の色差をCIEDE2000によって計算する /////
//////////////////////////////////////////////////
static double calcCIEDE2000(const Color& c1, const Color& c2)
{
    colorutil::RGB rgb1(c1.r / 255.f, c1.g / 255.f, c1.b / 255.f);
    colorutil::RGB rgb2(c2.r / 255.f, c2.g / 255.f, c2.b / 255.f);

    colorutil::XYZ xyz1 = colorutil::convert_RGB_to_XYZ(rgb1);
    colorutil::XYZ xyz2 = colorutil::convert_RGB_to_XYZ(rgb2);

    colorutil::Lab lab1 = colorutil::convert_XYZ_to_Lab(xyz1);
    colorutil::Lab lab2 = colorutil::convert_XYZ_to_Lab(xyz2);

    return colorutil::calculate_CIEDE2000(lab1, lab2);
}

static Color randomColor()
{
    std::random_device rnd;
    return Color(rnd() % 255, rnd() % 255, rnd() % 255);
}
