#pragma once

#include <cmath>

class Saturation
{
public:
    void setDrive(float d) { drive = d; }

    float process(float input)
    {
        const float x = input * (1.0f + drive * 3.0f);
        return std::tanh(x) * (1.0f / std::tanh(1.0f + drive * 3.0f));
    }

private:
    float drive = 0.0f;
};
