/************************************
 * GIG (Genetic Image Generator)
 * Y.Nakaue
 * 2022/07/07 - ????/??/??
 ************************************/

#include <iostream>
#include <random>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

#define GIG_WIDTH  500    // �g�p����摜�̉���
#define GIG_HEIGHT 500    // �g�p����摜�̍���

/* 8�r�b�gx4�iRGBA�j��Uint32�^�ŕ\�����邽�߂̃V�t�g���镝 */
#define RSHIFT (0)
#define GSHIFT (8)
#define BSHIFT (16)
#define ASHIFT (24)

/* SDL�֘A�̏I������ */
void quit()
{
    IMG_Quit();
    SDL_Quit();
}

/* Surface�ɑ΂��Ďw����W���w��F�œh�鏈�� */
void drawPixel(SDL_Surface* surface, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32* pixels = (Uint32*)surface->pixels;

    Uint32 color = (r << RSHIFT) | (g << GSHIFT) | (b << BSHIFT) | (255 << ASHIFT);

    pixels[y * surface->w + x] = color;
}

/* �e�s�N�Z���������_���F�œh��ꂽ�摜(Surface)���쐬���� */
SDL_Surface* createRandomSurface(SDL_Renderer* renderer)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, GIG_WIDTH, GIG_HEIGHT, 32, 0, 0, 0, 0);    // �^�����ȉ摜���쐬
    std::random_device rnd;    // ���������@

    /* �e�s�N�Z���������_���F�œh�� */
    for (int y = 0; y < GIG_HEIGHT; y++)
    {
        for (int x = 0; x < GIG_WIDTH; x++)
        {
            drawPixel(surface, x, y, rnd() % 255, rnd() % 255, rnd() % 255);
        }
    }
    return surface;
}

int main(int argc, char** argv)
{
    std::string original_img_name;  // ���摜��

    // �R�}���h���C�������̐��ɂ���ăt�@�C������ݒ�
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

    // SDL�̏�����
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    // �E�B���h�E���쐬
    SDL_Window* window = SDL_CreateWindow(
        "GeneticImageGenerator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GIG_WIDTH,
        GIG_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL)
    {
        std::cerr << "Failed to create window" << std::endl;
        quit();
        exit(1);
    }
    // �����_���̍쐬
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        std::cerr << "Failed to create renderer" << std::endl;
        quit();
        exit(1);
    }

    // ���摜��ǂݍ���
    SDL_Surface* original_img = IMG_Load(original_img_name.c_str());
    if (!original_img)
    {
        std::cerr << "Failed to load image: " << original_img_name << std::endl;
        quit();
        exit(1);
    }

    // ���摜���g�p�������T�C�Y�Ƀ��T�C�Y
    SDL_Surface* resized_img = new SDL_Surface();
    resized_img->w = GIG_WIDTH;
    resized_img->h = GIG_HEIGHT;
    SDL_BlitScaled(original_img, NULL, resized_img, NULL);

    // Surface����Texture���쐬
    SDL_Texture* original_texture = SDL_CreateTextureFromSurface(renderer, original_img);

    // �����_���ȏ����摜�̍쐬
    SDL_Surface* surface_1 = createRandomSurface(renderer);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface_1);

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

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
    // Surface�̌㏈��
    SDL_FreeSurface(surface_1);
    SDL_FreeSurface(resized_img);
    SDL_FreeSurface(original_img);

    // Texture�̌㏈��
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(original_texture);

    // SDL�̏I������
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    quit();

    return 0;
}
