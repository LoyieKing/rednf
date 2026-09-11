// b28d-solo-GmMacro verdict=AE ORIG 0x084a8802 orig/ours=35/36（1 条尾 nop，
// §6.1 对齐填充豁免），p167 CLEAR 4/4。修法：删除 stMacroData 用户空析构
// （GameMasterCmd.h 声明 + 本文件定义）——ORIG nm 0x084b5db4 W 0x5 系编译器隐式
// trivial D1，ORIG 正常路径无 dtor call、vector 清理走 _Destroy_aux<true>；我方旧
// 用户 dtor 使 4.4 判 non-trivial，正常路径多 1 次 dtor call 且 push_back 处布
// EH landing（~stMacroData ×2 + _Unwind_Resume + push esi/ebx），53 vs 35。
// 旧 b28 判词 N-EH/N83（P216 throw() 法不可用）作废：非平凡性源自我方用户 dtor，
// 删声明即复现 ORIG（P12/P71 判据；dtor 符号由 vector 使用点隐式发射，nm 仍在）。
void CMacro::Recording(int type, int sub, char* data, int len)
{
    stMacroData rec;
    rec.m_cmd = static_cast<unsigned char>(type);
    rec.m_sub = static_cast<unsigned char>(sub);
    if (data != 0 && len != 0)
    {
        rec.m_data = new char[len];                     // ORIG operator new[](unsigned)
        memcpy(rec.m_data, data, len);
        rec.m_len = len;
    }
    m_data.push_back(rec);
}