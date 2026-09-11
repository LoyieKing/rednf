// ORIG _ZN13user_creature13CCreatureItem11GetNoChargeEv W 0x0833efd6（size 0x0e）：
// movzbl 0x14(%eax),%eax 直返。P9：返回 unsigned char（bool 会生成 setne 规范化）。
// 头内不再保留 inline 定义（无调用点则永不 emission → 仅 ORIG 缺口），外联产出符号。
bool CCreatureItem::GetNoCharge()
{
    return m_noCharge;
}