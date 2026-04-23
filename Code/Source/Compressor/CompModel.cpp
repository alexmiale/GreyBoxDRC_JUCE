#include "CompModel.h"

CompModel::CompModel() = default;

void CompModel::prepare(double sampleRate)
{
    gainSmooth.prepare(sampleRate);
}

void CompModel::reset()
{
    gainSmooth.reset();
}

void CompModel::loadModel(std::ifstream& jsonStream) {
    nlohmann::json fullJson;
    jsonStream >> fullJson;

    auto& dense1 = model.get<0>(); // input (1) to 20
    RTNeural::torch_helpers::loadDense<float>(fullJson, "model.HCNet.0.", dense1);

    auto& dense2 = model.get<2>(); // 20 to 20
    RTNeural::torch_helpers::loadDense<float>(fullJson, "model.HCNet.2.", dense2);

    auto& output = model.get<4>(); // 20 to output (3)
    RTNeural::torch_helpers::loadDense<float>(fullJson, "model.HCNet.4.", output);
}

// ?? Parameter setters ????????????????????????????????????????

void CompModel::setThreshold(float thresholdDB)
{
    staticComp.setParams(thresholdDB, staticComp.getRatio(), staticComp.getWidth());
}

void CompModel::setRatio(float ratio)
{
    staticComp.setParams(staticComp.getThreshold(), ratio, staticComp.getWidth());
}

void CompModel::setWidth(float width)
{
    staticComp.setParams(staticComp.getThreshold(), staticComp.getRatio(), width);
}


void CompModel::setAllCompFromModel(const float* inputs) {
    float thresholdDB = inputs[0] * -80.0f;         // [-80, 0] dB
    float ratio = inputs[1] * 29.0f + 1.0f;       //  [1, 30]
    float kneeWidth = inputs[2] * 30.0f;          //  [0, 30]

    staticComp.setParams(thresholdDB, ratio, kneeWidth);
}

void CompModel::setTimeConstant(float tauSeconds)
{
    gainSmooth.setTimeConstant(tauSeconds);
}

void CompModel::setMakeUpGainDB(float dB)
{
    makeUp.setGainDB(dB);
}

// ?? Processing ???????????????????????????????????????????????

float CompModel::processSample(float x)
{
    // 1. Convert to dB  (use |x| to handle negative samples)
    float absX = std::fabs(x);
    float xdB = (absX > kFloor)
        ? 20.0f * std::log10(absX)
        : kMinDB;

    // Clamp to valid range
    xdB = std::max(xdB, kMinDB);

    // 2. Static gain computer ? gain reduction in dB (? 0)
    float gcDB = staticComp.processDB(xdB);

    // 3. Normalise to [0, 1]  (0 dB reduction ? 0,  -80 dB ? 1)
    float gcNorm = -gcDB / 80.0f;
    gcNorm = std::clamp(gcNorm, 0.0f, 1.0f);

    // 4. Smooth with one-pole filter
    float gsNorm = gainSmooth.processSample(gcNorm);

    // 5. Scale back to dB
    float gsDB = -gsNorm * 80.0f;

    // 6. Convert gain to linear
    float gLin = std::pow(10.0f, gsDB / 20.0f);

    // 7. Apply gain to input
    float compressed = x * gLin;

    // 8. Apply make-up gain
    return makeUp.processSample(compressed);
}

void CompModel::processBlock(float* buffer, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
        buffer[i] = processSample(buffer[i]);
}

const float* CompModel::forward(float input) {
    model.forward(&input);
    return model.getOutputs();
}