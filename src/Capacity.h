#pragma once
#include <cstdint>
#include <limits>
#include <stdexcept>

inline uint64_t CheckedAdd(uint64_t left, uint64_t right)
{
    if (right > std::numeric_limits<uint64_t>::max() - left)
        throw std::overflow_error("Serialized size overflow");
    return left + right;
}

inline uint64_t CheckedMultiply(uint64_t left, uint64_t right)
{
    if (right != 0 && left > std::numeric_limits<uint64_t>::max() / right)
        throw std::overflow_error("Carrier size overflow");
    return left * right;
}

inline uint64_t CarrierCapacityBytes(uint64_t carrierElementCount,
                                    uint8_t bytesPerElement, uint8_t embeddableBitsPerElement)
{
    if (bytesPerElement == 0 || embeddableBitsPerElement > unsigned(bytesPerElement) * 8)
        throw std::invalid_argument("Bit depth exceeds carrier element width");
    return CheckedMultiply(carrierElementCount, embeddableBitsPerElement) / 8;
}
