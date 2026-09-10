int getActivePropertyTypeToEnumValue(std::string& s)
{
    // ORIG 用 less_TCHAR（strcmp 内容比较）：查找键为 s.c_str() 动态指针，
    // 默认 std::less<const char*>（指针比较）必然 miss → 恒返回 17（错误）。
    // ORIG 16 处登记各只有两次 call：先就地构造 map::value_type
    // （_ZNSt4pairIKPKciEC1IRA<N>_S0_iEEOT_OT0_ = pair<const char* const,int>
    //  的 pair<char const(&)[N], int> 模板构造），再 insert(value_type const&)。
    // 用 make_pair 会多出 pair<const char*,int> → value_type 的转换临时。
    typedef std::map<const char*, int, less_TCHAR> LookupMap;
    for (;;)
    {
    init_retry:
        // ORIG 的 continue 直接跳回 +0xc（__lookup 的 __cxa_guard 检查），
        // 说明三个函数级静态声明在循环体内部。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int v;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it = __lookup.find(s.c_str());
            v = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            v = 0;
        }
        // ORIG 0x89bc019 起为 cmp/je 判定树（因含 case 0 值域稀疏），
        // case 0（未初始化态）与首个 case 0x2c4b 合并；每个 case 体在
        // !__inited 时登记一条键值后**贯穿**到下一 case 继续登记，
        // 直到落入 default 释放锁并置位 __inited 重跑（ORIG +0x919）。
        switch (v)
        {
        case 0:
        case 0x2c4b:
            if (!__inited) { __lookup.insert(LookupMap::value_type("slow resistance", 0x2c4b)); }
            else return 0;
            /* fall through */
        case 0x2c4d:
            if (!__inited) { __lookup.insert(LookupMap::value_type("freeze resistance", 0x2c4d)); }
            else return 1;
            /* fall through */
        case 0x2c4f:
            if (!__inited) { __lookup.insert(LookupMap::value_type("poison resistance", 0x2c4f)); }
            else return 2;
            /* fall through */
        case 0x2c51:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stun resistance", 0x2c51)); }
            else return 3;
            /* fall through */
        case 0x2c53:
            if (!__inited) { __lookup.insert(LookupMap::value_type("curse resistance", 0x2c53)); }
            else return 4;
            /* fall through */
        case 0x2c55:
            if (!__inited) { __lookup.insert(LookupMap::value_type("blind resistance", 0x2c55)); }
            else return 5;
            /* fall through */
        case 0x2c57:
            if (!__inited) { __lookup.insert(LookupMap::value_type("lightning resistance", 0x2c57)); }
            else return 6;
            /* fall through */
        case 0x2c59:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stone resistance", 0x2c59)); }
            else return 7;
            /* fall through */
        case 0x2c5b:
            if (!__inited) { __lookup.insert(LookupMap::value_type("sleep resistance", 0x2c5b)); }
            else return 8;
            /* fall through */
        case 0x2c5d:
            if (!__inited) { __lookup.insert(LookupMap::value_type("burn resistance", 0x2c5d)); }
            else return 9;
            /* fall through */
        case 0x2c5f:
            if (!__inited) { __lookup.insert(LookupMap::value_type("weapon break resistance", 0x2c5f)); }
            else return 10;
            /* fall through */
        case 0x2c61:
            if (!__inited) { __lookup.insert(LookupMap::value_type("bleeding resistance", 0x2c61)); }
            else return 11;
            /* fall through */
        case 0x2c63:
            if (!__inited) { __lookup.insert(LookupMap::value_type("piercing resistance", 0x2c63)); }
            else return 12;
            /* fall through */
        case 0x2c65:
            if (!__inited) { __lookup.insert(LookupMap::value_type("stuck resistance", 0x2c65)); }
            else return 13;
            /* fall through */
        case 0x2c67:
            if (!__inited) { __lookup.insert(LookupMap::value_type("confuse resistance", 0x2c67)); }
            else return 14;
            /* fall through */
        case 0x2c69:
            if (!__inited) { __lookup.insert(LookupMap::value_type("hold resistance", 0x2c69)); }
            else return 15;
            /* fall through */
        default:
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                // [b28d-solo-OnlyOrigBatch14] ORIG 0x89bc75d 段 `movb $1,__inited;
                // jmp +0xc（循环头）`；`continue` 在 4.4 下经 switch 尾再绕两条
                // 中继 jmp（ours 多 2 条）。goto 循环头标签直接产单条 jmp。
                goto init_retry;
            }
            return 17;
        }
    }
}
