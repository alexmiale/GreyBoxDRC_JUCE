#include "GainSmooth.h"

GainSmooth::GainSmooth()
{
    updateCoefficient();
}

void GainSmooth::prepare(double sampleRate)
{
    fs = sampleRate;
    updateCoefficient();
    reset();
}

void GainSmooth::setTimeConstant(float tauSeconds)
{
    tau = std::clamp(tauSeconds, 1e-6f, 1.0f);
    updateCoefficient();
}

void GainSmooth::reset()
{
    yPrev = 0.0f;
}

float GainSmooth::processSample(float gcNorm)
{
    // One-pole filter: y[n] = ?·y[n-1] + (1-?)·x[n]
    float y = alpha * yPrev + (1.0f - alpha) * gcNorm;
    yPrev = y;
    return y;
}

void GainSmooth::updateCoefficient()
{
    // ? = exp(-1 / (? · fs))  — guarantees |?| < 1 (stable)
    alpha = std::exp(-1.0f / (tau * static_cast<float> (fs)));
}