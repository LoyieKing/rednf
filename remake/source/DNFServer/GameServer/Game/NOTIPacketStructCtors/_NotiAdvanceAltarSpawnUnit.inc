// ---- SpawnUnit ----
// ORIG 0x081346a6（0x31）：_SummonObject ctor(this+0x4) → memset(this,0,0x28)
NOTIPacketStruct::_NotiAdvanceAltarSpawnUnit::_NotiAdvanceAltarSpawnUnit()
{
    std::memset(this, 0, 0x28);
}