BindCube_LvBonusScriptData::~BindCube_LvBonusScriptData() {}

// ── 构造（ORIG 0x832c402）：成员初始化序 m_data → m_lvBonus[2]，
//    随后 body 内 new CMTRand(time(0)) 存 +0x00。──
// [b28c-lane17] fresh：101=101，残差 = new[] 返回值的异常槽 `lea -0x1c(%ebp)`
// 物化时刻（ORIG 4.1 提前入 %esi 跨 call 保活；4.4 在使用点重算入 %edx）
// + 清理块 %esi/%edx 互换 ⇒ N18a/N7（跨版本 callee-saved 分配），无源形。
// [b28d-20] fresh 复验 101=101，残差 3 处全在 new 表达式 ctor 实参 `&seed`：
//   ORIG 0x832c48d `lea -0x1c(%ebp),%esi` 位于 0x832c497 operator new **之前**
//   （4.1 先求 ctor 实参地址并入 callee-saved 跨 call 保活）；4.4 在 0x832c4a7
//   CMTRand::C1 前重算入 %edx。P226 两形实测均不闭合：
//   ① `new CMTRand(({ seed; }))` ⇒ 多临时槽 -0x20（103 vs 101）；
//   ② `new CMTRand(({ (const unsigned long&)seed; }))` ⇒ 仍右值化临时 -0x20
//   （GCC 语句表达式在 C++ 下强制 lvalue→rvalue）。⇒ 判 N18a/N7 封顶。
// [b28d-solo-CItemList] **AE（101/101，ndiff 0，p167 10/10 CLEAR）**：改用
//   P307 形 `register const unsigned long* pseed = &seed;` + `new CMTRand(*pseed)`
//   ⇒ 4.4 复现 ORIG 0x0832c48d `lea -0x1c(%ebp),%esi`（取址实参提前入
//   callee-saved 跨 operator new 0x832c497 保活）。旧 N18a/N7 判词作废。
