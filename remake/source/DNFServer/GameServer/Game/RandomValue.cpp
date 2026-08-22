// Migrated from GameStubs.cpp CRandomValue stub (batch-5)。
// 保留原桩行为（空构造体 / operator() 固定返回 0），仅提供 ORIG 符号。
#include "RandomValue.h"

CRandomValue::CRandomValue(std::vector<int>&, int, int, int)
{
}  // ORIG W 0x085df7fe

int CRandomValue::operator()()
{
    return 0;
}  // ORIG W 0x085df826