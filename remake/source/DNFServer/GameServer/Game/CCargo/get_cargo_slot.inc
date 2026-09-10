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
