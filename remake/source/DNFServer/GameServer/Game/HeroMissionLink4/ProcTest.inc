// ---------------------------------------------------------------------------
// WongWork::CCompoundAvatar::ProcTest @ 0x08333fda
// ---------------------------------------------------------------------------
// [b28d-D33 P28/P61 封顶] ORIG 0x08333fda：142 vs 143 条；p167 差集 = ne/make_pair
// 非 const(MISSING) vs eq/make_pair const(EXTRA)（P61/P81 左值性 + P220 极性）。
// [F2d-C05② 甄别] 无命中形：gap=+1（show_func_diff orig=143 our=142，我们**少**）
//   ⇒ F2 减指令方向相反（残余 ORIG-only={mov×2} / OURS-only={add}）。维持封顶。
// [P1 round5 二轮·部分解封] ① **分支极性**：ORIG 侧实例化 `_ZNKSt17_Rb_tree_iteratorISt4pairIKiiEEneERKS3_`
//   （operator!=）⇒ 原码 `if (rit != result.end()) ++rit->second; else result.insert(...);`
//   → 换极性后 p167 的 ne 条目消失、TU identical 68→69 ✓ 保留。
//   ② **P0 反向（补具名局部）**：ORIG 0x08333fe9 序言把 `&m_tables[job]` 落 -0x18 槽
//   （this+job*0xc，`add %eax,%eax; add %edx,%eax; shl $2` 直证 sizeof(tagCompoundAvatarTable)=0xc）
//   ⇒ 原码含 `tagCompoundAvatarTable& table = m_tables[job];` → 补回后 fdiff blocks 17→10 ✓ 保留。
//   ③ **未解残差**（本轮真试后维持 N）：1) 乘法段 `mov (%eax,%ecx,4),%ecx; mov 0x14(%ebp),%eax;
//   imul %ecx,%eax` vs 我方 `mov (%eax,%ecx,4),%eax; imul 0x14(%ebp),%eax`——乘法操作数序两向实测
//   不改 codegen；2) `cmp -0xc(%ebp),%eax`（4.1 内存比较折叠）——交换比较两侧实测 blocks 10→12 反劣；
//   3) 剩余槽整体差 4（ORIG itemIdx=-0x40 / 我方 -0x3c）：我方另在浅端多 -0x4..-0xe 组槽、
//   帧下探 -0x5c（ORIG 最深 -0x44），属循环内临时槽分配模式差（N82/N126 族），无源码旋钮。
void WongWork::CCompoundAvatar::ProcTest(ENUM_CHARACTERJOB job,
                                         ENUM_EQUIPMENTTYPE equipType,
                                         int addRate, int bonusFlag,
                                         std::map<int, int>& result)
{
    // [P1 round5] P0 反向（ORIG **有**具名局部）：ORIG 0x08333fe9-0x0833fff 为
    //   `add %eax,%eax; add %edx,%eax; shl $0x2,%eax; add 0x8(%ebp),%eax;
    //    mov %eax,-0x18(%ebp); movl $0x1,0x4(%esp); mov -0x18(%ebp),%eax; mov %eax,(%esp);
    //    call getCompoundAvatarInfo` —— `&m_tables[job]`（this+job*0xc，sizeof
    //   tagCompoundAvatarTable=0xc 由 `mov %edx,%eax; add %eax,%eax; add %edx,%eax;
    //   shl $2` 直证）先落 -0x18 槽再作为 this 传入 ⇒ 原码含具名表引用。
    tagCompoundAvatarTable& table = m_tables[job];
    tagCompoundAvatar* info = table.getCompoundAvatarInfo(1);

    for (int loop = 0; loop < 10000; ++loop)
    {
        int itemIdx = 0;
        // 命中权重 = m_a[equipType] + addRate * m_e[equipType]
        // [P1 round5] 乘法操作数序（`m_e[eq] * addRate`）实测**不改 codegen**
        //   （ORIG `mov (%eax,%ecx,4),%ecx; mov 0x14(%ebp),%eax; imul %ecx,%eax`
        //   vs 我方 `mov (%eax,%ecx,4),%eax; imul 0x14(%ebp),%eax`，两向皆同形，
        //   fdiff blocks 17 不变）⇒ 4.4 固定按左操作数入 eax、右操作数作 mem 源，
        //   与源码序无关。已回滚。
        int rate = info->m_a[equipType] + addRate * info->m_e[equipType];
        if (bonusFlag == 1)
        {
            rate += G_CDataManager()->m_serverParameter.m_49ec;
        }

        unsigned long range = 10000;
        // [P1 round5] 交换比较两侧（`(unsigned)rate > randInt(...)`）实测**反劣**：
        //   blocks 10→12、ours 144→146 ⇒ 4.4 对 `randInt < rate` 的寄存器中转为固定
        //   形（ORIG 的 `cmp -0xc(%ebp),%eax` 是 4.1 的内存比较折叠，N 族）。已回滚。
        if ((unsigned)m_rand->randInt(range) < (unsigned)rate)
        {
            itemIdx = getItemIndex(info->m_optsA[equipType],
                                   get_rand_int(info->m_b[equipType]));
        }
        if (itemIdx == 0)
        {
            itemIdx = getItemIndex(info->m_optsB[equipType],
                                   get_rand_int(info->m_c[equipType]));
        }

        std::map<int, int>::iterator rit = result.find(itemIdx);
        // [P1 round5] p167 差集实证：ORIG 侧实例化的是 `_ZNKSt17_Rb_tree_iteratorISt4pairIKiiEEneERKS3_`
        //   （operator!=）+ **const** make_pair，我方是 eq + 非 const ⇒ 判词 P61/P81/P220 的
        //   左值性/极性根因在此分支：ORIG 原码为 `if (rit != result.end()) ++rit->second;
        //   else result.insert(...)`。本条即该形（真试一次新钥匙）。
        if (rit != result.end())
        {
            ++rit->second;
        }
        else
        {
            // [P1 round5] p167 第二层根因已定位但**不可两全**：
            //   ORIG 侧实例化 `_ZSt9make_pairIiiE…OS2_OS5_`（T1=T2=int 按值），我方是
            //   `_ZSt9make_pairIRiiE…OS3_OS6_`（T1=int&，因 itemIdx 是左值）。把第一实参
            //   显式转成右值（`static_cast<int>(itemIdx)`）后 **p167 差集确实闭合**（ProcTest
            //   条目消失），但件级从 142 条劣化到 **144 条**（ORIG 143；int& 版仅差 1 条），
            //   TU identical 69→68 ⇒ 按「四数不得劣化」回滚。留档：若日后允许以「符号形态」
            //   优先于「指令数」，该一行即是开关。
            result.insert(std::make_pair(itemIdx, 1));
        }
    }
}