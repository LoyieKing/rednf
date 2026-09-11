int Inter_SendSecuReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    int* packet = reinterpret_cast<int*>(data);      // 整包首槽指针视图（非成员访问）
    if (*packet == 0)
    {
        user->AddSecuRewardItem(static_cast<unsigned char>(8));
    }
    else
    {
        user->AddSecuRewardItem(static_cast<unsigned char>(0x10));
    }
    return 0;
}