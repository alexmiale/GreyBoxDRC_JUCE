#pragma once

#include "StaticComp.h"
#include "GainSmooth.h"
#include "MakeUp.h"

#include <cmath>
#include <algorithm>

#include "RTNeural/RTNeural.h"

/**
 * CompModel — Full grey-box compressor chain (Config Set 1).
 *
 * Signal flow (log-domain detector topology):
 *
 *   x[n]  ???  |·| ??? 20·log10(·) ??? xdB
 *                                          ?
 *              StaticComp (soft-knee)  ?????
 *                  ?  gc  (dB, ? 0)
 *                  ?
 *           normalise to [0,1]
 *                  ?
 *              GainSmooth (one-pole)
 *                  ?
 *           scale back to dB
 *                  ?
 *              10^(·/20)  ??? gLin
 *                  ?
 *   x[n] ????? × ??
 *                  ?
 *              MakeUp (static gain)
 *                  ?
 *                  ?
 *               y[n]
 */
class CompModel
{
public:
    CompModel();
    ~CompModel() = default;

    /** Call before processing begins. */
    void prepare(double sampleRate);

    /** Reset all internal state. */
    void reset();

    // ?? Parameter setters ????????????????????????????????????

    /** Static gain-computer parameters. */
    void setThreshold(float thresholdDB);
    void setRatio(float ratio);
    void setWidth(float width);
    /** takes output from model and processes data to be in correct range */
    void setAllCompFromModel(const float* inputs);

    /** Level-detector time-constant (seconds). */
    void setTimeConstant(float tauSeconds);

    /** Make-up gain (dB). */
    void setMakeUpGainDB(float dB);

    // ?? Processing ???????????????????????????????????????????

    /** Process a single sample. */
    float processSample(float x);

    /** Process a block of audio in-place. */
    void processBlock(float* buffer, int numSamples);

    // ?? Access to sub-modules (e.g. for UI readback) ?????????
    const StaticComp& getStaticComp() const { return staticComp; }
    const GainSmooth& getGainSmooth() const { return gainSmooth; }
    const MakeUp& getMakeUp()     const { return makeUp; }

    /** load the model weights into the model variable **/
    void loadModel(std::ifstream& jsonStream);

    /** initate forward pass for the model
        model returns 3 values for threshold, ratio, and knee width**/
    const float* forward(float input);

private:
    StaticComp staticComp;
    GainSmooth gainSmooth;
    MakeUp     makeUp;

    // Minimum dB floor to avoid log10(0)
    static constexpr float kMinDB = -80.0f;
    static constexpr float kFloor = 1e-10f;   // ~ -200 dBFS

    RTNeural::ModelT<float, 1, 3,
        RTNeural::DenseT<float, 1, 20>,
        RTNeural::ReLuActivationT<float, 20>,
        RTNeural::DenseT<float, 20, 20>,
        RTNeural::ReLuActivationT<float, 20>,
        RTNeural::DenseT<float, 20, 3>, 
        RTNeural::SigmoidActivationT<float, 3>> model;
};