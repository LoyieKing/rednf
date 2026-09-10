int Cipher::Initialize(const unsigned char* key, int keyLen)
{
    std::memcpy(m_keyBuf, key, keyLen);
    for (int i = 0; i < CRYPTO_GRAPH_MAX; ++i)
    {
        int ks = m_graphs[i]->GetKeySize();
        // [b28c-L9] ORIG 0x08097357 `cmp -0xc(%ebp),%eax; jl return0岛`：body 为
        // fall-through、`return 0` 外置岛（4.4 对否定早返恒反转成 jge + fall 岛）。
        // 正向包裹（P222）复现 ORIG 极性与岛位：ndiff 21→6。
        // 残差 6 = N7：m_graphs[i]->Initialize 间接 call 的 funcptr ORIG→%ecx、
        // 实参链→%edx，4.4 恒互换（%edx/%ecx）；指令数 60=60、助记符全等 ⇒ NEAR 封顶。
        if (keyLen >= ks)
        {
            m_graphs[i]->Initialize(key, ks);
            key += ks;
            keyLen -= ks;
        }
        else
        {
            return 0;
        }
    }
    return 1;
