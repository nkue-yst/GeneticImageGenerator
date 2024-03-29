#include "GeneticImageGenerator.hpp"
#include "Log.hpp"

#include <algorithm>
#include <filesystem>
#include <utility>

#include "pbar.hpp"

#define ELITE_NUM 4

GeneticImageGenerator::GeneticImageGenerator(uint32_t width, uint32_t height, uint32_t num_per_generation, float mutation_rate)
    : w(width)
    , h(height)
    , num_per_gen(num_per_generation)
    , mutation_rate(mutation_rate)
{
    printLog("Create Genetic Image Generator", true);

    this->generated_img_list.reserve(num_per_generation);

    ///////////////////////
    ///// SDLの初期化 /////
    ///////////////////////
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    printLog("Init SDL2", true);

    // 生成画像の保存先ディレクトリを作成
    std::filesystem::create_directory("generated_image");

    // 最高スコアの記録用csvファイルを作成
    this->ofs_record_file = std::ofstream("score.csv");
}

GeneticImageGenerator::~GeneticImageGenerator()
{
    std::cout << std::endl;

    //////////////////////////////////////////
    ///// 最後に生成した世代の画像を保存 /////
    //////////////////////////////////////////
    std::string file_name = "./generated_image/gen";
    file_name += std::to_string(this->current_gen);
    file_name += ".png";

    SDL_Surface* surface = this->scored_list.front().second->convertToSurface();
    IMG_SavePNG(surface, file_name.c_str());
    SDL_FreeSurface(surface);

    ///////////////////////////////
    ///// 生成したImageの破棄 /////
    ///////////////////////////////
    delete this->original_img;
    for (auto img : this->generated_img_list)
    {
        delete img;
    }
    printLog("Delete generated images", true);

    //////////////////////////////////
    ///// Window・Rendererの破棄 /////
    //////////////////////////////////
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    printLog("Destroy window and renderer", true);

    /////////////////////////////
    ///// SDL関連の終了処理 /////
    /////////////////////////////
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
    IMG_SavePNG(original_img_surface, "./generated_image/original.png");
    printLog("Scale original image", true);

    this->original_img = Image::convertoToImage(original_img_surface);

    SDL_FreeSurface(original_img_surface);
    SDL_FreeSurface(surface);
}

Image* GeneticImageGenerator::createRandomImage()
{
    Image* img = new Image(this->w, this->h);    // 真っ黒な画像を作成
    std::random_device rnd;                      // 乱数生成機

    ////////////////////////////////////////
    ///// 各ピクセルをランダム色で塗る /////
    ////////////////////////////////////////
    for (int y = 0; y < this->h; ++y)
    {
        for (int x = 0; x < this->w; ++x)
        {
            drawPixel(img, x, y, rnd() % 255, rnd() % 255, rnd() % 255);
        }
    }

    return img;
}

void GeneticImageGenerator::createFirstGen()
{
    std::cout << std::endl << "--- Generate generation 1 ---" << std::endl;

    ////////////////////////////////
    ///// プログレスバーの設定 /////
    ////////////////////////////////
    pbar::pbar bar(ELITE_NUM, "Generate ");
    bar.enable_recalc_console_width(1);
    bar.init();

    for (int i = 0; i < ELITE_NUM; ++i)
    {
        Image* img = this->createRandomImage();
        this->generated_img_list.push_back(img);

        bar.tick();
    }

    ////////////////////////////////////////////
    ///// 生成した全ての画像のスコアを計算 /////
    ////////////////////////////////////////////
    // プログレスバーの設定
    pbar::pbar bar_calc(this->generated_img_list.size(), "Calculate");
    bar_calc.enable_recalc_console_width(1);
    bar_calc.init();

    for (Image* img : this->generated_img_list)
    {
        std::pair<double, Image*> score(img->calcScore(this->original_img), img);
        this->scored_list.push_back(score);

        bar_calc.tick();
    }
    std::sort(this->scored_list.begin(), this->scored_list.end());    // スコアの昇順に並べ替え

    //////////////////////////////////
    ///// 上位画像のスコアを出力 /////
    //////////////////////////////////
    for (int i = 0; i < ELITE_NUM; i++)
        std::cout << "Score[" << i + 1 << "]: " << this->scored_list.at(i).first << std::endl;

    ////////////////////////////
    ///// 最高スコアを記録 /////
    ////////////////////////////
    this->ofs_record_file << "第1世代" << ',' << this->scored_list.front().first << std::endl;

    /////////////////////////////
    ///// 第1世代を保存する /////
    /////////////////////////////
    SDL_Surface* surface = this->scored_list.front().second->convertToSurface();
    IMG_SavePNG(surface, "./generated_image/gen1.png");
    SDL_FreeSurface(surface);
}

void GeneticImageGenerator::generateNextGen()
{
    std::cout << std::endl << "--- Generate generation " << ++current_gen << " ---" << std::endl;

    std::vector<Image*> new_gen;    // 次の世代の画像リスト

    ////////////////////////////////
    ///// スコア上位数個を選抜 /////
    ////////////////////////////////
    Image* elite[ELITE_NUM];
    for (int i = 0; i < ELITE_NUM; ++i)
    {
        elite[i] = this->scored_list.at(i).second;
        Image* elite_img = new Image(*elite[i]);
        new_gen.push_back(elite_img);
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ///// 上位画像数個からランダムにピクセルデータをコピーし，新たな画像を生成する /////
    ////////////////////////////////////////////////////////////////////////////////////
    std::random_device rnd;    // 乱数生成機

    ////////////////////////////////
    ///// プログレスバーの設定 /////
    ////////////////////////////////
    pbar::pbar bar(this->num_per_gen - ELITE_NUM, "Generate ");
    bar.enable_recalc_console_width(1);
    bar.init();

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

        bar.tick();    // プログレスバーの更新
    }

    ////////////////////////////////////////////
    ///// 生成した全ての画像のスコアを計算 /////
    ////////////////////////////////////////////
    // プログレスバーの設定
    pbar::pbar bar_calc(new_gen.size(), "Calculate");
    bar_calc.enable_recalc_console_width(1);

    this->scored_list.clear();
    for (Image* img : new_gen)
    {
        std::pair<double, Image*> score(img->calcScore(this->original_img), img);
        this->scored_list.push_back(score);

        bar_calc.tick();
    }
    std::sort(this->scored_list.begin(), this->scored_list.end());    // スコアの昇順に並べ替え

    ////////////////////////////////
    ///// 最新の世代を更新する /////
    ////////////////////////////////
    std::for_each(this->generated_img_list.begin(), this->generated_img_list.end(), [](Image* img)
        {
            delete img;
        });
    this->generated_img_list = new_gen;

    //////////////////////////////////
    ///// 上位画像のスコアを出力 /////
    //////////////////////////////////
    for (int i = 0; i < ELITE_NUM; i++)
        std::cout << "Score[" << i + 1 << "]: " << this->scored_list.at(i).first << std::endl;

    ////////////////////////////
    ///// 最高スコアを記録 /////
    ////////////////////////////
    this->ofs_record_file << "第" << this->current_gen << "世代" << ',' << this->scored_list.front().first << std::endl;

    //////////////////////////////////
    ///// 10世代ごとに画像を保存 /////
    //////////////////////////////////
    if (this->current_gen % 10 == 0)
    {
        std::string file_name = "./generated_image/gen";
        file_name += std::to_string(this->current_gen);
        file_name += ".png";

        SDL_Surface* surface = this->scored_list.front().second->convertToSurface();
        IMG_SavePNG(surface, file_name.c_str());
        SDL_FreeSurface(surface);
    }
}
