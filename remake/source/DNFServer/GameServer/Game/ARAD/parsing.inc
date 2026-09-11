// b28d-solo-ARAD verdict=IDENTICAL/AE：P283 goto 尾标签复现 4.1「begin 后 jmp 到底部
// 条件块」；具名 `int idx` 由块内提至函数级 + 具名 insert 结果 `res`（P212/P247 槽序
// 旋钮）后槽位与 ORIG 全对齐（idx -0x20 / insert sret -0x28 / .second -0x24）。
int ExceptionItem::parsing(char* data)
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    // [L4e 2026-09-11 · 负结论：94/95，残余＝goto 后 1 条 `nop`] ORIG 0x81a2039 `jmp` 直落
    // 0x81a20b9（无填充），我方同位置多 1 条 nop。根因＝后端版本行为：同形最小复现
    // /tmp/exp/v20.cpp(-m32) 在 cc1plus 4.4.4/4.4.6/4.4.7 均于循环尾生成 nop，c5root
    // GCC 4.1.2 同源无 nop 且逐条全等；8 个候选旗标（-fno-exceptions/-falign-loops=1/
    // -fno-reorder-blocks/…）无一消除 ⇒ 源码层不可修；整 TU 换 4.1.2 亦不可行
    // （HashMap.h 等 4.4-only 语法编译失败）。串引用差＝未 relink 显示（§6.3 假阳性），已核。
    Arad_Script script;
    if (!script.Load(data, "material_exception_item"))
        return 0x27;
    if (script.GetRowNum() == 0)
        return 0x2a;
    // ORIG 循环每轮重调 GetRowNum
    // b28d-60：P283 4.1「begin 后无条件 jmp 到底部条件块」布局——原 while(true)+
    // break 使 4.4 条件置顶（setle/jne end）；改 goto 尾标签复现 setg/jne body。
    int r = 0;
    int idx;   // b28d-solo-ARAD：P212 全头部声明（槽序试探）
    goto check;
loop_body:
    {
        idx = script.GetInt(r, "material_exception_item_index");
        // b28d-solo-ARAD：具名 sret 槽试探（ORIG insert sret 在 -0x28、.second -0x24）。
        std::pair<std::map<int, int>::iterator, bool> res =
            m_exceptionItemIndex.insert(std::make_pair(idx, idx));
        if (!res.second)
            return 0x33;   // ORIG insert 失败（.second==0）→ 0x33
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}