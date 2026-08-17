// AvatarCoin — DNF 币（ORIG 0x817feXX..0x8180XXX；
// 权威参照 docs/class_func_reports/AvatarCoin.md）
// 布局：+0x00 unsigned int m_coin（ORIG 类尺寸 4，嵌于 Inven 区 +0x658）
// 原始工程中 AvatarCoin 方法分布在两个源文件：本文件 +
// Arad_AvatarRoulette.cpp（Use/BuyProcess/GetPackage/SaveToDB/
// SendSyncPacket）。Use 与 IsUsable 跨 TU 调用（PLT）是 ORIG 帧形态
// （sub $0x18）的来源，勿合并到同一 TU。
// 本批次实现 ctor/Set/Add/Get/IsUsable/Use；BuyProcess/GetPackage/
// SaveToDB/SendSyncPacket 依赖 CUser/Stream/MsgQueue/HistoryLog，
// 随 AvatarRoulette 域批次补充。

class AvatarCoin
{
public:
    AvatarCoin();
    void Set(unsigned int value);
    int Add(unsigned int value);
    unsigned int Use();
    unsigned int Get() const;
    char IsUsable() const;

    unsigned int m_coin;  // +0x00
};

AvatarCoin::AvatarCoin() : m_coin(0) {}

void AvatarCoin::Set(unsigned int value)
{
    m_coin = value;
}

int AvatarCoin::Add(unsigned int value)
{
    m_coin += value;
    return 1;  // ORIG 恒返回 1
}

unsigned int AvatarCoin::Get() const
{
    return m_coin;
}

char AvatarCoin::IsUsable() const
{
    return m_coin != 0;
}
