// b28d-solo-N 封顶：槽位集与 ORIG 全对齐（cpBuf -0x1010 / 三份 ErrorLog
// e -0x50/-0x60/-0x70 / auto_ptr -0x38 / isJpn -0x2e / isKor -0x2d / lineNo -0x2c）；
// 残差三类均为已登记 N 族：① `memset(cpBuf,0,0xfa0)` 4.1 内联 rep stos vs 4.4
// 恒 call（N78）；② `isJpn == false` 取反 4.4 恒 `movzbl;xor $1;test;je` vs
// ORIG `cmpb $0,mem;je`（N122）；③ r 槽 -0x28 vs ORIG -0x24 + 内存直比 vs
// 寄存器比较（N82/P286）。本波修正的真语义 bug：第三处 ErrorLog 原写
// `e.m_message += getKey(i)`，ORIG 0x5739 为 `add $0x8,%eax` + `operator=`
// ⇒ 应为 `e.m_detail = getKey(i)`；另补两处 `e.m_flag = lineNo`
// （ORIG 0x5415/0x5489 `mov lineNo,-0x44/-0x54`）。
// ORIG 0x8aa529e。
// [M2S5StringRaw 2026-09-11] 本轮修复 gap 主项：两处 bool 条件原写成取反形
// （`isJpn == false` / `!foundDuplicate`），令 4.4 发 `movzbl; xor $1; test` 4 条，
// 而 ORIG 为 `cmpb $0,mem; je` 2 条（0x5535 `je 0x55b7` 落点是「无检查 add」块、
// 0x5794 `je 0x57a1` 落点是 push_back 块 ⇒ ORIG 源码本就是正向 if）。已改为
// `if (isJpn)`（真块 fall-through 带 NULL 检查、else 为无检查 add）与
// `if (foundDuplicate) return false;`。预期 390 → 386（ORIG 387；余 N18b 的
// r 寄存器比较 1 条我方无）。待父泳道 fresh 编译核账。
bool RDARScriptStringManager::loadStringFile(std::istream* in, const char* path,
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
                                             int /*table*/,
                                             std::vector<ErrorLog>* err)
{
    // ORIG 0x8aa529e：① skip 是表达式短路物化（`je L;je L;call bad;test;je L2`
    // + `mov $1/$0,%eax`，无具名 bool 槽，P0）；② auto_ptr 默认构造后 reset
    // （ORIG 0x52ec `auto_ptr(0)` → new → C1 → reset）；③ isJpn/isKor 为
    // 「预清 + 条件置位」（P194）；④ ErrorLog 第三字段 = lineNo（e.m_flag）；
    // ⑤ 重复检测先 getKey(i+1) 后 getKey(i)、无判空、第三处为
    // `e.m_detail = getKey(i)`（ORIG `add $0x8,%eax` + operator=，非 +=）。
    if (in == 0 || path == 0 || in->bad())
        return false;

    std::auto_ptr<StringDictionary> dict;
    dict.reset(new StringDictionary());

    bool isJpn = false;                                          // local_32
    if (std::strstr(path, "Creature.jpn.str") != 0)
        isJpn = true;
    bool isKor = false;                                          // local_31
    if (std::strcmp(path, "Etc/Etc.kor.str") == 0)
        isKor = true;

    int lineNo = 0;
    char lineBuf[0x2928];
    // [M2S5E 2026-09-11] P339 声明序：ORIG 槽序 isJpn(-0x2e)/isKor(-0x2d)/
    //   lineNo(-0x2c)/foundDuplicate(-0x25)/r(-0x24)/i(-0x20)/count(-0x1c)
    //   ⇒ foundDuplicate 的**声明**在 r 之前（4.4 按声明序自深至浅分配）；
    //   旧形在 freeze() 之后才声明，令 4.4 把它放 -0x21、r 放 -0x28（两槽各偏 4）。
    //   赋值点保持在 freeze() 之后（ORIG 0x56xx `movb $0x0,-0x25` 同点）。
    bool foundDuplicate;
    // [M2S5E 2026-09-11 负结论] `volatile int r` 试验：读数 393 vs ORIG 387
    //   （gap 0→−6）且全函数槽整体平移（-0x38→-0x34、-0x2e→-0x2a、-0x2c→-0x28）
    //   ⇒ 回滚。ORIG 0x8aa529e 段两处 r 比较为 4.1 的恒 load 形
    //   `mov -0x24(%ebp),%eax; cmp $A,%eax; je`；4.4 -O0 对普通局部恒发 mem-compare
    //   `cmpl $A,-0x24(%ebp); jne`（该处我方少 1 条）——4.4 无源形可翻（P75 封顶）。
    int r;

    while (!in->eof())
    {
        ++lineNo;
        in->getline(lineBuf, 0x2928);

        char* key = 0;
        char* value = 0;
        r = parseStringLine(lineBuf, &key, &value);

        // [M2S5E 2026-09-11] 决策树 → 真 switch（P 族：手写 if/else-if → switch）：
        //   ORIG 0x8aa529e+0x152 段为 `mov -0x24(%ebp),%eax; cmp $0xfffffffe,%eax;
        //   je <err1>; cmp $0xffffffff,%eax; jne <skip>`（比较链 + 单次 load），
        //   正是 4.1 -O0 对 switch 的发射形；旧 if/else-if 让 4.4 发
        //   `cmpl $A,-0x24(%ebp); jne`（mem-compare，少 1 条 load）。
        switch (r)
        {
        case -1:
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000000);
                e.m_message = path;
                e.m_flag = lineNo;
                err->push_back(e);
            }
            return false;
        case -2:
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000001);
                e.m_message = path;
                e.m_flag = lineNo;
                err->push_back(e);
            }
            return false;
        }

        if (key != 0 && value != 0)
        {
#if !remake_verify
            // DNF-FIX: ORIG 的 cpBuf 是函数级槽（4.4 不回收）；块内声明会让下面的 value 悬垂
            char cpBuf[4000] = { 0 };
#endif
            if (isKor)
            {
#if remake_verify
                // [b29-Lane21] N78/P214③：块内 `char cpBuf[4000] = { 0 };` 令 4.4 内联
                // `lea; mov $0,%eax; mov $0x3e8,%edx; rep stos`（ORIG 0x0835… 形）；
                // 原函数级声明 + memset 恒发 `call memset`（多 4 条）。
                char cpBuf[4000] = { 0 };
#endif
                std::strcpy(cpBuf, value);
                CodePage::script2Database(value, cpBuf);
                value = cpBuf;
            }

            // [M2S5StringRaw 2026-09-11] ORIG 0x5535 `cmpb $0,-0x2e; je 0x55b7`
            // + 0x55b7 块体 = 无检查 add ⇒ ORIG 源码是**正向 `if (isJpn)`**
            // （真块 fall-through 带 NULL 检查，else 块在其后）。旧形写
            // `isJpn == false` 令 4.4 发 `movzbl; xor $1; test; je`（4 条，
            // 与 4.1 的 cmpb+je 2 条不同，属 N122 真根因，非纯后端差）。
            if (isJpn)
            {
                // ORIG 0x5549 `test %al,%al; setne %al; test %al,%al; je <NULL 分支>`
                // ⇒ 正极性包裹（P214①）；toTString 结果无槽往返（P0 实参内联）
                if (*toTString(value) != '\0')
                    dict->add(toTString(key), toTString(value));
                else
                    dict->add(toTString(key), "NULL");
            }
            else
            {
                dict->add(toTString(key), toTString(value));
            }
        }
    }

    dict->freeze();

    // 重复连续 key 检测
    foundDuplicate = false;
    // [M2S5E 2026-09-11] P339 声明序（ORIG 槽深→浅）：ORIG `addl $0x1,-0x20(%ebp)`
    //   + `cmp -0x1c(%ebp),%eax` ⇒ 循环变量 i 在 -0x20（深）、上限 count 在 -0x1c
    //   （浅）；旧形 `int count = ...; for (int i = ...)` 让 4.4 把 count 放 -0x20、
    //   i 放 -0x1c（全函数 10 处 mem 引用互换）。把 i 提到 count 之前声明
    //   （`for (; i < count; ++i)` 保持初始化位置与 ORIG 的 `movl $0x0,-0x20` 同点）。
    int i = 0;
    int count = dict->getCount() - 1;
    for (; i < count; ++i)
    {
        // ORIG 0x564b：strcmp/printf 的 getKey 均无具名槽（P0 实参内联），
        // 第 2 实参（i+1）先求值。
        if (std::strcmp(dict->getKey(i), dict->getKey(i + 1)) == 0)
        {
            std::printf("%s, %s \n", dict->getKey(i), dict->getKey(i + 1));
            foundDuplicate = true;
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000002);
                e.m_message = path;
                e.m_detail = dict->getKey(i);
                err->push_back(e);
            }
        }
    }

    // [M2S5StringRaw 2026-09-11] ORIG 0x5794 `cmpb $0,-0x25; je 0x57a1`：je 跳
    // push_back 块 ⇒ 正向 `if (foundDuplicate) return false;`（旧形 `!foundDuplicate`
    // 令 4.4 发 movzbl+xor+test，4 条 vs ORIG 2 条）。
    if (foundDuplicate)
        return false;
    dictionaries_.push_back(dict.release());
    return true;
}