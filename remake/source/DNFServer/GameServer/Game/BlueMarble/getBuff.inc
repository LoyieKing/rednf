// [b28d-solo-BlueMarble AE] P251/P261+P305：原 if/goto 链使 4.4 每次重载形参槽
// （多 2 载入 + 3 nop 岛，53/58）；改 `switch((int)type)`（窄枚举必须 (int) 化，
// 否则 case 1/2 被 -fstrict-enums 静默删）后 type 单次载入复用 eax，53=53 ndiff 0。
// ORIG 0x080d990e
int BlueMarble::getBuff(CUser* user, BlueMarbleBuffType::T type)
{
    if (user == 0)
    {
        return 0;
    }
    int seat = getUserSlot(user);
    if (seat == -1)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__,
                              0x4c7, "BlueMarble::getBuff slot(%d)", seat);
        return 0;
    }
    // ORIG 0x080d9979 `mov 0x10(%ebp),%eax` 只载入一次并全程复用 eax：
    // 0x080d997c `cmp$1; je→getGoldBuff@999e`、0x080d9984 `cmp$2;
    // je→getDropBuff@99b2`、0x080d9988 `test; jne→mov$0@99c6`，fall
    // type==0→getExpBuff@9997（P251/P261 无槽 cmp 链 ⇒ 原码是 switch）。
    // P305：BlueMarbleBuffType 是窄枚举（仅 T_0=0），必须 `switch((int)type)`
    // 否则越界 case 1/2 被 -fstrict-enums 静默删除。
    switch ((int)type)
    {
    case 0:
        return getExpBuff(seat);
    case 1:
        return getGoldBuff(seat);
    case 2:
        return getDropBuff(seat);
    default:
        return 0;
    }
}