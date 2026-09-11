// ----------------------------------------------------------------------------
// CParty::getMonsterTotalExp @ 0x085a23dc（_ZN6CParty18getMonsterTotalExpEjciffb）
// 逐指令转写（docs/class_func_reports/CParty/getMonsterTotalExp.md + objdump）：
//   base = G_CDataManager() 返回值 + 0x68；
//   flag(char,有符号) <= 3 时 rate = *(float*)(base + 4*(flag+0xfc))，
//   否则 rate = 1.0f（0x3f800000 立即数）；
//   bool d 非 0：rate *= *(float*)0x08cbbbd0（字节 00 00 40 40 = 3.0f）；
//   x87 链（fildll/fdivrp/fmuls 就近舍入 fistpll）：
//     (double)(unsigned long long)exp / 2.0f（*(float*)0x08cbbbd4，
//     字节 00 00 00 40 = 2.0f）
//       * 参数 c(0x1c(%ebp)) * *(float*)(base + 4*(a+0xe4) + 8) * b * rate
unsigned int CParty::getMonsterTotalExp(unsigned int monsterIdx, char flag, int a,
                                        float b, float c, bool d)
{
    // ORIG base = G_CDataManager()+0x68 == &CDataManager::m_serverParameter
    //（CDataManager.h 权威布局 +0x68）。ORIG 0x085a23f4：call 返回 %eax
    // 直接 add $0x68 后 spill 基址指针局部（-0x14），非引用绑定。
    ServerParameterScript* sp = reinterpret_cast<ServerParameterScript*>(
        reinterpret_cast<char*>(G_CDataManager()) + 0x68);
    float rate;
    if (flag <= 3)
    {
        // ORIG 地址 = &sp + (flag+0xfc)*4 = &m_flagExpRate3f0[flag]
        //（ServerParameterScript.h +0x3f0；flag<0 时与 ORIG 同址前越界读取）。
        rate = sp->m_flagExpRate3f0[static_cast<int>(flag)];
    }
    else
    {
        rate = 1.0f;  // .text 立即数 0x3f800000
    }
    if (d)
    {
        // .rodata 0x08cbbbd0: 00 00 40 40 → float 3.0f
        rate *= 3.0f;
    }
    // ORIG 经验表地址 = &sp + (a+0xe4)*4 + 8 = &m_monsterExpTable398[a]（+0x398）。
    // ORIG 0x085a244f：ULL/2.0f 无 double 显式转（flds float 除数），乘法链
    // c→table→b→rate；结果 fistpll 落 -0x28 后取低 32 位入独立无符号局部。
    // ORIG 无具名 long long 局部（fistpll 落复用的零扩展临时槽 -0x28/-0x24），
    // 写成单表达式 → gcc 合并临时槽。
    unsigned int exp = static_cast<unsigned int>(static_cast<long long>(
        static_cast<unsigned long long>(monsterIdx) / 2.0f *
        c * sp->m_monsterExpTable398[a] * b * rate));
    return exp;
}