// ---------------------------------------------------------------------------
// [Q8S5 2026-09-11 指派复核（Main 第⑤类待核项）] setcc/jcc + disp2 逐指令核实 ⇒ **等价，非 bug**：
//   ① setcc 点：我方 `1af4 seta %al` vs ORIG `0x085f3841 setbe %al`，两侧**同一** `cmp -0x10(%ebp),%eax`
//      （eax = vector<OptionGroup>::size()，mem = get_value() ⇒ sel）。我方 `seta`(size>sel) + `je 1c03`
//      （跳 logFormat 舱）↔ ORIG `setbe`(size<=sel) + `je 0x085f3897`（跳正常路径）⇒ **条件取补 + 两臂对调**
//      ⇒ 语义相同（两舱 logFormat 的 line 均 `0x390`：ORIG 0x085f3869 / 我方 1c21）✓
//   ② disp2 ORIG-only `+0xd ×2`：ORIG `movzbl 0xd(%eax),%edx` + `mov %dl,0xd(%eax)`（字段 load/store，0x085f394a/953、
//      0x085f3987/990）vs 我方 `lea 0xd(%eax),%edx` + `add $0xd,%eax`（1b9c/1ba2、1bc4/1bca，**取址/地址算术**）
//      ⇒ 门控 **③d**，与结构字段偏移无关 ✓
//   cmpscan2 v2 亦判 `[INFO 窗口不匹配]`（非 SYMSIGN）⇒ 三路一致（工具 v2 / 本核实 / Q8S3 独立核实）。
//   无改动、无回滚。
// ---------------------------------------------------------------------------
// [P8S1 2026-09-11 P139] 首参拼写改 typedef 名 `itemIndex_t`（GameTypes.h，
//   = unsigned long，mangled 仍 `…13change_optionEm…` 不变）⇒ 宏
//   __PRETTY_FUNCTION__ 按**定义处**拼写展开，与 ORIG prett 串逐字节一致
//   （探针 /tmp/p8s1/pf.cpp：声明 unsigned long、定义 typedef 名 → 展开 typedef 名）。
bool CRandomOptionItemHandle::change_option(itemIndex_t itemIdx, ENUM_RARITY rarity,
                                            int level, int grade, int group,
                                            int slot, RandomOption* opts)
{
    if (slot < 0 || slot > 2)
    {
        return false;
    }


    unsigned char optType = 0;
    // [L31 round2] 四处预清全补反劣（406→416）⇒ 回滚。本轮只补 **err 单处**（P198① 最小集）：
    // ORIG 0x085f3352 `movl $0x0,-0x1c(%ebp)`（optType 预清后紧邻）⇒ 原码 `int err = 0;`
    // 后接赋值两句式（i/pick/sel 无预清证据）。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N95）→ 试=①prett2 缺口 0（P139 已
    //   落地）、strall 串差 0（仅 base64 假阳性）；②p167 CLEAR；③444 后端变体实测
    //   432/440 gap=-8 **逐项相同**（本 TU 已路由 446 后端，无后端旋钮）；④残余
    //   （权威 ndiff 424）全为 N82/N95 族：帧 0xe0/0xd0 与 optType(-0x53/-0x65)/err
    //   (-0x20/-0x1c) 槽差、`remaining -= w` 的 4.1 %ebx 跨调用保活 5 条 vs 4.4
    //   mem-sub 2 条、内层循环尾 break+回跳两条 jmp、sel<size 的 seta/setbe 极性；
    //   无 P342/ILb 候选（本 TU 的 `_Destroy_aux` 实例仅 ILb1、与 ORIG 同）⇒ 维持
    //   （新=DIFF 432/440 gap=-8, blocks=58）。
    int err = 0;
    err = _get_option_type(itemIdx, rarity, level, group, optType);
    // [b28d-solo-UserMiscLink] P225：ORIG 0x085f33a1 为 `cmpl $0; setne %al; test %al,%al;
    // je`（bool 物化岛）⇒ while 形复现（4.4 裸 if 恒直跳）。
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            // [P8S1 2026-09-11 P139 落地] `itemIndex_t` 已在 GameTypes.h，定义处拼写
            //   同步（change_option.inc）⇒ 换回宏（展开串 = ORIG prett size 0x78 逐字节同）。
            __PRETTY_FUNCTION__,
            0x356, "_get_option_type (%d,%d,%d,%d,%d) error_line(%d)",
            (int)itemIdx, (int)rarity, level, group,
            (int)optType, err);
        return false;
    }

    int outGroup = 0;
    err = _choose_option_group(rarity, group, grade,
                               (unsigned char)level, optType, outGroup);
    while (err != 0)
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            // [P8S1 2026-09-11 P139 落地] 同上（宏展开 = ORIG prett 串）。
            __PRETTY_FUNCTION__,
            0x35d, "_choose_option_group (%d,%d,%d,%d,%d,%d) error_line(%d)",
            (int)rarity, group, grade, level,
            (int)optType, outGroup, err);
        return false;
    }

    // [b28d-solo-UserMiscLink] 判词（N 封顶）：443/432、p167 CLEAR(47=47)；
    // 残差=①帧 0xc0 vs 0xd0 + 局部槽深差（optType -0x53 vs -0x65、err -0x28 vs -0x1c）
    // ⇒ N82/N95；②4.4 lea 折叠（reinterpret_cast<unsigned char*>(opts)[i] 字节写 vs ORIG disp 直写）；③寄存器分配。
    // 已复现：const_iterator（P96/P270 翻转 13 项 p167 差集）、begin()+sel 的 deEv/plERKi
    // （P167 判据）、setne 物化岛（P225 while 形）、union 字节键、GetRandomOption 逐点重取（P157）。
    // [L4b 2026-09-11 本轮] ①P183 去 seedRoll 具名局部（444→442）；
    // ②P243 `slot == i` 书写序（cmp 操作数序，ndiff −4）；
    // [P8S1 2026-09-11 P139 落地] ORIG prett 符号
    //   `_ZZN13random_option23CRandomOptionItemHandle13change_optionEm11ENUM_RARITYiiiiP12RandomOptionE19__PRETTY_FUNCTION__`
    //   （size 0x78 @0x08cd0f00，ORIG 体内 REF：`85f33e6: movl $0x8cd0f00,0x8(%esp)`）已复现：
    //   P7HeaderAxis 在 GameTypes.h 加 `typedef unsigned long itemIndex_t;`，本处定义首参
    //   拼写同步（change_option.inc）+ 5 处手写串换回宏（探针实证：宏按**定义处**拼写展开，
    //   .h 声明写 unsigned long 不影响）。验收：prett2 缺口 0、prett size 120=120、
    //   四数 118/118/1/3/29 与改前逐项相同。
    // ③§6.5 串盲区真差异修复（旧形·已由 P8S1 换回宏，见上）
    //   `bool random_option::CRandomOptionItemHandle::change_option(itemIndex_t, ENUM_RARITY,
    //   int, int, int, int, RandomOption*)`（旧形展开为 `long unsigned int`，运行期日志文本不符）；
    // ④dwRaw 键按 `*reinterpret_cast<short*>(&dwRaw)` 直取（去 `movzwl+mov %ax,%临时`
    //   物化，442→438）。复测回滚：去 optB 具名局部（权威 ndiff 412→448）、while→for（同码）。
    // 残差（权威 DIFF 438/432、ndiff=424）：帧 0xe0/0xd0 与 optType(-0x53/-0x65)、
    // err(-0x20/-0x1c) 槽差（N82/N95）；remaining -= w 的 4.1 %ebx 跨调用保活 5 条 vs
    // 4.4 mem-sub 2 条；内层循环尾 break+回跳两条 jmp；sel<size 的 seta/setbe 极性 ⇒ N 封顶。
    // 不同权重表（GetRandomOption()+0xf4 = RandomOptionScript::m_mapf4）：
    // 键 = 低字节 rarity | 高字节 optType（ORIG 0x085f34d3/da 两次字节写 -0x6e/-0x6d
    // ⇒ 原码 union 字节视角）；P157：GetRandomOption() 每次重取（ORIG 四处独立 call）。
    DifferentWeightValue dwv;
    union
    {
        unsigned short m_w;
        unsigned char m_b[2];
    } dwRaw;
    dwRaw.m_b[0] = (unsigned char)rarity;
    dwRaw.m_b[1] = optType;
    // [L4b] ORIG 0x085f34dd 直接 `lea -0x6e(%ebp),%edx` 传 union 首址给 find
    // （无 short 成员读出的临时物化）；旧形 `find(dwRaw.m_w)` 令 4.4 先
    // `movzwl -0x62; mov %ax,-0x52` 再取临时地址（多 2 条）⇒ 显式按 short* 取址。
    std::map<short, DifferentWeightValue>::const_iterator dw =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.find(
            *reinterpret_cast<short*>(&dwRaw));
    if (dw == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_mapf4.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            // [P8S1 2026-09-11 P139 落地] 同上（宏展开 = ORIG prett 串）。
            __PRETTY_FUNCTION__,
            0x369, "different_weight_ (%d,%d) ",
            (int)rarity, (int)optType);
        return false;
    }
    dwv = dw->second;

    // 组值表（GetRandomOption()+0x160 = m_map160）。
    std::map<int, OptionGroupValue>::const_iterator ogIt =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.find(outGroup);
    if (ogIt == reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map160.end())
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            // [P8S1 2026-09-11 P139 落地] 同上（宏展开 = ORIG prett 串）。
            __PRETTY_FUNCTION__,
            0x371, "option_group_ (%d) ", outGroup);
        return false;
    }

    int remaining = ogIt->second.m_fieldC;
    std::vector<OptionGroup> groupVec = ogIt->second.m_vec0;   // 值拷贝后 erase
    // [L4b 复测·回滚] 去 optB 具名局部（逐点重取参数）：权威口径 ndiff 412→448 反劣
    // （条数恒 440）⇒ 维持具名局部形。ORIG 的 `mov 0x24(%ebp),%ecx` 形属 4.1 参数槽
    // 直读，4.4 无该局部时寄存器分配走坏（%ebx 提前保活扰动全块）。
    unsigned char* optB = reinterpret_cast<unsigned char*>(opts);
    // ORIG 0x085f3705：迭代器默认构造（C1Ev）在循环之前 ⇒ 先声明后赋值（P50 复活）。
    std::vector<OptionGroup>::iterator selIt;

    // 第一组已占槽（非本次 slot）按其组类型扣权重并移出候选。
    // （i 的 ORIG 预清形 0x085f3674 `movl $0x0,-0xc` 属上述已回滚组。）
    for (int i = 0; i <= 2; ++i)
    {
        // [L4b] P243 书写序：ORIG 0x085f3733 `mov 0x20(%ebp),%eax; cmp -0xc(%ebp),%eax;
        // je`（eax=slot，cmp i）⇒ 源码槽序写 slot==i。
        if (slot == i || optB[i * 3] == 0)
        {
            continue;
        }
        // ORIG 0x085f36b7：begin 在 if 内、while 形、`(*it).m` 走 deEv（P167 新判据）。
        // [L4b 实测] for 形（++it 在 for 头）与 while 形同码（438/ndiff 424 不变）⇒
        // 维持 while 形；尾部两条 jmp（break + 回跳）属 4.4 控制流布局，记 N。
        std::vector<OptionGroup>::iterator it = groupVec.begin();
        while (it != groupVec.end())
        {
            if ((unsigned int)(*it).m_type0 == (unsigned int)optB[i * 3])
            {
                remaining -= (*it).m_weight4;
                groupVec.erase(it);
                break;
            }
            ++it;
        }
    }

    unsigned long bound = (unsigned long)remaining;
    int pick = m_rand[9].randInt(bound);          // m_rand+0x5808 → 池[9]

    CRandomList<unsigned int> chooser((unsigned int)pick);
    std::for_each(groupVec.begin(), groupVec.end(), chooser);

    unsigned int sel = chooser.get_value();
    // [D04 轴 B/P196 逐点自校 · 已试并回滚] ORIG 0x085f383e `cmp -0x10(%ebp),%eax;
    // setbe %al; test; je <fail 块>` = `sel >= size()` guard 形；我方 `if (sel < size())`
    // 产互补 `seta`。按 Main 轴 B 协议试 guard 形（失败块后置）：**ours 442→444（+2）反劣，
    // 四数不变** ⇒ 已回滚。两 cc 均属无符号序（非 P196 符号性分叉），差异为 4.1/4.4 的
    // 条件取反 fold（ORIG 用 setbe 走 fail 岛、4.4 用 seta 直落），源形不可择 ⇒ N。
    if (sel < groupVec.size())
    {
        // ORIG 0x085f38f8：begin()+sel 再 deEv（非 operator[]；P167 判据）。
        selIt = groupVec.begin() + sel;
        optB[0xa] = (unsigned char)(*selIt).m_type0;    // 第二组槽首字节=组类型
        optB[0xb] = (unsigned char)_get_option_value(dwv.m_field0, dwv.m_field4);
        optB[0xc] = (unsigned char)_get_option_value(dwv.m_field0, dwv.m_field4);
        optB[0xd] = (unsigned char)((optB[0xd] & 0xfc) | (slot & 3));
        unsigned long seedBound = 0x40;
        // [L4b] P183 去 seedRoll 具名局部：ORIG 0x085f3945 直接把 randInt 结果
        // 内联进表达式（`mov %eax,%edx; and $0x3f,%edx; lea 0(,%edx,4),%ecx`，
        // 无独立槽、无「存槽再读」往返）。
        optB[0xd] = (unsigned char)((optB[0xd] & 3) |
                                    ((m_rand[10].randInt(seedBound) & 0x3f) << 2));
    }
    else
    {
        LogManager::logFormat(
            1, "RandomOption.cpp",
            // [P8S1 2026-09-11 P139 落地] 同上（宏展开 = ORIG prett 串）。
            __PRETTY_FUNCTION__,
            0x390, "_option_group.size() %d, %d ", sel,
            (unsigned int)groupVec.size());
        return false;
    }
    return true;
}