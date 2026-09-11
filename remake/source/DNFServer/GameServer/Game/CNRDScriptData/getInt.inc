int CNRDScriptData::getInt(const char* key) const
{
    const int* p = m_intMap.get(key);
    // b28c-L10：ORIG 0x0807fe8a `cmpl $0x0,-0xc(%ebp); jne HIT`，空值分支 fall-through
    // `mov $-1,%eax; jmp end`、命中分支跳走。早期返回形 `if(p==0)return -1; return *p;`
    // 与 ORIG 逐条同形；goto 尾标签形会在跳转目标前多 1 条 nop（19 vs 18）。
    if (p == 0)
        return -1;
    return *p;
}