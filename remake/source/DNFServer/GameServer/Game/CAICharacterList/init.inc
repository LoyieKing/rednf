// ---- init（ORIG 0x0834a28c）：truncate 失败打印并返 false；m_maxAP 置
//      0x7fffffff；遍历文件列表逐条 clear→new→import（失败 printf+析构释放+
//      返 false）→FindIndex→insert_db（失败 printf+返 false，ORIG 不释放
//      aiChar）→独立掉落收集（push_back 到 script.m_f2c）→set（重新
//      FindIndex）→_add。----
bool CAICharacterList::init()
{
    if (!sync_script::GetInstanceSyncScript()->truncate_aicharacter_info_table())
    {
        std::puts("Fail truncate_aicharacter_info_table");
        return false;
    }

    m_maxAP = 0x7fffffff;   // +0x4bc（ORIG 0x0834a2c5 `movl $0x7fffffff,0x4bc(%eax)` 直落）

    // ORIG 0x0834a2cf：fileName 在循环外置 0（P28 声明序 = 槽序）。
    const char* fileName = 0;
    // [b28d-L14 实证] ORIG 0x0834a28c：非 const _Rb_tree_iterator + 默认构造
    // （C1Ev）+ 后置 it++（ppEi）——P50 复活 / P28 增量侧判据。
    std::map<int, std::string>::iterator it;
    STAICharacterCommonScript script;
    it = g_aiCharacterScriptFileList.m_fileMap.begin();
    while (it != g_aiCharacterScriptFileList.m_fileMap.end())
    {
        fileName = it->second.c_str();
        if (fileName == 0)
            break;

        script.clear();

        CAICharacter* aiChar = new CAICharacter;
        if (!importAICharacterCommonScript(&script, fileName, false))
        {
            std::printf("AI charac Script Error: %s\n", fileName);
            delete aiChar;                    // ORIG 0x0834a3b3 test/je + D1 + _ZdlPv
            return false;
        }

        const int fileIdx =
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName);
        if (!sync_script::GetInstanceSyncScript()->insert_aicharacter_info_to_db(fileIdx, script))
        {
            std::printf("insert_aicharacter_category Error: %s\n", fileName);
            return false;                     // ORIG 此路径不释放 aiChar（忠实保留）
        }

        // 独立掉落收集（G_CDataManager()+0xa890 = m_independentDrop.m_vecC）。
        // ORIG 0x0834a4a9：push_back 目标是 script+0xf2c（m_f2c），非局部 vector；
        // 0x0834a4b2 cmp/setg 为有符号比较 ⇒ 下标 int；每字段独立 operator[]
        // （P157，不缓存元素引用）。
        std::vector<stIndependentDropInfo>& drops =
            G_CDataManager()->m_independentDrop.m_vecC;
        for (int i = 0; i < (int)drops.size(); ++i)
        {
            if (drops[i].m_field0 == 1 && drops[i].m_monsterIndex == fileIdx)
                script.m_f2c.push_back(drops[i].m_drop);
        }
        // [S2CAICharList 2026-09-11 负结论①] 上面的 push_back 触发本 TU 两个 weak 符号
        //   _ZN26stMonsterIndependentDrop_tC1ERKS_（ORIG 0x08376bb2，58 条）与
        //   _ZN26stMonsterIndependentDrop_taSERKS_（ORIG 0x083a3942，59 条）；本 TU 52/53
        //   条（gap=+6，DIFF）。根因**不是**编译器分块差异，而是 CDataManager.h:398 的成员
        //   划分：ORIG 的 +0x04..+0x2c 是 4 个独立成员（边界 5/4/1/1，与语义分组一致：
        //   分档掉率×5 / 期望值×4 / 数量上限 / 主掉率），非 int[11] 一体数组。
        //   探针实证：int[5]+int[4]+int+int ⇒ C1ERKS_/aSERKS_ 与 ORIG **逐条一致**（含每块
        //   结尾 `mov 0x14(%edx),%edx` 破 src 基址、随后重载）；int[11] ⇒ 11 连发（我方现状）；
        //   union 数组视图（保留 m_rate[11] 名）⇒ 仍 11 连发（GCC 按最大成员整体拷贝）。
        //   ⇒ 可修但**不能只在本 TU 内修**：拆成员会破坏 3 个其它 TU 的 `m_rate[i]` 访问
        //   （MonsterIndependentDrop.cpp / DomainMiscDeps.cpp / GmCmds_D.cpp），需父泳道协调。
        // [负结论②] _ZNSt20__uninitialized_copyILb0EE…<__normal_iterator<const pair<int,int>*
        //   …>,pair<int,int>*>（ORIG 0x0823eade）：我方 56 条 vs ORIG 39 条，差全在 EH
        //   landing pad（我方循环体 39 条与 ORIG 逐条一致）。探针：同头集 + -fno-exceptions
        //   ⇒ size=0x6f = ORIG 的 nm size 0x6f，n1_v verdict=AE（39/39）；带 -fexceptions
        //   恒 0xa5/56 条。ORIG 本 TU 确有 EH（_add 0x0834a8a8 `call _Unwind_Resume`）⇒
        //   ORIG 链接时该 COMDAT 取自另一个 -fno-exceptions TU（PacketBuf.cpp 族），per-TU
        //   工具按符号名归属给本 TU ⇒ **工具口径缺口（weak COMDAT 跨 TU 归属）**，源码无解。

        aiChar->set(
            g_aiCharacterScriptFileList.FindIndexByFullScan(fileName), script);
        _add(aiChar);
        it++;
    }
    return true;
}