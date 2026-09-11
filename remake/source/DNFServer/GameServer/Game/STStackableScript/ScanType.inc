// ---- DNFLex 扫描器接口：本 TU 只用这 6 个自由函数，故不 include
//      DNFLexWrapper.h —— 其 → DNFLex.h → <iostream> 会带出 __ioinit 与多余的
//      `_GLOBAL__I_`（ORIG 本 TU 无任何 GI：nm 全表无 stStackable/STStatChange/
//      STRecovery/CountinuousItemOption/AvatarOptionChangeType/RecipeInfo/
//      ImportStackable 键名的 GI，且 0x0898xxxx..0x089cxxxx 区间 GI 计数 = 0）
//      ⇒ P338/P327。局部声明与 DNFLexWrapper.h 权威签名逐字一致（mangled 相同，
//      链接到同一 ORIG 函数）。----
bool ScanType(std::string& token, bool consume);
