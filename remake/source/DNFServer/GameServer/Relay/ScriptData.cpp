// df_relay_r — ScriptData（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <string.h>
#include <stdlib.h>

#include "ScriptData.h"

ScriptData::ScriptData()
{
    clear();
}

ScriptData::~ScriptData()
{
}

void ScriptData::clear()
{
    mRelayNum = 100;
    strncpy(mServerIpA, "192.168.0.8", 0x11);
    mPortTcp = 5555;
    mPortUdp = 5555;
    mReservedA = 0;
    mReservedB = 7950;
    strncpy(mServerIpB, "192.168.0.15", 0x11);
    mReservedC = 30300;
    mReservedD = 101;
    mFlag = true;
}

