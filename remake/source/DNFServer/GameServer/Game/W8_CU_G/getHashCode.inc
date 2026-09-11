// ORIG 0x084fa298：对角色列表首元素起 size()*0x14d1 字节做 MD5 摘要。
// b28d-solo-W8_CU_G: NEAR/N95 封顶 — ORIG 0x084fa298 md5_context 槽基 -0xe0 vs
// 重建 -0xe8：4.4-O0 局部槽基恒从 ebp-8 起（帧顶留 8B gap），4.1 大局部可顶到 ebp
//（N95 定性；double framePad 反致下移、int pad/声明序调整不可上移）。帧同 0xf4、
// 指令/助记符逐条全等，仅 3 处 lea disp 差（32/32）。语义等价。
void CUser::getHashCode(unsigned char* digestOut)
{
    md5_context ctx;
    md5_starts(&ctx);
    // P0/P134：ORIG 无 bytes/head 槽（imul 结果 ebx 跨 call 缓存、operator[]
    // 结果 eax 直传），实参嵌套求值（R2L：size()*0x14d1 先物化，[0] 后算）。
    md5_update(&ctx,
               reinterpret_cast<unsigned char*>(&m_characList[0]),
               static_cast<int>(
                   static_cast<unsigned int>(m_characList.size()) * 0x14d1u));
    md5_finish(&ctx, digestOut);
}