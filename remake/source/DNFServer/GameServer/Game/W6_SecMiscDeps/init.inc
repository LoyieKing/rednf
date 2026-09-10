void WongWork::CSecurityCardCenter::init()
{
    // [b28d-solo-W6_SecMiscDeps 判词 · IDENTICAL_AE] ORIG 0x085fe05e：本轮 P310
    // 翻案后逐条全等（原「N 封顶（P209附）：unsigned 局部尾测不可翻」判词作废）。
    // ORIG T 0x085fe05e：固定密钥初始化头部 CRijndael；42 字符 characterSet
    // 逐个 push 进 m_characterSet；+0x484 置 1。（字符集数据段 0x93ac1e0）
    // [b28d-solo-W6_SecMiscDeps · P338 局部静态数据符号判据] ORIG 符号
    // `_ZZN8WongWork19CSecurityCardCenter4initEvE12characterSet` = nm
    // `093ac1e0 0000002a d`：**42 字节 .data**（无 NUL 终止符、非 const）。
    // 旧 `static const unsigned char[]` 生 43 字节 .rodata（多写 '\0'）。
    // 正解 = 非 const + 定长 [0x2a] 初始化（C++ 允许数组尺寸等于字面量长度，
    // 丢弃终止符），段与尺寸逐项对齐 ORIG。
    // GCC 4.4 对 `unsigned char[N] = "<N 字符>"` 报 too long ⇒ 用 42 元素
    // 显式初始化列表（内容逐字符照 ORIG .data 0x93ac1e0 的 42 字节）。
    static unsigned char characterSet[0x2a] = {
        '+', '<', '>', '?', '@', '#', '$', '&',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
        'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
        'Z', '1', '2', '3', '4', '5', '6', '7',
        '8', '9' };
    m_cipher.Initialize("n6;fw;vjfs0fuguf", 0x10, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    // [b28d-solo-W6_SecMiscDeps 判词更新 · P310 翻案] ORIG 0x085fe0d1 尾测
    // `mov -0xc(%ebp),%eax; cmp $0x29,%eax; setbe; test; jne`（两段寄存器形）。
    // 4.4 对 unsigned 局部恒折 `cmpl $0x29,-0xc` 单段（旧 N 封顶判词作废）；
    // P310 解法：局部声明 **int** + 显式 `(unsigned int)i` 比较 ⇒ 逐条复现。
    // 语义不变（i∈[0,42)，unsigned 转换后恒非负）。
    for (int i = 0; (unsigned int)i < 42; ++i)
    {
        m_characterSet.push_back(characterSet[i]);
    }
    m_timeCount = 1;
