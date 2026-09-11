// ORIG 0x0858053e T：
//   getMood()==mood 直接 false；否则 while(changeMood(true)!=mood) 空循环，返回 true。
bool CNPCDynamicInfo::forceChangeMood(ENUM_NPC_MOOD mood)
{
    if (getMood() == mood)
        return false;
    while (changeMood(true) != mood)
        ;
    return true;
}