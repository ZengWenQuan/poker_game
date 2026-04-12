#include <cmath>

extern "C" float __powf_finite(float x, float y)
{
    // 某些预编译库在 Linux 下会引用 glibc 的 finite 变体符号，这里做最小兼容桥接。
    return std::pow(x, y);
}

extern "C" float __expf_finite(float x)
{
    // 同上，提供 exp 的 finite 版本以避免链接缺失。
    return std::exp(x);
}
