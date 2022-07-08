#include <iostream>
#include <random>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

void quit()
{
    IMG_Quit();
    SDL_Quit();
}

void drawPixel(SDL_Surface* surface, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_MapRGB(surface->format + y * surface->pitch + x * surface->format->BytesPerPixel, r, g, b);
}

SDL_Surface* createRandomSurface(SDL_Renderer* renderer)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);
    std::random_device rnd;

    for (int y = 0; y < 100; y++)
    {
        for (int x = 0; x < 100; x++)
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
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    // �E�B���h�E���쐬
    SDL_Window* window = SDL_CreateWindow(
        "GeneticImageGenerator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        100,
        100,
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

    // ���摜��100x100�Ƀ��T�C�Y
    SDL_Surface* resized_img = new SDL_Surface();
    resized_img->w = 100;
    resized_img->h = 100;
    SDL_BlitScaled(original_img, NULL, resized_img, NULL);

    // Surface����Texture���쐬
    SDL_Texture* original_texture = SDL_CreateTextureFromSurface(renderer, original_img);

    // �����_���ȏ����摜�̍쐬
    SDL_Surface* surface_1 = createRandomSurface(renderer);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface_1);

    // ���C�����[�v
    while (1)
    {
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
