CUser::~CUser()
{
    // 2026-09-10 gA（A 组）：**实现体不在本 .inc**（本文件 37 字节，仅函数头 +
    //   m_powerUp=0 开头），主体在聚合器 CUser.cpp ⇒ 本组写域外，未动。
    //   判定依据：ORIG 0x0864ef9c（676 vs 678，差 2 条为帧/槽分配残差）。
    m_powerUp = 0;
