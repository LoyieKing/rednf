// 频道类型 ∈ {4,6,5,7,8,9,10,13,14,15,16} 返回真，否则假。
// 调用侧（GetPvPChannelNumber）按 bool 语义 test %al 使用；定义者按 ORIG
// cmp/je 链 + mov $1/mov $0 分支形态实现即可（int/bool 返回符号相同）。
bool IS_PVP_CHANNEL(int channelType);
