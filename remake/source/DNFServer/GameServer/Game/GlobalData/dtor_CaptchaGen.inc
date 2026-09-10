CaptchaGen::~CaptchaGen()
{
    // ORIG 0x080de717：ix 实参先载 i 槽（int→unsigned 转换 ⇒ arg 先于 this，
    // 技巧5/15）；0x080de74b `setg` ⇒ 两侧 int 有符号比较；循环体 FT_Done_Face
    // （0x080de731）；循环后 FT_Done_FreeType(m_library)（0x080de75a）再落
    // vector 成员 dtor。
    // [b28 判词·N·EH 可见性] ORIG 49 vs 我们 33：ORIG 4.1 对**外部 FreeType .so**
    // 不透明调用布 landing（push esi/ebx + cleanup vector D1 + _Unwind_Resume 段，
    // 16 insn）；本仓 FreeType 以 extern "C" 占位定义**同 TU 内联可见**（1138 起），
    // 4.4 throw 分析判 nothrow → 无 landing。探针 /tmp/l2_p10.cpp 实证改 extern 声明
    // 即产 landing，但搬移 shim 定义会同时改 ctor/addFont（已 AE）调用点形态，
    // 风险>收益；`throw(...)` spec（-fno-enforce-eh-specs）与 try/catch{throw;}
    // 实测均无效（33 不变）。属 N 类「跨工具链第三方符号可见性」，语义等价豁免。
    for (int i = 0; i < (int)m_faces.size(); ++i)
    {
        FT_Done_Face(m_faces[i]);
    }
    FT_Done_FreeType(m_library);
