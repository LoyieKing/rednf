// ORIG W 0x086b086c：返回 +0x862 字节（任一 premium 定义需每日检查时为 1）。
bool CUserPremium::IsNeedCheckEveryday()
{
    return m_field862;
}