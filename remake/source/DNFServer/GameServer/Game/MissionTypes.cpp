#include <bitset>
#include "MissionTypes.h"

#include <bitset>


bool is_Victory_in_a_row(std::bitset<32> bits, short victory)
{
    if (victory <= 0) return true;
    int count = 0;
    for (int i = 0; i < 32; ++i) {
        if (bits[i]) {
            if (++count >= victory) return true;
        } else {
            count = 0;
        }
    }
    return false;
}

