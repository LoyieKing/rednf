// [b28-L14] P74/P26：ORIG C1ERKS_ 0x86d296a / aSERKS_ 0x86d2a1a 是编译器隐式合成
// （逐成员、跳过尾部对齐空洞），`= default` 外联定义既保持 W 符号发射又不再搬 pad。
TownAreaScript::TownAreaScript(const TownAreaScript& other) = default;
