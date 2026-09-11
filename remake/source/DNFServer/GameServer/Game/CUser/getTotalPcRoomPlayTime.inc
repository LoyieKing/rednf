// ---- 四个 CUser 字段访问器（CacheCharacterMemory 域；ORIG W 弱符号）----
unsigned int CUser::getTotalPcRoomPlayTime() const   // 0x08110e10：*(int*)(this+0x8eba0)；b28d-Main 代落·S350 返回 unsigned int
{
    return m_totalPcRoomPlayTime;
}