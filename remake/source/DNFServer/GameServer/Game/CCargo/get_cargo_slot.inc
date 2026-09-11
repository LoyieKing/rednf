// ---- get_cargo_slot @ 0x850b2b4（按值返回 Inven_Item） ----
// ORIG 0x850b2b4：`call IsOperatorable; xor $0x1; test %al,%al; je +0xaf` ——
// je 落点 +0xaf 为 `mov this+0x4,%edx; imul $0x3d; memcpy(sret, &m_pItems[idx],0x3d)`，
// 即 **IsOperatorable==1（合法索引）→ 返回 m_pItems[idx] 原内容**；
// IsOperatorable==0 → `Inven_Item out; memset(&out,0,0x3d);` 返空 item。
// [b28d 真语义 bug 修正] 旧码分支内容互换（!IsOperatorable 返回原内容），已按 ORIG 翻转。
// [b28d-P59 修复] 否定守卫 + 尾返回成员后 68=68 **AE**（旧码 104 条）。
// 语义与 ORIG 分支一一对应（IsOperatorable==1 返 m_pItems[idx] / ==0 返空 item）。
Inven_Item CCargo::get_cargo_slot(int idx) const
{
    // [b28d-P59] ORIG 块序 = 空 item 分支直落、memcpy 分支在尾（`je +0xaf`）
    // ⇒ 源码用否定守卫 `if (!IsOperatorable(idx)) {...return;}` 再 fall 到返回成员。
    if (!IsOperatorable(idx))
    {
        Inven_Item out;
        memset(&out, 0, sizeof(out));
        return out;
    }
    return m_pItems[idx];
}