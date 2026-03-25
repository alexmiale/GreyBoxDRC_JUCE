#include "StaticComp.h"

StaticComp::StaticComp()
    : threshold(-20.0f),
    ratio(4.0f),
    width(5.0f)
{
}

void StaticComp::setParams(float thresholdDB, float newRatio, float newWidth)
{
    threshold = std::clamp(thresholdDB, -80.0f, 0.0f);
    ratio = std::max(newRatio, 1.0f);
    width = std::max(newWidth, 0.0f);
}

float StaticComp::processDB(float xdB) const
{
    // Soft-knee compression curve
    float ydB;

    if (width < 1e-6f)
    {
        // Hard-knee fallback
        ydB = (xdB <= threshold)
            ? xdB
            : threshold + (xdB - threshold) / ratio;
    }
    else {
        if (2.0f * (xdB - threshold) < -width) {
            ydB = xdB; // below threshold and width window -- no compression
        }
        else if (2.0f * std::abs(xdB - threshold) <= width)
        {
            // above threshold but in knee width -- smoothed compression
            float t = (1.0f / ratio - 1.0f) * (xdB - threshold + width / 2.0f);
            ydB = xdB + (t * t) / (2.0f * width);
        }
        else {
            ydB = threshold + (xdB - threshold) / ratio; // above threshold and out of knee width -- normal compression
        }
    }
        

    // Gain reduction (always <= 0)
    float gc = ydB - xdB;

    // Clamp to valid range
    return std::clamp(gc, -80.0f, 0.0f);
}