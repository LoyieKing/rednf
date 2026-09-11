CMissionList_Charac::CMissionList_Charac()
{
    // ORIG C1 @0x832af90：仅隐式成员构造（MissionInfo[36] 逐个清零 +
    // bitset<256> 默认构造，不额外 reset；显式清零循环与 reset 均为
    // NEW 冗余，移除以对齐 ORIG 字节）。
}