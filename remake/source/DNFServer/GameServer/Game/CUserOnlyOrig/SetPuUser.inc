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
