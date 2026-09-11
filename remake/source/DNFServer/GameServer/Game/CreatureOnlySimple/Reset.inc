// ORIG _ZN13user_creature4CEgg5ResetEv T 0x08338f82（报告 Reset.md 全汇编转写）：
// +0x08/+0x0c/+0x10 清零、+0x14 字节清零（均为基类字段）。
void CEgg::Reset()
{
    m_creatureItemType = CREATURE_ITME_TYPE_EGG;  // +0x08
    m_uid = 0;               // +0x0c
    m_slotNo = 0;            // +0x10
    m_noCharge = 0;          // +0x14（字节）
}