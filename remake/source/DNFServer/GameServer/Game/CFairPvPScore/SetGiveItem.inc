// ---- W 内联组（ORIG 地址见头文件注释，逐一反汇编实证） ---------------------
void CFairPvPScore::SetGiveItem(int v)                 // 0x8234f74
{
    m_giveItem = (unsigned char)v;
    alter();                                           // ORIG call CData::alter
}