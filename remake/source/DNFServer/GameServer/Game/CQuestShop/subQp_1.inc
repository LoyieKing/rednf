// ============================================================================
// ---- CQuestShop::subQp(unsigned char, unsigned char)  ORIG T 0x85eed2a
int CQuestShop::subQp(unsigned char status, unsigned char count)
{
    int cost = 0;                                 // ORIG movl $0x0,-0xc
    cost = findStatusQp(status);                  // +0x00..+0x33
    if (cost <= 0)
        return 1;                                 // +0x3b
    cost = static_cast<int>(count) * cost;        // imul +0x4a
    if ((getQp() - cost >> 31) != 0)              // shr $0x1f 负数判定
        return 0x16;                              // +0x5d
    subQp(cost);                                  // _ZN10CQuestShop5subQpEi
    return 0;                                     // +0xa1
}