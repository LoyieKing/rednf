// 迁移自 GameStubs.cpp seriaRoom_AniDeco 桩（batch-6）。
// 构造：将两个 short 字段清零（ORIG W 0x0822ee18）。
// 符号：_ZN17seriaRoom_AniDecoC1Ev（弱符号）。

class seriaRoom_AniDeco
{
public:
    seriaRoom_AniDeco();

    short m_field0; // +0x00
    short m_field2; // +0x02
};

seriaRoom_AniDeco::seriaRoom_AniDeco()
{
    m_field0 = 0;
    m_field2 = 0;
}