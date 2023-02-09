/************************************
 * GIG (Genetic Image Generator)
 * Y.Nakaue
 * 2022/07/07 - ????/??/??
 ************************************/

#include "GeneticImageGenerator.hpp"

#include <iostream>
#include <string>
#include <memory>

#define WIDTH  100
#define HEIGHT 100
#define NUM_PER_GENERATION 300    // 世代ごとの生成する画像数
#define MUTATION_RATE 0.01f        // 突然変異の確率（1%）

int main(int argc, char** argv)
{
    // 画像生成クラスの生成
    std::unique_ptr<GeneticImageGenerator> generator(new GeneticImageGenerator(WIDTH, HEIGHT, NUM_PER_GENERATION, MUTATION_RATE));

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

    generator->createWindowAndRenderer();               // ウィンドウ・レンダラを作成
    generator->loadOriginalImage(original_img_name);    // 元画像を読み込む
    generator->createFirstGen();                        // 最初の世代の画像を生成する

    SDL_Texture* tex = NULL;

    bool loop = true;
    while (loop)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                loop = false;
                break;
            
            case SDL_WINDOWEVENT:
                if (ev.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    loop = false;
                    break;
                }

            default:
                break;
            }
        }

        SDL_DestroyTexture(tex);
        tex = generator->generated_img_list.at(0)->convertToTexture(generator->renderer);

        SDL_RenderClear(generator->renderer);
        SDL_RenderCopy(generator->renderer, tex, NULL, NULL);
        SDL_RenderPresent(generator->renderer);

        generator->generateNextGen();    // 次の世代を生成する
    }

    SDL_DestroyTexture(tex);
    return 0;
}
