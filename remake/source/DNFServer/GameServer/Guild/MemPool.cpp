// df_guild_r — MemPool 显式实例化点
// ORIG 仅实例化 MemPool<CAdvertisementExposeInfo>（弱符号 7 个：
// ctor C1/C2 0x080fe010、alloc 0x080fe038、free(void*,unsigned) 0x080fe188、
// free(void*) 0x080fe1ce、dtor D1/D2 0x080ff966、headOfFreeList_ 0x093feaec）。
// 模板完整定义在 MemPool.h（使用方 TU 隐式实例化自身池类型）。
#include <stdio.h>
#include <stdlib.h>

#include "MemPool.h"
#include "CInGameAdvertisementManager.h"

template class MemPool<CAdvertisementExposeInfo>;
