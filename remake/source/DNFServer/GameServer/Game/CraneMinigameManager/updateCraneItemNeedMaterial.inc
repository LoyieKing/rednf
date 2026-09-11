// ORIG 0x080ebebc：m_selectIdx = count % m_needMaterials.size()；
// 取对应 pair 调 NeedMaterialDataManager::setNeedMaterialInfo(m_material, pair)。
// 逐条对齐：帧 0x20 + `push %esi; push %ebx` 与 ORIG 全等（+0x49 at() 结果留
// %ebx、+0x50 m_material 留 %esi，二者跨 +0x56 getInstance() 保活 ⇒ 用 P179
// register 形复现 4.1 双 callee-saved 形；普通引用形被 4.4 落栈槽，37→40）。
// 残余 DIFF 归因（N 类，40/40 等长，仅 6 行搬移链次序）：4.1 把 count 提前
// 载入 %ebx 再 `mov %ebx,%eax` 喂 divl、余数 `mov %edx,%ecx`；4.4 直取
// 0xc(%ebp) 且 div 结果先归 %eax 再 `mov %eax,%ecx`（N7/N82 RA 族）。
// [b28d-lane15 复核] 维持 N：N96 清绑后残差为 N7/N82 RA 寄存器角色差（ORIG
// +0x26 余数三段搬运 / +0x49 双 callee-saved 保活），语义等价。
void CraneMinigameManager::updateCraneItemNeedMaterial(int count)
{
    // [P6AuditOldN.P6S2 2026-09-11] 试验旋钮：ORIG 0x080ebec4 在 size() 调用**前**
    //   先 `mov 0xc(%ebp),%ebx` 保活 count（4.1 求值序），4.4 调用后才读 ⇒ 用裸
    //   register（P231 非绑定、不占槽）承载 count 值，强制提前读并跨调用保活。
    register unsigned int regCount = static_cast<unsigned int>(count);
    unsigned int size = m_needMaterials.size();
    // b28c-L19 清绑（P0/N96 全域禁 register asm）：改 P0 表达式直传形。
    // ORIG +0x26..+0x2f：余数三段搬运后 1 字节存 +0x68；+0x49/+0x50：at() 结果
    // 指针与 m_material 双 callee-saved 保活（4.4 自然形可能落栈槽，残差记 N）。
    // [N1Free91to105 2026-09-11] 负结论：本题（ORIG 40 / our 38，gap +2）残余为
    // 4.1 与 4.4 的两处求值/搬运序差：
    //  ① ORIG 0x080ebec4 `mov 0xc(%ebp),%ebx`（size() 调用**前**读 count 并保活）
    //     vs 4.4 调用后才读 —— `A % B` 操作数求值序差异；
    //  ② ORIG 0x080ebee2-0x080ebee6 `mov %edx,%ecx; mov %ecx,%eax`（余数三段搬运）
    //     vs 4.4 `mov %edx,%eax`（两段）—— 窄化链跨版本差异。
    // 已试旋钮：去双重 static_cast 改隐式窄化 `count % size` ⇒ 40/38 与 diff 逐条不变
    // ⇒ 回滚。判 N 类封顶（ORIG 地址 0x080ebebc，形见 report）。
    // [P6AuditOldN.P6S2 2026-09-11] 本波落地 2 旋钮：裸 register 承载 count
    //   （regCount，P231 非绑定不占槽）⇒ ORIG 0x080ebec4 `mov 0xc(%ebp),%ebx` 提前
    //   读 + `mov %ebx,%eax` 两段已复现；再加深 register 余数 regRem ⇒ 38→39 条
    //   （gap +2→+1）。残余仅 ORIG 0x080ebee2..e6 `mov %edx,%ecx; mov %ecx,%eax`
    //   三段搬运（4.4 恒两段；已试 `register unsigned char` 形 ⇒ `%bl` 写并重载
    //   this，更劣已回滚）。判 N 类封顶。
    register unsigned int regRem = regCount % size;
    m_selectIdx = static_cast<unsigned char>(regRem);
    NeedMaterialDataManager::getInstance()->setNeedMaterialInfo(
        m_material, m_needMaterials.at(m_selectIdx));
}