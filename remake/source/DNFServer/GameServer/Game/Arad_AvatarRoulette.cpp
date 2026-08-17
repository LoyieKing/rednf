// Arad_AvatarRoulette — 原始工程 localjapan/Arad_AvatarRoulette.cpp 的复刻 TU。
// 收录 AvatarCoin 的「使用侧」方法（ORIG 参照 docs/class_func_reports/AvatarCoin.md）：
//   Use（本批次）；BuyProcess/GetPackage/SaveToDB/SendSyncPacket/
//   HistoryLog::AddLog 依赖 CUser/Stream/MsgQueue/HistoryLog，随域批次补充。
// 注意：Use 调用 IsUsable 必须跨 TU（PLT 调用 → ORIG 帧 sub $0x18），
// 与 AvatarCoin.cpp 保持分 TU。

class AvatarCoin
{
public:
    unsigned int Use();
    char IsUsable() const;

    unsigned int m_coin;  // +0x00
};

unsigned int AvatarCoin::Use()
{
    // register 绑定使 -O0 下分支条件直接用 test %al 取字节（ORIG 形态），
    // 不产生 setne 提升。
    register char r __asm__("al") = IsUsable();
    if (r != 0)
    {
        m_coin = m_coin + -1;
        return 1;
    }
    return 0;
}
