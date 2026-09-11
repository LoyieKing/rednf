void Cipher::AdjustDataSize(int key, int* size) const
{
    // [b28d-46 判词·AE] 45/45 指令数全等、调用点差集零；ORIG 0x080973c0。
    // Cipher 属加密/哈希类：按用户规则只要求算法正确（本件语义已逐条对应），豁免 byte 层。
    // [jcc 逐位复核] ORIG 0x08097411 `je +0x74(leave)`：写回 `*size = v` 在 if 块**内**
    // （v%bs==0 时不写回）；我方旧版写回在块外 ⇒ je 落点下标 40（写回行）≠ ORIG 43。
    int type = GetCryptoType(key);
    int v = *size;
    int bs = m_graphs[type]->GetBlockSize();
    if (v % bs != 0)
    {
        v += bs - v % bs;
        *size = v;
    }
}