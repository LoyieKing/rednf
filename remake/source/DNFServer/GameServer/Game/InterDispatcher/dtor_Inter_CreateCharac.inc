Inter_CreateCharac::~Inter_CreateCharac() {}
// [b28d-lane2] verdict=DIFF ndiff=380（ORIG 0x084bee06）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84bee06）：本轮修 P167 六分支 put_byte
// （补 4 调用点）+ m_serverEnvir.m_serverGroup 字段读（消 get_server_group EXTRA）；
// 余 N18a 单段 regmove + N82/P112 帧槽平移（大函数）。
