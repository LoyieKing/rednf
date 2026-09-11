bool CSyncScript::insert_skill_info(STSkillScript* script, int job, int type)
{
    // [b28d-solo-sync_script] 判词：**语义完成、byte 层 N 封顶**（§6.2 三件套）。
    //   ORIG @0x08622e08：①gap = 1（1401 vs 1400）；②P167 调用点差集 CLEAR
    //   （258=258，含全部 CStringMaker/for_each/GetEscapeString/CStreamBuffer 调用）；
    //   ③槽位集残差 = 浅槽临时区整体 +0x1c（cMyTrace/string 物化序；牵动 0xb000
    //   缓冲基址 -0xa924 vs ORIG -0xa908 全部 29 处，整帧重排风险>收益，Main 已批复
    //   挂起）+ for_each 临时物化 1-2 处。归 N82（4.1 不按 lifetime 复用 vs 4.4 复用）
    //   + N78（memset 0xb000 展开形）。逐块人工比对：Ghidra C 段与重建代码逐句对应，
    //   无缺失分支/字段/调用。
    // [S3/M3FreeNext15B 2026-09-11 复扫] 语义轴（本波新钥匙）：§6.5 串审计 CLEAR
    //   —— 两侧 literal 多重集全等（ORIG 8 kinds/62 refs = 我方 8/62）：SQL 串
    //   `inSert into skill_info(...) values(%s)` @ORIG 0x8ce3eb4 与我方 .rodata+0x39c
    //   **逐字节相等**（jobs/列序/占位符全同）；`Error!!! %s->%s [%s][%s][%d]`
    //   @0x8ce2f4d = 我方 +0x2f5；`__PRETTY_FUNCTION__` @0x8ce5700 = 我方 +0x10c0；
    //   分隔串 `, '`/`', '`/`;`/`'`/`,` 全部命中。cMyTrace 两处变参序
    //   （script→database→m_str4/m_str8→PRETTY→0x7ff/0x807）与 ORIG 逐条一致；
    //   §6.5.1 编码扫描（enc_kr_scan.py）全域 0 处不匹配。codegen：p167 仍 CLEAR
    //   （258=258）；Main 已批复**挂起**（浅槽临时区整体 +0x1c 牵动 0xb000 缓冲基址
    //   29 处，整帧重排风险>收益）⇒ 本波**不动 codegen**，现场读数 1400/1391。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG：localVec = m_vec44/m_vec50 交替展开（pre_required_skill 段）。
    std::vector<int> localVec;
    for (unsigned int i = 0; i < script->m_vec44.size(); ++i)
    {
        localVec.push_back(script->m_vec44[i]);
        localVec.push_back(script->m_vec50[i]);
    }
    // GetEscapeString 输出区：ORIG 栈上单块 0xb000 缓冲（入口即 memset），
    // 按 escCount*0x1000 步进取槽（共 7 次）；name/name2 转码缓冲各 0x400。
    char escapedBuf[0xb000];
    memset(escapedBuf, 0, sizeof(escapedBuf));
    int escCount = 0;
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N78/N82）→ 试=4 形地址改写 A/B：
    //   原 `escapedBuf + escCount++ * 0x1000`（our 1391/gap +9）→
    //   `(char*)escapedBuf + …`（回到 +9，无效）→ **`&escapedBuf[… * 0x1000]`**（下标形）
    //   与 `&escapedBuf[… << 12]`（位移形）**均 our 1397 / gap +3**（等价，二者取一）；
    //   每处地址计算：4.4 由 `mov i,%eax; shl $0xc,%eax; mov %eax,%edx; lea; add` 改为
    //   `mov i,%eax; mov %eax,%edx; shl $0xc,%edx; lea; add`（末 3 处折 `lea (%eax,%edx,1)`）。
    //   TU 四数 168/296/2/2/69 逐项不变（零劣化）⇒ 推翻 N78「地址计算形不可造」的部分。
    //   残余 3 = ① ORIG `mov -0x20(%ebp),%edx; lea; shl %edx; add` 的寄存器直选
    //   （4.4 恒先装 %eax 再转 %edx，同 OnEnterState 0x1d4 中转族）②尾块
    //   `mov $0x0/%0x1,%ebx; jmp` 判定块序（4.4 直落）⇒ N82 维持。
    #define SKILL_ESC_SLOT (&escapedBuf[escCount++ * 0x1000])
    char name[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_str4.c_str(), name))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7ff, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_str4.c_str(),
            __PRETTY_FUNCTION__, 0x7ff);
    }
    char name2[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_str8.c_str(), name2))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x807, 5)(
            "Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
            CodePage::database(), script->m_str8.c_str(),
            __PRETTY_FUNCTION__, 0x807);
    }
    // GetEscapeString 输出区声明见函数头；流对象在 name2 处理后构造。
    CStreamBuffer sb;
    // 1)job_index 2)skill_index(m_field0) 3)module_type(type)
    sb << job;
    sb << ", " << script->m_field0;
    sb << ", " << type;
    // 4)name 5)name2（转码后 escape；GetEscapeString 结果写入槽缓冲）
    // [b28e] ORIG 0x086230ed 段形：`call GetEscapeString; mov %eax,%ebx;` 后紧跟
    // `<< ", '"` → `<< %ebx` → `<< "'"` —— **直接用返回值**、全程寄存器保活，
    // 无 escName 具名指针槽（P0/P183；具名槽令 4.4 多发 `mov %eax,-0x38(%ebp)` +
    // 回读并把 `add %edx,%eax` 折成 `lea (%edx,%eax,1)`，帧 +0x20）。
    sb << ", '" << GetEscapeString(db, name, SKILL_ESC_SLOT) << "'";
    sb << ", '" << GetEscapeString(db, name2, SKILL_ESC_SLOT) << "'";
    // 6)basic_explain(+0x10 先于 +0xc) 7)skill_explain(+0xc)
    sb << ", '" << GetEscapeString(db, script->m_str10.c_str(), SKILL_ESC_SLOT)
       << "'";
    sb << ", '" << GetEscapeString(db, script->m_strc.c_str(), SKILL_ESC_SLOT)
       << "'";
    // 8)m_vec18
    {
        // ORIG 站点形态：for_each 返回临时 + 实参临时同在一个 full-expression
        // 内，两个 D1 均在 << 链之后（0862330c→0862332f/08623351）。
        sb << ", '"
           << std::for_each(script->m_vec18.begin(), script->m_vec18.end(),
                            CStringMaker()).c_str()
           << "'";
    }
    // 9)+0x5c 10)+0x60
    sb << ", " << script->m_5c;
    sb << ", " << script->m_60;
    // 11)m_arr7c[6]
    {
        sb << ", '" << std::for_each((int*)script->m_arr7c, (int*)script->m_arr7c + 6, CStringMaker()).c_str() << "'";
    }
    // 12)growType[0..4].m_8 五段分号连接（单引号包裹）
    {
        // ORIG：五段临时对象同在一个 full-expression（0xa0..0xc8 栈槽），
        // GCC 右到左求值 ⇒ for_each 构造序 +0xdc(growType[4])→+0x9c([0])
        // （0862347f..08623645），每段 for_each 后立即 c_str；10 个临时
        // 在 << 链全部完成后成对（res_i, arg_i，i=0..4）析构（086236e8+）。
        sb << ", '"
           << std::for_each(script->m_growType[0].m_8,
                            script->m_growType[0].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[1].m_8,
                            script->m_growType[1].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[2].m_8,
                            script->m_growType[2].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[3].m_8,
                            script->m_growType[3].m_8 + 2, CStringMaker()).c_str()
           << ";"
           << std::for_each(script->m_growType[4].m_8,
                            script->m_growType[4].m_8 + 2, CStringMaker()).c_str()
           << "'";
    }
    // 13)m_vec1dc（lea 0x1dc@08623889）14)m_vecf4（lea 0xf4@0862399a）
    {
        sb << ", '" << std::for_each(script->m_vec1dc.begin(), script->m_vec1dc.end(), CStringMaker()).c_str() << "'";
    }
    {
        sb << ", '" << std::for_each(script->m_vecf4.begin(), script->m_vecf4.end(), CStringMaker()).c_str() << "'";
    }
    // 15)m_useItems11c（"%d,%d,%d"）
    {
        sb << ", '" << std::for_each(script->m_useItems11c.begin(), script->m_useItems11c.end(), CStringMaker()).c_str() << "'";
    }
    // 16)+0x30 17)+0x34
    sb << ", " << script->m_30;
    sb << ", " << script->m_34;
    // 18)localVec（pre_required_skill）
    {
        sb << ", '" << std::for_each(localVec.begin(), localVec.end(), CStringMaker()).c_str() << "'";
    }
    // 19)m_f100[2]（for_each<float*> +0x100..+0x108，"%f"）
    {
        sb << ", '" << std::for_each((float*)script->m_f100, (float*)script->m_f100 + 2, CStringMaker()).c_str() << "'";
    }
    // 20)m_arr138[2]
    {
        sb << ", '" << std::for_each((int*)script->m_arr138, (int*)script->m_arr138 + 2, CStringMaker()).c_str() << "'";
    }
    // 21)m_i108[2..3]（for_each<int*> +0x110..+0x118）
    {
        sb << ", '" << std::for_each((int*)script->m_i108 + 2, (int*)script->m_i108 + 4, CStringMaker()).c_str() << "'";
    }
    // 22)icon：esc(str160[0]),arr168[0],esc(str160[1]),arr168[1]
    // [b28e] ORIG 0x08623f30/0x08623f78：**两次 GetEscapeString 都直用返回值**
    // （先 %ebx=m_str160[1] 的结果、再 %esi=m_str160[0] 的结果，全无具名槽）；
    // 链中位置为 icon0 第 3 位 / icon1 第 6 位，GCC 右→左求值恰好复现 ORIG 的
    // 「先算 m_str160[1]、再 arr168[0]、再 m_str160[0]」调用序。
    sb << ", '" << GetEscapeString(db, script->m_str160[0].c_str(), SKILL_ESC_SLOT)
       << "," << script->m_arr168[0] << ","
       << GetEscapeString(db, script->m_str160[1].c_str(), SKILL_ESC_SLOT)
       << "," << script->m_arr168[1] << "'";
    // 23)command_key_explain：esc(m_str194)（同 P183：返回值直用，无具名槽）
    sb << ", '"
       << GetEscapeString(db, script->m_str194.c_str(), SKILL_ESC_SLOT) << "'";
    // 24)skill_command_advantage：m_240,m_244
    sb << ", '" << script->m_240 << "," << script->m_244 << "'";
    // 25)static_data：m_vec198（lea 0x198@086240e4）
    {
        sb << ", '" << std::for_each(script->m_vec198.begin(), script->m_vec198.end(), CStringMaker()).c_str() << "'";
    }
    // 26)level_info：m_vec1a8（外层 for_each<vector<float>*> → operator()(vector<float>&)）
    {
        sb << ", '" << std::for_each(script->m_vec1a8.begin(), script->m_vec1a8.end(), CStringMaker()).c_str() << "'";
    }
    // 27)start_cool_time：m_21c,m_220
    sb << ", '" << script->m_21c << "," << script->m_220 << "'";
#undef SKILL_ESC_SLOT
    db->set_query("inSert into skill_info(job_index, skill_index, module_type, name, name2, basic_explain, skill_explain, purchase_cost, type, skill_class, growtype_maximum_level, second_growtype_maximum_level, skill_fitness_growtype, skill_fitness_second_growtype, consume_item, required_level, required_level_range, pre_required_skill, consume_mp, cool_time, casting_time, icon, command_key_explain, skill_command_advantage, static_data, level_info, start_cool_time) values(%s)",
                  sb.c_str());
    return db->exec(true);
}