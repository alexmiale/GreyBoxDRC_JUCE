#include "MakeUp.h"

MakeUp::MakeUp()
    : gainDB(0.0f),
    gainLinear(1.0f)
{
}

void MakeUp::setGainDB(float newGainDB)
{
    gainDB = newGainDB;
    gainLinear = std::pow(10.0f, gainDB / 20.0f);
}

float MakeUp::processSample(float x) const
{
    return x * gainLinear;
}