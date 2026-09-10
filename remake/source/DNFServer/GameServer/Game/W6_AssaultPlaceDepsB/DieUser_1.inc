void CAssaultPlace::DieUser(CUser* user, int killerIdx, MSG_DIE_ASSAULT_PLAYER& msg)
{
    // b28 BL6: P0——ORIG 切片无 idx 槽往返，返回值寄存器直传出参槽；
    // 具名局部引入 -0xc 双 store/load +帧差。嵌套实参复现 ORIG 0x82ed3fe 形态。
    DieUser(_GetUserIndex(user, true), killerIdx, msg);
