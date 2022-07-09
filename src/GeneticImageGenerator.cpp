#include "GeneticImageGenerator.hpp"
#include "Log.hpp"

#include <chrono>

GeneticImageGenerator::GeneticImageGenerator(uint32_t width, uint32_t height, uint32_t num_per_generation)
    : w(width)
    , h(height)
    , num_per_gen(num_per_generation)
{
    printLog("Create Genetic Image Generator", true);

    // SDLの初期化
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    printLog("Init SDL2", true);
}

GeneticImageGenerator::~GeneticImageGenerator()
{
    std::cout << std::endl;

    // 生成したImageの破棄
    for (auto img : this->generated_img_list)
    {
        delete img;
    }
    printLog("Delete generated images", true);

    // Surfaceの破棄
    SDL_FreeSurface(this->original_img_surface);
    printLog("Free original image surfaces", true);

    // Window・Rendererの破棄
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    printLog("Destroy window and renderer", true);

    // SDL関連の終了処理
    IMG_Quit();
    SDL_Quit();
    printLog("Quit SDL2", true);

    printLog("Destroy Genetic Image Generator", true);
}

void GeneticImageGenerator::createWindowAndRenderer()
{
    // ウィンドウを作成
    this->window = SDL_CreateWindow(
        "GeneticImageGenerator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        this->w,
        this->h,
        SDL_WINDOW_SHOWN
    );
    if (this->window == NULL)
    {
        printLog("Create main window", false);
        exit(1);
    }
    else
    {
        printLog("Create main window", true);
    }

    // レンダラの作成
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    if (this->renderer == NULL)
    {
        printLog("Create renderer", false);
        exit(1);
    }
    else
    {
        printLog("Create renderer", true);
    }
}

void GeneticImageGenerator::loadOriginalImage(std::string name)
{
    SDL_Surface* surface = IMG_Load(name.c_str());
    if (!this->original_img_surface)
    {
        printLog("Load original image", false);
        exit(1);
    }
    else
    {
        printLog("Load original image", true);
    }

    this->original_img_surface = SDL_CreateRGBSurface(0, this->w, this->h, 32, 0, 0, 0, 0);
    SDL_BlitScaled(surface, NULL, this->original_img_surface, NULL);
    printLog("Scale original image", true);

    SDL_FreeSurface(surface);
}

Image* GeneticImageGenerator::createRandomImage()
{
    Image* img = new Image(this->w, this->h);    // 真っ黒な画像を作成
    std::random_device rnd;                      // 乱数生成機

    /* 各ピクセルをランダム色で塗る */
    for (int y = 0; y < this->h; y++)
    {
        for (int x = 0; x < this->w; x++)
        {
            drawPixel(img, x, y, rnd() % 255, rnd() % 255, rnd() % 255);
        }
    }

    return img;
}

void GeneticImageGenerator::createFirstGen()
{
    std::cout << "--- Generate generation 1 ---" << std::endl;
    for (uint32_t i = 0; i < this->num_per_gen; i++)
    {
        // 処理が進んでいることを確認するための出力
        if (i % 5 == 0)
            std::cout << ".";

        Image* img = this->createRandomImage();
        this->generated_img_list.push_back(img);
    }
    std::cout << std::endl;
}

void GeneticImageGenerator::generateNextGen()
{
    std::vector<Image*> winners;    // 2つの画像のうちスコアが高い方を入れる配列

    for (uint32_t i = 0; i < this->generated_img_list.size(); i += 2)
    {
        // ここで2枚ずつスコアを比較し，勝者を選び，優秀なものを選別する

    }
}
