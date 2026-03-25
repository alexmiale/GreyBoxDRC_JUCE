#pragma once

#include <cmath>
#include <algorithm>

/**
 * Level Detector — Single one-pole smoothing filter.
 *
 * Operates on the gain-reduction envelope normalised to [0, 1]:
 *   0 = no reduction,  1 = maximum reduction (80 dB).
 *
 * One-pole filter (Giannoulis et al. 2012, Eq. 3):
 *   y[n] = ? · y[n-1] + (1 - ?) · x[n]
 *
 * ? = exp(-1 / (? · fs))     (Eq. 5)
 *
 * Config 1 uses a single learned time-constant ? for both
 * attack and release (no switching behaviour).
 */
class GainSmooth
{
public:
    GainSmooth();
    ~GainSmooth() = default;

    /** Prepare the filter for a given sample rate. */
    void prepare(double sampleRate);

    /** Set the learned time-constant in seconds (0 < ? < 1). */
    void setTimeConstant(float tauSeconds);

    /** Reset the filter state (call on transport reset, etc.). */
    void reset();

    /** Process one normalised gain-reduction sample.
     *  Input and output are in the range [0, 1]. */
    float processSample(float gcNorm);

private:
    void updateCoefficient();

    double fs = 44100.0;
    float  tau = 0.01f;   // learned time-constant (seconds)
    float  alpha = 0.0f;    // one-pole coefficient
    float  yPrev = 0.0f;    // filter state
};