int ScriptLoader::LoadScriptDaily(RestrictType::T type)
{
    // ORIG 0x08abf4de：首 token 用 ScanTypeOrInt(isType, s, ok, true) 解析
    // （支持类型名/整数），isType 或 !ok 即整函数返回 0；
    // 字段顺序 ipgNo, fieldA, fieldB, t1, t2, fieldC（fieldA/fieldB 在时间前）；
    // readTime 失败 → 返回 -1；find 命中（重复 ipgNo）→ printf + 返回 -1；
    // 末尾 m_infoMap.insert(make_pair(ipgNo, info))。
    std::string str;
    bool isType = false;
    bool ok = false;
    // [b28d-solo P232] ORIG 0x08abf4fd 第三个 dead bool（-0x25）预清零。
    bool dailyLoaded = false;
    while (true) {
        // [b28d-solo P214③] ORIG 0x08abf501 循环头 4 条 `movl $0,-0x4e|-0x4a|
        // -0x56|-0x52` = t1[4]/t2[4] 的 `={0}` 预清（每轮重清，8 字节各 2 dword）。
        short t1[4] = {0}, t2[4] = {0};
        // [b28d-solo P0] ORIG 0x08abf53e `mov %eax,-0x5c`（ScanTypeOrInt 结果直接
        // 落 ipgNo 槽，后续 printf/find/pair 全用该槽）；`int v0` + 二次转换多一对
        // 存/取（-0x2c→-0x64）。
        unsigned int ipgNo = (unsigned int)ScanTypeOrInt(isType, str, ok, true);
        // [b28d-solo P223/BM8] ORIG 0x08abf547/0x08abf556 两个独立 nop 岛
        //（+0x1e8/+0x1eb）各自 `jne` 到共享 `mov $0,%ebx` 块 ⇒ 两条独立否定
        // goto + 连续尾标签串；`if(isType)return 0; if(!ok)return 0;` 使 4.4
        // 产就地 `mov $0,%ebx; jmp` 且极性反转。
        if (isType)
            goto daily_is_type;
        if (!ok)
            goto daily_not_ok;
        int fieldA = ScanInt((bool*)0);
        int fieldB = ScanInt((bool*)0);
        // [b28d-solo P292] ORIG 调用点 `call readTime; xor $1,%eax; test %al,%al`
        // ⇒ callee 返回 bool（int 返回产 `test %eax; sete %al`）。mangled 不含
        // 返回类型，改声明零 ABI 风险。
        extern bool readTime(short* buf);
        if (!readTime(t1))
            return -1;
        if (!readTime(t2))
            return -1;
        extern unsigned int uint32Time(short* buf);
        // [b28d-solo P226] ORIG 0x08abf612 `call uint32Time; mov %eax,%edi`（值升格
        // callee-saved 跨第二次 uint32Time 保活，无槽往返）；普通具名局部使 4.4 落槽。
        register int start = (int)uint32Time(t1);
        int end = (int)uint32Time(t2);
        int fieldC = ScanInt((bool*)0);
        // [b28d-solo P167/P123] ORIG 0x08abf5ff 调 `_Rb_tree_iterator::operator==`
        //（eq）非 operator!=；且 `je <printf 岛>` + 正常路径直落 ⇒ 源码为
        // `if (find == end) { 插入 } else { printf; return -1; }`。
        if (m_infoMap.find(ipgNo) == m_infoMap.end()) {
            InfoDaily* info = new InfoDaily(type, ipgNo, (unsigned int)fieldA,
                                            start, end, fieldB, fieldC);
            // [b28d-solo P192/P178] ORIG 无 make_pair 调用，直接 value_type 转换
            // ctor（`_ZNSt4pairIKjPN...EEC1IRjRS3_EEOT_OT0_`）。
            m_infoMap.insert(
                std::map<unsigned int, InfoDaily*>::value_type(ipgNo, info));
        } else {
            // [b28d-solo P185] ORIG rodata 原文（无前缀/无 \n）。
            printf("already registed ipg_no:%u", ipgNo);
            return -1;
        }
    }
daily_is_type:;
daily_not_ok:;
    return 0;
