/************************************
 * GIG (Genetic Image Generator)
 * Y.Nakaue
 * 2022/07/07 - ????/??/??
 ************************************/

#include "GeneticImageGenerator.hpp"

#include <iostream>
#include <string>

#define WIDTH  500
#define HEIGHT 500
#define NUM_PER_GENERATION 100

int main(int argc, char** argv)
{
    // �摜�����N���X�̐���
    std::unique_ptr<GeneticImageGenerator> generator(new GeneticImageGenerator(WIDTH, HEIGHT, NUM_PER_GENERATION));

    // �R�}���h���C�������̐��ɂ���ăt�@�C������ݒ�
    std::string original_img_name;
    if (argc == 2)
    {
        original_img_name = argv[1];
    }
    else if (argc < 2)
    {
        std::cout << "Original image file -> ";
        std::cin >> original_img_name;
    }
    else
    {
        std::cerr << "Too many arguments" << std::endl;
        exit(1);
    }

    generator->createWindowAndRenderer();               // �E�B���h�E�E�����_�����쐬
    generator->loadOriginalImage(original_img_name);    // ���摜��ǂݍ���
    generator->createFirstGen();                        // �ŏ��̐���̉摜�𐶐�����

    SDL_Texture* tex = generator->generated_img_list.at(0)->convertToTexture(generator->renderer);

    // ���C�����[�v
    bool loop = true;
    while (loop)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_KEYDOWN:
                std::cout << "Any key down" << std::endl;
                break;
            case SDL_QUIT:
                loop = false;
                break;
            default:
                break;
            }
        }

        SDL_RenderClear(generator->renderer);
        SDL_RenderCopy(generator->renderer, tex, NULL, NULL);
        SDL_RenderPresent(generator->renderer);
    }

    SDL_DestroyTexture(tex);
    return 0;
}
