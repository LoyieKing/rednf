// ============================================================================
// CUser::exchange_interspace_expand(...)
// ORIG 0x0865f614 sz 0x19b。扩容槽位切换包装：ChannelEquipslotSwitch 开启时
// 目的域 3↔0x12 互换校验；双开关状态与请求一致才允许 change_equip_expand。
// 参数序按 ORIG mangled EiimiiimPicc：i,i,m,i,i,i,m,Pi,c,c。
// ============================================================================
int CUser::exchange_interspace_expand(int fromSpace, int fromSlot,
                                      unsigned long fromItemIdx, int addInfo,
                                      int toSpace, int toSlot,
                                      unsigned long toItemIdx, int* outSlot,
                                      char channelSwitchReq, char expandSwitchReq)
{
    int checkToSpace = toSpace;
    if (getCurChannelEquipslotSwitch() == 1)
    {
        if (toSpace == 3)
            checkToSpace = 0x12;
        else if (toSpace == 0x12)
            checkToSpace = 3;
    }

    if (!_checkItemMovable(this, (ENUM_ITEMSPACE)fromSpace, fromSlot, fromItemIdx,
                           (ENUM_ITEMSPACE)checkToSpace, toSlot, toItemIdx))
    {
        *outSlot = -1;
        return 0x15;
    }

    // b28d-49 @0x865f6ba：ORIG `call getCurChannelEquipslotSwitch; cmp -0x1c(%ebp),%al;
    // jne` = 8 位同型比较；getter 返 unsigned char + 形参 char 触发 4.4 32 位扩展
    //（movzbl+movsbl+cmp reg），调用点 (char) 收窄即复现（探针 g4）。
    if ((char)getCurChannelEquipslotSwitch() != channelSwitchReq
        || (char)getCurExpandEquipslotSwitch() != expandSwitchReq)
        return 1;

    int unusedSlot = 0;
    if (getCurChannelEquipslotSwitch() == 0 && getCurExpandEquipslotSwitch() == 0)
        return change_equip_expand(fromSpace, fromSlot, toSpace, toSlot, outSlot);

    if (getCurChannelEquipslotSwitch() == 1 && getCurExpandEquipslotSwitch() == 0)
        return change_equip_expand(fromSpace, fromSlot, checkToSpace, toSlot,
                                   outSlot);

    return 1;
}