#pragma once

class Ticker {
public:
    Ticker(unsigned ticks_to_rise): _ticks_to_rise(ticks_to_rise) { }
    virtual ~Ticker() = default;

    inline void reset() { _ticks = 0; }
    inline void tick() { _ticks = (_ticks >= _ticks_to_rise ? 0 : _ticks + 1); }

    bool risen() { return _ticks >= _ticks_to_rise; }

private:
    const unsigned _ticks_to_rise;
    unsigned _ticks = 0;
};