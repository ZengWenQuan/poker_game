#include <cmath>

extern "C" float __powf_finite(float x, float y)
{
    return std::pow(x, y);
}

extern "C" float __expf_finite(float x)
{
    return std::exp(x);
}
