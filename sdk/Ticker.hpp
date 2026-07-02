#pragma once

class Ticker {
public:
    Ticker(unsigned ticks_to_rise): _ticks_to_rise(ticks_to_rise) { }
    virtual ~Ticker() = default;

    inline void reset() { _ticks = 0; }
    // On a rise the count restarts at 1: the wrapping tick is itself the first
    // tick of the next interval, so risen() fires exactly every ticks_to_rise ticks.
    inline void tick() { _ticks = (_ticks >= _ticks_to_rise ? 1 : _ticks + 1); }

    bool risen() { return _ticks >= _ticks_to_rise; }

private:
    const unsigned _ticks_to_rise;
    unsigned _ticks = 0;
};