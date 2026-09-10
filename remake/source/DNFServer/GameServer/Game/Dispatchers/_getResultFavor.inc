unsigned short Dispatcher_GiveGiftToNPC::_getResultFavor(
    CUser const* user, CNPCDynamicInfo const* npcInfo,
    stNPCRelationship_t const& relation, CItem const* item,
    unsigned int count, short addFavor, unsigned short baseFavor,
    ENUM_NPC_FAVOR_CHANGE_CONST& changeConst) const
{
    // ORIG 0x0821e486：帧 0x18；两形参 dead spill `mov %dx,-0x14` / `mov %ax,-0x18`，
    // 结果经 2 字节中转槽 -0x2（`mov %ax,-0x2; movzwl -0x2,%eax`）。
    // 技巧31：dead pad 把帧撑到 0x18 并使两 spill 落到 -0x14/-0x18；
    // 具名 unsigned short 中转（技巧8/13：末个声明占最近 ebp 槽 -0x2）。
    char framePad[8];                                     // 撑帧
    (void)framePad;
    const unsigned short sum =
        static_cast<unsigned short>(baseFavor + addFavor);
    return sum;
