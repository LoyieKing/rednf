NPC_Teleport::~NPC_Teleport() {}

// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x08283c5a
// [b28d-solo-GlobalData] 归零 AE 85=85（p167 11/11 CLEAR）：①各失败点 ORIG 为
// `mov $imm,%eax; jmp end` 直接物化 ⇒ 源码全 `return X;` 早返（旧 result 变量
// 汇聚形多 -0x1c 槽 + 4 条）；②pvp/expired 两处零槽短路物化用 P226 语句表达式
// `if (({ A || B; }))`（具名 bool 落槽 -0x16/-0xd）；③remain 声明在 if 外、
// 赋值在 if 体内（ORIG 0x08283cfb 在 isGMUser 之后）；④CUser.h is_equip_aura_avatar
// 返回 char→int（Main 代落，消调用点 movsbl）。
