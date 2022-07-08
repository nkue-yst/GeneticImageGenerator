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
    GeneticImageGenerator(uint32_t width, uint32_t height, uint32_t num_per_generation);
    ~GeneticImageGenerator();

    /* �E�B���h�E�E�����_���̍쐬 */
    void createWindowAndRenderer();

    /* ���摜��ǂݍ��� */
    void loadOriginalImage(std::string name);

    /* Image�ɑ΂��Ďw����W���w��F�œh�鏈�� */
    inline void drawPixel(Image* img, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b)
    {
        img->pixels.at(y * img->w + x) = Color(r, g, b);
    }

    /* �e�s�N�Z���������_���F�œh��ꂽ�摜(Surface)���쐬���� */
    Image* createRandomImage();

    /* �����摜���w����쐬���� */
    void createFirstGen();

    uint32_t w;              // ��������摜�̉���
    uint32_t h;              // ��������摜�̍���
    uint32_t num_per_gen;    // ���ゲ�Ƃɐ�������摜�̐�

    std::vector<Image*> generated_img_list;

    SDL_Window* window;        // ���C���E�B���h�E
    SDL_Renderer* renderer;    // ���C���E�B���h�E�p�����_��

    SDL_Surface* original_img_surface;    // ���摜�f�[�^
};
