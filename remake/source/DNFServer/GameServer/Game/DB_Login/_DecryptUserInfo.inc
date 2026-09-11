// [b28d-solo-DB_Login] verdict=DIFF ndiff=9 gap=0（N7/P75：Hex2Binary 第三实参
// ORIG 多 `mov %eax,%edx` 中转、Decrypt 侧我方多该中转；(int)/(unsigned int)/
// 无 cast 三形均不翻。p167 CLEAR）。
// [Lane26 复核] 探针 /tmp/l26p/pd.cpp：局部缓冲 t7 恒 `shrl %eax; mov %eax,0x8(%esp)`
// （无 edx）；同形仅第二参为**形参指针**时才出 edx 中转（pc.cpp t4）。ORIG 为局部
// 缓冲却出 `mov %eax,%edx` ⇒ 4.1/4.4 实参压栈时刻版本差（P75），源形不可造 ⇒ N7/P75。
// S216：Hex2Binary/Decrypt 第三实参 cast 互换（(int)↔(unsigned int)）四形实测
// 逐条不变（同宽 cast 被 4.4 折叠）⇒ 维持 N7/P75 封顶。
bool DB_Login::_DecryptUserInfo(char* hex, char* out, memberIdentificationNumber_t mid)
{
    // b28d-68 修法：ORIG 0x0841300e。①Initialize 后 cipherBuf 0x80 字节 rep stos 清零
    // （原注释「未清零」误判，ORIG 0x08413075..0x08413089 实证）；②strlen(hex) 三处
    // 表达式直用（ORIG 每处独立 call，P0 禁缓存局部）；③cMyTrace 全表达式临时链
    // （ORIG 槽 -0x4c/-0x3c/-0x2c 深→浅，P221/P227）；④Decrypt 返回值比较用 while
    // 形复现 4.1 setne/test/je 物化（P225）；⑤成功/失败直接 return 常量（ORIG 无 ok 局部）；
    // ⑥ret 入口初始化 0x6fffffff（ORIG 0x0841301a）。
    // 残差 N7/P75：Hex2Binary 第三实参 ORIG 多 `mov %eax,%edx` 中转、Decrypt 第三实参
    // 我方多该中转（4.4 先 lea 第一实参地址 ⇒ len/out 落 edx）；(int)/(unsigned int)/
    // 无 cast 三形实测均不翻（P248 三档试尽）。p167 CLEAR、gap=0（140=140）。
    unsigned int ret = 0x6fffffff;
    CRijndael cipher;
    cipher.Initialize("sbTgiYsfxpfg5fox", 0x20, CRijndael::sm_chain0, 0x10,
                      0x10, 0, 0);
    unsigned char cipherBuf[128] = {0};
    if (strlen(hex) >> 1 >= 0x81)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3815, 5)(
            "DB_Login::_DecryptUserInfo ERROR m_id=%s", NumberToString(mid, 0));
        return false;
    }
    if (!DNFFLib::Hex2Binary(hex, cipherBuf, (int)(strlen(hex) >> 1)))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x381b, 5)(
            "DB_Login::_DecryptUserInfo::Hex2Binary ERROR m_id=%s", NumberToString(mid, 0));
        return false;
    }
    ret = cipher.Decrypt(reinterpret_cast<const char*>(cipherBuf), out,
                         strlen(hex) >> 1);
    while (ret != 0x6fffffff)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3820, 5)(
            "DB_Login::_DecryptUserInfo::Decrypt ERROR m_id=%s ret=%u",
            NumberToString(mid, 0), ret);
        return false;
    }
    return true;
}