#ifndef CHANNEL_HH_
# define CHANNEL_HH_

# include <list>
# include <cmath>

# include "../../memory/mmu.hh"
# include "../../memory/registers/nrx4.hh"
# include "../constants.hh"
# include "../filters/filter.hh"

class Channel {
public:
    Channel(const RegisterProxy& nrx3, const NRX4Proxy& nrx4, const std::list<Filter*>& filters);
    virtual ~Channel();

    void fill_stream(int16_t* stream, unsigned int len);

protected:
    void update();

protected:
    int32_t frequency_;
    const std::list<Filter*> filters_;

    const RegisterProxy& nrx3_;
    const NRX4Proxy& nrx4_;
};

#endif // !CHANNEL_HH_
