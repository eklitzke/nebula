#ifndef CHANNEL_HH_
# define CHANNEL_HH_

# include <cmath>

# include "memory/mmu.hh"
# include "sound/constants.hh"

class Channel {
public:
    Channel(const MMU& mmu);
    virtual ~Channel();

    virtual void fill_stream(int16_t* stream, int len) = 0;

protected:
    void freq_to_stream(int16_t* stream, int len, int freq);

protected:
    const MMU& mmu_;

private:
    int count_stream_;
};

#endif // !CHANNEL_HH_
