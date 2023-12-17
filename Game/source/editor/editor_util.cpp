#include "pch.h"
#include "editor/editor_util.hpp"

ImVec4 HexToVec4(unsigned hex)
{
    float s = 1.0f / 255.0f;
    return ImVec4(
        ((hex >> IM_COL32_A_SHIFT) & 0xFF) * s,
        ((hex >> IM_COL32_B_SHIFT) & 0xFF) * s,
        ((hex >> IM_COL32_G_SHIFT) & 0xFF) * s,
        ((hex >> IM_COL32_R_SHIFT) & 0xFF) * s);
}
