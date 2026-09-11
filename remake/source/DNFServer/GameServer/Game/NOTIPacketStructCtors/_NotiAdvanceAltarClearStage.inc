// ---- ClearStage ----
// ORIG 0x08134720（0x31）：_Stage ctor(this+0xd) → memset(this,0,0x1c)
NOTIPacketStruct::_NotiAdvanceAltarClearStage::_NotiAdvanceAltarClearStage()
{
    std::memset(this, 0, 0x1c);
}