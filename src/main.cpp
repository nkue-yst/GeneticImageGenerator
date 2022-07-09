/************************************
 * GIG (Genetic Image Generator)
 * Y.Nakaue
 * 2022/07/07 - ????/??/??
 ************************************/

#include "GeneticImageGenerator.hpp"

#include <iostream>
#include <string>
#include <memory>

#define WIDTH  500
#define HEIGHT 500
#define NUM_PER_GENERATION 10

int main(int argc, char** argv)
{
    // 画像生成クラスの生成
    std::unique_ptr<GeneticImageGenerator> generator(new GeneticImageGenerator(WIDTH, HEIGHT, NUM_PER_GENERATION));

    // コマンドライン引数の数によってファイル名を設定
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
    generator->generateNextGen();

    SDL_Texture* tex = generator->generated_img_list.at(0)->convertToTexture(generator->renderer);

    // メインループ
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
            default:
                break;
            }
        }

        //generator->generateNextGen();

        SDL_RenderClear(generator->renderer);
        SDL_RenderCopy(generator->renderer, tex, NULL, NULL);
        SDL_RenderPresent(generator->renderer);
    }

    SDL_DestroyTexture(tex);
    return 0;
}
