// ============================================================================
// importPairPvPStatScript —— ORIG T 0x08977d9c（init.md:4936，
// this+0x4df8 ← "Etc/FairPvPStat.etc"）。恒返回 0（ORIG 全路径 ebx=0）。
// [grow type index] 以 (job,grow) 为键在 m_map0 插入清零 CharacterStatus，
// 后续段直接写该值字段；[attack speed]/[jump speed] 无 cur 判空（ORIG 缺失，
// 引用恒非空故等价）。[jump speed] 读后丢弃（ORIG 同）。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N89/N82 封顶（原 DIFF 524）ORIG
//   0x08977d9c orig/ours=569/496，P167 CLEAR（56/56，§6.2 三件套①）。本波修：
//   insert 结果用 `pair<const_iterator,bool>`（ORIG 转换构造
//   `C1ISt17_Rb_tree_iteratorIS4_EbEEOS_IT_T0_E` + const_iterator ptEv；
//   `const_cast` 取 current）。残余：STL 内联实例差（N89）+ 超大件槽差（N82）。
// [BigEquipScript pass-2 2026-09-09] orig/ours=569/551（gap 73→18）：
//   ①判空下沉：ORIG 17 个数值段各自 `cmpl $0,-0xc(%ebp); jne; mov $0,%ebx;
//     jmp`（[HP MAX] 0x08977fcb … [water resistance] 0x0897860f 逐段实证；
//     [attack speed]/[jump speed] 两段无判空，P171 照抄），原写成链前单一判空；
//   ②ScanType 失败返回当前 ok 值（ORIG 0x0897864d movzbl -0xc9(%ebp),%ebx），
//     非恒 false；③段序按 ORIG（inventory limit/MP regen/move speed/attack/
//     cast/hit/jump power/jump speed/weight/dark/light/fire/water）重排并补回
//     被误覆盖的 [weight] 段；④CDataManager.h CharacterStatus 的 m_field00/
//     m_field04/m_field44 int→unsigned（ORIG [HP MAX] 0x08977fe9 / [MP MAX]
//     0x08978044 / [move speed] 0x089782cb 三段 store 为 RHS-first 中转
//     `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,disp`，而同函数 +0x3c
//     [inventory limit] 0x08978214 / +0x50 [weight] 0x089784b8 为 LHS-first
//     ⇒ P211 判异型；同尺寸同偏移、sizeof 0x54/static_assert 不变；三消费 TU
//     CUser/CDataManagerScripts/InitScriptProducers_2 四数 A/B 实测零回退）。
//   残余 18 = ORIG 19 个 continue 跳板 `nop; jmp loop`（4.1 每个 continue 发
//   3 字节跳板；4.4 jump-threading 合并为共享 jmp，P223 两种 goto 形实测均
//   -1/-2 反劣）⇒ 封 N（跨版本 continue 代码生成差）。
// [sweep2-InitScriptProducers_2] 复核（§6.2 三件套）：fresh orig/ours=548/549
//   gap=−1；p167 calls=56/56，差集仅 C1↔C2 同址别名（P93 结构性噪音，非缺口）；
//   ORIG 18 个集中 continue 跳板（0x08977e02..0x08977e3b 各 `nop; jmp 0x08977e3c`）
//   vs 我方各段直接 jmp 循环头（ndiff 41 全在此）⇒ N 封顶，无真缺口。
bool importPairPvPStatScript(FairPvPStatScript& script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    script.clear();

    std::string token;
    bool ok = false;
    CharacterKind kind;
    CharacterStatus* current = 0;

    for (;;)
    {
        // ORIG 失败路径返回当前 ok 值（0897864c movzbl -0xc9(%ebp),%ebx），
        // 非恒 false（我方旧形 DIFF 1 条 movzbl）。
        if (!ScanType(token, true))
            return ok;

        if (token == "[job index]")
        {
            kind.m_job = (char)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }

        if (token == "[grow type index]")
        {
            kind.m_grow = (char)ScanInt(&ok);
            if (!ok)
                return false;

            CharacterStatus status;
            std::memset(&status, 0, sizeof(status));     // ORIG memset 0x54
            // ORIG 用 const_iterator 版 pair（p167 MISSING 转换构造
            // `C1ISt17_Rb_tree_iteratorIS4_EbEEOS_IT_T0_E` + const_iterator ptEv）。
            const std::pair<std::map<CharacterKind, CharacterStatus>::const_iterator,
                            bool> inserted =
                script.m_map0.insert(std::make_pair(kind, status));
            if (!inserted.second)
                return false;
            current = const_cast<CharacterStatus*>(&inserted.first->second);
            continue;
        }

        if (token == "[HP MAX]")
        {
            if (current == 0)
                return false;
            current->m_field00 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[MP MAX]")
        {
            if (current == 0)
                return false;
            current->m_field04 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[physical attack]")
        {
            if (current == 0)
                return false;
            current->m_field08[0] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[physical defense]")
        {
            if (current == 0)
                return false;
            current->m_field08[1] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[magical attack]")
        {
            if (current == 0)
                return false;
            current->m_field08[2] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[magical defense]")
        {
            if (current == 0)
                return false;
            current->m_field08[3] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[inventory limit]")
        {
            if (current == 0)
                return false;
            current->m_field3c = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[MP regen speed]")
        {
            if (current == 0)
                return false;
            current->m_field42 = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[move speed]")
        {
            if (current == 0)
                return false;
            current->m_field44 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[attack speed]")                   // ORIG 此段无判空
        {
            current->m_field48 = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[cast speed]")
        {
            if (current == 0)
                return false;
            current->m_field4a = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[hit recovery]")
        {
            if (current == 0)
                return false;
            current->m_field4c = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[jump power]")
        {
            if (current == 0)
                return false;
            current->m_field4e = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[jump speed]")                     // ORIG 读后丢弃
        {
            ScanInt(&ok);
            continue;
        }
        if (token == "[weight]")
        {
            if (current == 0)
                return false;
            current->m_field50 = ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[dark resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[6] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[light resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[7] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[fire resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[4] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
        if (token == "[water resistance]")
        {
            if (current == 0)
                return false;
            current->m_field08[5] = (short)ScanInt(&ok);
            if (!ok)
                return false;
            continue;
        }
    }
    return false;
}