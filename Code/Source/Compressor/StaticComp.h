#pragma once

#include <cmath>
#include <algorithm>

/**
 * Static Gain Computer — Soft-knee compression curve.
 *
 * Maps input level (dB) to a gain-reduction value (dB ? 0).
 * Designed so an MLP can be dropped in later to predict
 * threshold / ratio / knee-width from conditioning data.
 *
 * Soft-knee equation (Giannoulis et al. 2012, Eq. 1):
 *   if   2(xdB - T) < -W :  ydB = xdb
 *   else if  2|xdB - T| <= W : xdB + [(1/R - 1)(xdB - T + W/2)^2] / 2W
 *   else 2(xdB - T) > W : T + (xdB - T) / R
 *
 * Gain reduction:  gc = ydB - xdB   (always <= 0)
 */
class StaticComp
{
public:
    StaticComp();
    ~StaticComp() = default;

    /** Call once before processing to set the curve parameters.
     *  These will eventually come from an MLP at runtime. */
    void setParams(float thresholdDB, float ratio, float width);

    /** Returns the gain-reduction in dB for a given input level in dB.
     *  Result is in the range [-80, 0]. */
    float processDB(float xdB) const;

    float getThreshold() const { return threshold; }
    float getRatio()     const { return ratio; }
    float getWidth()     const { return width; }

    // ?? Future MLP hook ??????????????????????????????????????
    // Replace or augment setParams() with:
    //   void predictParams (const float* conditioningData, int numConditions);
    // which runs an MLP forward pass and internally calls setParams().
    // The processDB() function stays the same.
    // ?????????????????????????????????????????????????????????

private:
    float threshold;  // dB, valid range (-80, 0)
    float ratio;      // > 1
    float width;      // > 0
};