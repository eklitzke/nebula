#include "gpu.hh"

GPU::GPU(MMU& mmu)
    : mmu_ (mmu)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTTHREAD);
    this->screen_ = SDL_SetVideoMode(WIDTH * COEF, HEIGHT * COEF, 32,
                                     SDL_DOUBLEBUF | SDL_HWSURFACE);
    SDL_WM_SetCaption("nebula", 0);
    SDL_FillRect(this->screen_, 0, SDL_MapRGB(this->screen_->format, 0, 0, 0));

    // Colors
    this->colors_[0] = SDL_MapRGB(this->screen_->format, 255, 255, 255);
    this->colors_[1] = SDL_MapRGB(this->screen_->format, 192, 192, 192);
    this->colors_[2] = SDL_MapRGB(this->screen_->format, 96, 96, 96);
    this->colors_[3] = SDL_MapRGB(this->screen_->format, 0, 0, 0);
}

GPU::~GPU()
{
    SDL_Quit();
}

void GPU::do_cycle(uint8_t cycles)
{
    this->wait_count -= cycles;
    if (this->wait_count < 0)
    {
        switch (this->mmu_.STAT.mode.get())
        {
        case LCDC_MODE_0: // H-Blank
            if (this->mmu_.LY.get() == this->mmu_.LYC.get())
            {
                this->mmu_.STAT.coin_int.set(1);
                this->mmu_.IF.set(this->mmu_.IF.get() | 0x2);
            }
            else
                this->mmu_.STAT.coin_int.set(0);
            if (this->mmu_.LCDC.LDE.get() && this->mmu_.LY.get() < 144)
                this->draw_line();
            this->wait_count = 207;
            this->mmu_.LY.set(this->mmu_.LY.get() + 1);
            if (this->mmu_.LY.get() < 160)
                this->mmu_.STAT.mode.set(LCDC_MODE_2);
            else
                this->mmu_.STAT.mode.set(LCDC_MODE_1);
            break;
        case LCDC_MODE_1: // V-Blank
            this->mmu_.STAT.mode.set(LCDC_MODE_1);
            this->wait_count = 4560; // Cycles for V-Blank.
            this->timer_.adjust();
            this->mmu_.IF.set(this->mmu_.IF.get() | 0x1);
            this->mmu_.LY.set(0);
            SDL_Flip(this->screen_);
            break;
        case LCDC_MODE_2:
            this->wait_count = 83;
            this->mmu_.STAT.mode.set(LCDC_MODE_3);
            break;
        case LCDC_MODE_3:
            this->wait_count = 175;
            this->mmu_.STAT.mode.set(LCDC_MODE_0);
            break;
        };
    }
}

void GPU::draw_line()
{
    uint8_t background[WIDTH] = {0};
    uint8_t real_y = this->mmu_.LY.get() + this->mmu_.SCY.get();
    uint8_t scx = this->mmu_.SCX.get();

    SDL_LockSurface(this->screen_);

    // Real background.
    if (this->mmu_.LCDC.BGD.get())
    {
        BGWTile bg_tile(this->mmu_, scx - (scx % 8), real_y);
        for (uint8_t x = 0; x < WIDTH; ++x)
        {
            uint8_t real_x = scx + x;
            if (real_x % 8 == 0)
                bg_tile = BGWTile(this->mmu_, real_x, real_y);
            background[x] = this->mmu_.BGP.C[bg_tile.color(real_x % 8)].get();
        }
    }

    // Window on background.
    if ((uint8_t) this->mmu_.LCDC.WDE.get() && this->mmu_.WY.get() <= real_y)
    {
        uint8_t tmp = this->mmu_.WX.get() - 7;
        BGWTile win_tile(this->mmu_, tmp - (tmp % 8), real_y);
        for (uint8_t x = tmp; x < WIDTH; ++x)
        {
            if (x % 8 == 0)
                win_tile = BGWTile(this->mmu_, x, real_y);
            background[x] = this->mmu_.BGP.C[win_tile.color(x % 8)].get();
        }
    }

    // FIXME: Draw objects.

    // Drawing of the finale background.
    for (uint8_t x = 0; x < WIDTH; x++)
    {
        SDL_Rect rect = {
            (Sint16) (x *COEF), (Sint16) (this->mmu_.LY.get() * COEF),
            COEF, COEF
        };
        SDL_FillRect(this->screen_, &rect, this->colors_[background[x]]);
    }

    SDL_UnlockSurface(this->screen_);
}
