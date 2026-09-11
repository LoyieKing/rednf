// ORIG 0x082a4cea：ctor 体内只有 Reset()；三组 Inven_Item[4][2] 与
// GameResultType[4] 的构造全部由编译器隐式生成（含 EH 反向析构 landing pad），
// 不存在 placement-new（ORIG 无 _ZnwjPv 调用点）。
BattleData::BattleData()
{
    Reset();
}
