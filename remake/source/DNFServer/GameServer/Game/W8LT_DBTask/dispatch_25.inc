// ORIG 0x8417b6e：恒返 1（mov $0x1; ret；与 DB_Stat* 族其他成员不同，
// 无基类 dispatch 前置检查、无 GetOutBuffer、不调 LogWrite）。
bool DB_StatCoinUse::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}