#include "sweep.hh"

FrequencySweep::FrequencySweep(int num, NR52Proxy& nr52, const NR10Proxy& nr10)
    : Filter(), num_ (num), nr52_ (nr52), nr10_ (nr10),
      tick_ (SAMPLE_RATE, 128), counter_ (0)
{}

int32_t FrequencySweep::filter(int32_t freq) {
    if (this->enabled_) {
        if (this->tick_.next() && (--this->counter_) == 0) {
            this->last_diff_ = this->diff_;
            this->diff_ *= (1 << this->sweep_shift_);
            logging::info("Sweep: %d -> %d", this->last_diff_, this->diff_);
        }
    }
    freq = this->compute_freq(freq);
    if (freq <= 0 || 2048 <= freq) {
        logging::info("Disable sound channel %d.", this->num_);
        this->nr52_.channel_on[this->num_ - 1].set(0);
    }
    return freq;
}

void FrequencySweep::reload() {
    this->tick_.reset();

    this->counter_ = Cycle<unsigned int>(this->nr10_.sweep_time.get());
    this->enabled_ = this->counter_.maximum() != 0;

    this->sweep_shift_ = this->nr10_.sweep_shift.get();
    this->way_ = (this->nr10_.sweep_way.get() ? FREQUENCYSWEEP_DEC : FREQUENCYSWEEP_INC);

    this->diff_ = 1;
    this->last_diff_ = 0;
}

int32_t FrequencySweep::compute_freq(int32_t freq) {
    if (!this->enabled_ || this->last_diff_ == 0)
        return freq;
    switch (this->way_) {
    case FREQUENCYSWEEP_INC:
        return freq / this->last_diff_ + freq / this->diff_;
    case FREQUENCYSWEEP_DEC:
        return freq / this->last_diff_ - freq / this->diff_;
    };
    logging::error("Sound sweep: WTF?");
    return 0;
}
