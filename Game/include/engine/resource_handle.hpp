#pragma once
#include <cstdint>

#pragma push_macro("max")
#undef max

template <typename T>
class ResourceHandle
{
public:
    ResourceHandle() : id(std::numeric_limits<uint32_t>::max()) { }
    ResourceHandle(uint32_t id, uint8_t magicValue = 0) : id((id << 2) | magicValue) {}

    
    [[nodiscard]] uint32_t Id() const { return id >> 2; }
    [[nodiscard]] uint8_t MagicValue() const { return id & 0xff; }
    [[nodiscard]] uint8_t Value() const { return id; }
    [[nodiscard]] bool Valid() const { return id != std::numeric_limits<uint32_t>::max(); };

private:
    uint32_t id;
};

#pragma pop_macro("max")
