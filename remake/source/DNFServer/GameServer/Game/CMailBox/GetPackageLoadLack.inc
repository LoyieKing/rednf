// ORIG 0x08557608 (W)：逐项拷贝 lack 列表前 n 项到 out（越界即停）
void CMailBox::GetPackageLoadLack(unsigned int* out, unsigned int n)
{
    for (unsigned int i = 0; i < n && i < m_loadLackList.size(); ++i)
        out[i] = m_loadLackList[i];
}