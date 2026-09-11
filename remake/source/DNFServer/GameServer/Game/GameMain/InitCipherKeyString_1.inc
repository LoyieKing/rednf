// InitCipherKeyString() —— ORIG 0x82a025a（0x1ff 字节，逐指令转写）：
// i=0..0x1387：new Cipher(0x410) 存入 s_secu_cipher_keystring_manager_[i]
// （ORIG 记录 stride 0x10：+0 Cipher*、+4/+8 int 键索引）；
// 首轮经 guard 记录总钥长；重复填充选中密钥串至总钥长后 Initialize。
// 尾部 cMyTrace 输出耗时（文案「推断」）。
// [b28d-28] DIFF | ORIG 0x082a025a | P167 MISSING __cxa_guard_acquire/release/
// abort + _Unwind_Resume vs EXTRA memset：ORIG 静态局部 guard 形（P21）我方
// 以 memset 直清，语义等价但形态不可复现。
// [b28d-solo-GameMain] AE | 135=135 | 真 bug 修：buf[0x800]（ORIG rep stos 计数
// 0x200 dwords = 0x800 字节；旧 buf[0x200] 少清 0x600 字节）+ 函数局部 static
// guard 形（P20/P21）+ P227 全表达式临时链 + 去 rec 指针缓存（P183）。
void InitCipherKeyString()
{
    int t0 = static_cast<int>(s_systemTime_.getCurSec());

    for (int i = 0; i <= 0x1387; ++i)
    {
        // P183：ORIG 每处重算 `shl $0x4; add $&manager`，不缓存 rec 指针。
        GlobalData::s_secu_cipher_keystring_manager_[i].cipher = new Cipher;
        // ORIG 0x082a02c1：函数局部 static 的 guard 形（P20/P21）——guard 变量
        // _ZGVZ19InitCipherKeyStringvE17needed_key_length，值取 **[0]** 的 cipher
        // 总钥长（ORIG 直接传 &manager 基址，非当前 i）。
        static int needed_key_length =
            GlobalData::s_secu_cipher_keystring_manager_[0].cipher->GetTotalKeyLength();

        // ORIG 0x082a0327：`rep stos` 计数 0x200 dwords = **0x800 字节**（P116/P253，
        // 旧实现 buf[0x200] 少清 0x600 字节，属真语义 bug）。
        unsigned char buf[0x800] = {0};
        char* keyStr = SelectKeyString(
            &GlobalData::s_secu_cipher_keystring_manager_[i].keyId,
            &GlobalData::s_secu_cipher_keystring_manager_[i].tblId);
        int keyLen = static_cast<int>(strlen(keyStr));
        int off = 0;
        // ORIG 0x082a03a6 条件只有 `off < needed_key_length`（P171 照抄，无 keyLen 守卫）。
        while (off < needed_key_length)
        {
            memcpy(buf + off, keyStr, keyLen);
            off += keyLen;
        }
        GlobalData::s_secu_cipher_keystring_manager_[i].cipher->Initialize(
            buf, needed_key_length);
    }

    // P118/P139：ORIG 传 _ZZ19InitCipherKeyStringvE19__PRETTY_FUNCTION__；
    // P227：全表达式临时链令 elapsed 在 ctor 前求值并以 %ebx 跨 ctor 保活（无槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x283e, 9, true, true)(
        "[!] Init CipherKeyString TotalTime : %ds",
        static_cast<int>(s_systemTime_.getCurSec()) - t0);
}