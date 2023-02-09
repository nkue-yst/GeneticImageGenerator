#include "GeneticImageGenerator.hpp"
#include "Log.hpp"

#include <algorithm>
#include <chrono>
#include <string>
#include <utility>

#define ELITE_NUM 4

GeneticImageGenerator::GeneticImageGenerator(uint32_t width, uint32_t height, uint32_t num_per_generation, float mutation_rate)
    : w(width)
    , h(height)
    , num_per_gen(num_per_generation)
    , mutation_rate(mutation_rate)
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

    // 最後に生成した世代の画像を保存
    std::string file_name = "gen";
    file_name += std::to_string(this->current_gen);
    file_name += ".png";

    SDL_Surface* surface = this->generated_img_list.front()->convertToSurface();
    IMG_SavePNG(surface, file_name.c_str());
    SDL_FreeSurface(surface);

    // 生成したImageの破棄
    delete this->original_img;
    for (auto img : this->generated_img_list)
    {
        delete img;
    }
    printLog("Delete generated images", true);

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

    // 初期描画
    SDL_RenderClear(this->renderer);
    SDL_RenderPresent(this->renderer);
}

void GeneticImageGenerator::loadOriginalImage(std::string name)
{
    SDL_Surface* surface = IMG_Load(name.c_str());
    if (!surface)
    {
        printLog("Load original image", false);
        exit(1);
    }
    else
    {
        printLog("Load original image", true);
    }

    SDL_Surface* original_img_surface = SDL_CreateRGBSurface(0, this->w, this->h, 32, 0, 0, 0, 0);
    SDL_BlitScaled(surface, NULL, original_img_surface, NULL);
    IMG_SavePNG(original_img_surface, "original.png");
    printLog("Scale original image", true);

    this->original_img = Image::convertoToImage(original_img_surface);

    SDL_FreeSurface(original_img_surface);
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
        {
            std::cout << ".";
            std::flush(std::cout);
        }

        Image* img = this->createRandomImage();
        this->generated_img_list.push_back(img);
    }

    /* 第0世代を保存する */
    SDL_Surface* surface = this->generated_img_list.front()->convertToSurface();
    IMG_SavePNG(surface, "gen0.png");
    SDL_FreeSurface(surface);

    std::cout << std::endl << "--- Complete generating generation 1 ---" << std::endl;
}

void GeneticImageGenerator::generateNextGen()
{
    std::cout << "--- Generate generation " << current_gen << " ---" << std::endl;

    /* 生成した全ての画像のスコアを計算 */
    std::vector<std::pair<double, Image*>> scored_list;    // 計算したスコアとImageのペアを保持するリスト
    for (Image* img : this->generated_img_list)
    {
        std::pair<double, Image*> score(img->calcScore(this->original_img), img);
        scored_list.push_back(score);
    }
    std::sort(scored_list.begin(), scored_list.end());    // スコアの昇順に並べ替え

    /* スコア上位数個を選抜 */
    std::vector<Image*> new_gen;    // 次の世代の画像リスト

    Image* elite[ELITE_NUM];
    for (int i = 0; i < ELITE_NUM; i++)
    {
        elite[i] = scored_list.at(i).second;
        Image* elite_img = new Image(*elite[i]);
        new_gen.push_back(elite_img);
    }

    /* 上位画像数個からランダムにピクセルデータをコピーし，新たな画像を生成する */
    std::random_device rnd;    // 乱数生成機

    for (int i = 0; i < this->num_per_gen - ELITE_NUM; i++)
    {
        Image* img = new Image(this->w, this->h);

        for (uint32_t y = 0; y < this->h; y++)
        {
            for (uint32_t x = 0; x < this->w; x++)
            {
                if ((double)rnd() / std::random_device::max() < this->mutation_rate)    // 突然変異
                {
                    img->pixels[y * img->w + x] = randomColor();
                }
                else
                {
                    img->pixels[y * img->w + x] = elite[rnd() % ELITE_NUM]->pixels[y * img->w + x];
                }
            }
        }

        new_gen.push_back(img);
    }

    /* 上位画像のスコアを出力 */
    for (int i = 0; i < ELITE_NUM; i++)
        std::cout << "Score[" << i + 1 << "]: " << elite[i]->calcScore(this->original_img) << std::endl;

    // 100世代ごとに画像を保存
    if (this->current_gen % 10 == 0)
    {
        std::string file_name = "gen";
        file_name += std::to_string(this->current_gen);
        file_name += ".png";

        SDL_Surface* surface = this->generated_img_list.front()->convertToSurface();
        IMG_SavePNG(surface, file_name.c_str());
        SDL_FreeSurface(surface);
    }

    std::cout << "--- Complete generating generation " << this->current_gen++ << " ---" << std::endl;

    // 最新の世代を更新する
    std::for_each(this->generated_img_list.begin(), this->generated_img_list.end(), [](Image* img)
        {
            delete img;
        });
    this->generated_img_list = new_gen;
}
