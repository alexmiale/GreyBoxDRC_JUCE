#pragma once

#include <cmath>

/**
 * Make-Up Gain — Static (constant) gain applied to the
 * compressed output signal to restore overall loudness.
 *
 * For Config 1 this is a single learned scalar in dB,
 * converted to a linear multiplier.
 */
class MakeUp
{
public:
    MakeUp();
    ~MakeUp() = default;

    /** Set the make-up gain in dB. */
    void setGainDB(float gainDB);

    /** Get the current make-up gain in dB. */
    float getGainDB() const { return gainDB; }

    /** Apply the make-up gain to a single sample (linear domain). */
    float processSample(float x) const;

private:
    float gainDB = 0.0f;
    float gainLinear = 1.0f;
};