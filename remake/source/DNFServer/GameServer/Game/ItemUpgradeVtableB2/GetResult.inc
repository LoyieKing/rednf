// ============================================================================
// DisJoint::GetResult @ 0x084733f6
//   out.m_f[5] = m_cubeIndexVecs[4][0]；
//   out.m_f[0] = ROUND(((ROUND(1.1f*sellPrice) 经 getItemSellPrice 折算 ×1000 ÷
//                m_creationConst) * m_creationRate[rarity]) ÷ 1000.0f)，最小 1；
//   _GetAdditionalItem(item, &out.m_f[6], &out.m_f[1])。
//   物品缺失走 LogManager::logFormat(1,…,0x6e,"[DisJoint::GetResult] Null Item\n")
//   并返回 0x11。
// [b28d-solo-ItemUpgradeVtableB2] **AE 105/105**：旧判 N85 已翻正——①rarity
//   无槽（get_rarity 直入 %edx 参与 0x14c 索引）②sellPrice 用 int 槽复用
//   （fildl→*1.1f→RC=trunc fistpl 写回同槽，禁 float 局部）③getItemSellPrice
//   返回值无槽（直进 imul $0x3e8）④base 具名槽 -0xc ⑤money 不设具名局部
//   （4.4 临时槽形与 ORIG -0x20 同构）⑥最小值用 if 形（三目折 cmovg）。
// ============================================================================
int DisJoint::GetResult(int itemIdx, DisJointResult& out)
{
    CItem* script = G_CDataManager()->find_item(itemIdx);
    if (script == 0)
    {
        LogManager::logFormat(1, "DisJoint.cpp",
                              __PRETTY_FUNCTION__,
                              0x6e, "[DisJoint::GetResult] Null Item\n");
        return 0x11;
    }

    // [b28d-solo-ItemUpgradeVtableB2] P0/P183：ORIG 0x08473410..0x08473466
    // rarity 无栈槽（get_rarity 结果直入 %edx 参与索引）⇒ 不设具名局部。
    const float rate = m_creationRate[script->get_rarity()];
    out.m_f[5] = m_cubeIndexVecs[4][0];

    // ORIG 0x0847348a..0x084734a3：int 槽 -0x10 复用（GetSellPrice 结果 →
    // fildl → *1.1f → RC=trunc fistpl 写回同槽）⇒ 具名 int 写回赋值，
    // 禁 float 局部（多 fstps 且帧 +0x10）。
    int sellPrice = script->GetSellPrice();
    sellPrice = (int)(sellPrice * 1.1f);
    // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x08473445..0x0847346c：getItemSellPrice
    // 返回值**无槽**（直接 `imul $0x3e8,%eax` 进入除法）⇒ 不设 sell 具名局部。
    // ORIG 0x0847346f..0x08473482：整数除法商存具名槽 -0xc（fildl 直读）。
    const int base = (getItemSellPrice(sellPrice, 0, 0,
                                       script->IsRandomOption()) * 1000) /
                     m_creationConst;
    // [b28d-solo-ItemUpgradeVtableB2] ORIG 0x0847350a `fistpl -0x20(%ebp)` +
    // `mov -0x20,%edx` 是编译器临时槽（不设 money 具名局部，4.4 直接
    // fistpl 到临时再写 out.m_f[0]，可省一个具名槽）。
    out.m_f[0] = (int)(((float)base * rate) / 1000.0f);
    // ORIG 0x08473520..0x0847352b `test %eax,%eax; jg` + movl $1 ⇒ if 形
    // （三目在 4.4 折 cmovg）。
    if (out.m_f[0] <= 0)
    {
        out.m_f[0] = 1;
    }

    _GetAdditionalItem(script, out.m_f[6], out.m_f[1]);
    return 0;
}