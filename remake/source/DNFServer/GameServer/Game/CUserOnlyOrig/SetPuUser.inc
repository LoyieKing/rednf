// ---------------------------------------------------------------------------
// 4. CUser::SetPuUser @0x08692b58
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig IDENTICAL] ORIG 0x08692b58：**P211 档③新翻正位（本波）**——uchar 成员
// setter 的 store 序由「RHS 左值类型」决定：4.4 对 uchar 字段 + bool 右值恒 RHS-first，
// 把目标改 **bool 同址视角**（CUser.h:2350-2354 匿名 union `m_bPuUser796d8`，Main 代落）
// 即翻为 ORIG 的 LHS-first 5 条形（探针 /tmp/pu5.cpp u0：`mov %al,-0x4; mov 0x8,%eax;
// movzbl -0x4,%edx; mov %dl,0x796d8(%eax)`），逐条与 ORIG 全等；uchar 视角保留，
// Inter_LoadPuUserCheck 消费点零改动。
void CUser::SetPuUser(bool flag)
{
    // ORIG 0x08692b58：`mov %al,-0x4; mov 0x8,%eax; movzbl -0x4,%edx; mov %dl,0x796d8(%eax)`
    // bool=bool 直落形（同址 bool 视角，P211 档③同型化）。
    // [b28d-solo-CUserOnlyOrig P0] ORIG 无 this 槽（0x8(%ebp) 直读）⇒ 原码无具名
    // `self` 局部；旧写法 `CUser* self = this;` 多 store+load 且帧 0x14→0x4 漂移。
    // [b28d-solo-CUserOnlyOrig P211 档③] 直接 `m_field796d8 = flag`（uchar=bool 异型）
    // 恒 RHS-first；`*(bool*)&m_field796d8` 视图反多一条 `add $0x796d8,%eax`
    // （大偏移取址不可折 disp）⇒ 用头内同址 bool 视角名直访问（单条 disp store）。
    m_bPuUser796d8 = flag;
}