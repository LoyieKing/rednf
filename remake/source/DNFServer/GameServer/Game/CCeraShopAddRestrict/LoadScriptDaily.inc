// [L6S6 2026-09-11 双钥匙**命中**，破旧判 N95/N93/N7] ① 顺序轴：fieldC 的 ScanInt
// 在 readTime 之后、map 查询之前（ORIG 0x08abf5b2），start/end 的 uint32Time 在
// 查询**之后**的命中分支内（ORIG 0x08abf60c/619）；② `register int end` 裸限定 ⇒
// 4.4 把它 spill 到最深槽 -0x6c（ORIG 同址），帧 `sub $0x8c` 与全部槽位
// （ipgNo -0x5c / t1 -0x4e / t2 -0x56 / fieldA -0x24 / fieldB -0x20 / fieldC -0x1c /
// end -0x6c / new -0x60）逐一对齐 ORIG 0x08abf4de ⇒ 154=154 AE，p167 calls 20=20
// （仅余 C1/C2 别名名差）。
// 旧判词：N 封顶（154=154、p167 CLEAR；残余 N95 槽基差 8 + N93 块内槽序 + N7 寄存器中转 edi/eax）。
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
        // [L6S6 2026-09-11 顺序轴] ORIG 指令序：ScanInt#1/#2 → readTime×2 →
        //   ScanInt#3(fieldC) @08abf5b2 → map end/find/operator== @08abf5c1..5ff →
        //   uint32Time#1/#2 @08abf60c/619 → operator new @08abf627 → ctor @08abf668。
        // 即 fieldC 的 ScanInt 在 readTime 之后、map 查询**之前**，而 start/end 的
        // uint32Time 在 map 查询**之后**（命中分支内）。旧形的 start/end 求值在
        // ScanInt#3 之前，造成槽位/序整体错位（end 落 -0x20 而 ORIG 落 -0x6c）。
        // uint32Time 为纯读函数 ⇒ 移入分支内不改变行为。
        int fieldC = ScanInt((bool*)0);
        // [b28d-solo P167/P123] ORIG 0x08abf5ff 调 `_Rb_tree_iterator::operator==`
        //（eq）非 operator!=；且 `je <printf 岛>` + 正常路径直落 ⇒ 源码为
        // `if (find == end) { 插入 } else { printf; return -1; }`。
        if (m_infoMap.find(ipgNo) == m_infoMap.end()) {
            // [b28d-solo P226] ORIG 0x08abf612 `call uint32Time; mov %eax,%edi`
            //（值升格 callee-saved 跨第二次 uint32Time 保活，无槽往返）。
            register int start = (int)uint32Time(t1);
            // [L6S6 2026-09-11 P231 命中，破 N95 槽基差] ORIG 0x08abf619
            // `mov %eax,-0x6c(%ebp)`：end 落**最深**槽（低于 t2 -0x56/ipgNo -0x5c）。
            // 试形：分支内普通 `int end`（落 if 区 -0x20）✗；`int end;` 函数级声明
            // ✗（落 -0x28，但使 if 区三字段上移 4 对齐 ORIG）；循环体池首声明 ✗
            // （同 -0x28）。裸 `register int end` ⇒ 4.4 spill 到最深槽 -0x6c，
            // 帧随之 0x7c→0x8c 与 ORIG 全等、near→AE。
            register int end = (int)uint32Time(t2);
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
}