#ifndef LENGTH_HH_
# define LENGTH_HH_

# include <stdint.h>

# include "../../memory/registers/nr52.hh"
# include "../../memory/registers/nrx1.hh"
# include "../../memory/registers/nrx4.hh"
# include "../../utils/converter.hh"
# include "filter.hh"

class Length : public Filter {
public:
    Length(int num, NR52Proxy& nr52, const NRX1Proxy& nrx1, const NRX4Proxy& nrx4);

    void reload();
    int32_t filter(int32_t frequency);

private:
    Converter tick_;
    uint8_t length_;

    int num_;
    NR52Proxy& nr52_;
    const NRX1Proxy& nrx1_;
    const NRX4Proxy& nrx4_;
};

#endif // !LENGTH_HH_
