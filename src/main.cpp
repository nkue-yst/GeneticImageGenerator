#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Not enough param" << std::endl;
        return 1;
    }

    const char* original_img = argv[0];


    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    SDL_Delay(1000);

    IMG_Quit();
    SDL_Quit();
}
