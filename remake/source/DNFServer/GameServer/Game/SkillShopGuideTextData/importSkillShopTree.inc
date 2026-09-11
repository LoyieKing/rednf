// ============================================================================
// importSkillShopTree —— ORIG T 0x0891427f（范围 [0x0891427f, 0x08914482]）
// 报告 docs/class_func_reports/_lt_global_gt_/importSkillShopTree.md
// 返回 int：0=文件加载失败，1=成功。与 importSkillShopGuideText 同属
//   RDARScriptEtcParameter.cpp 日志域；装载 SkillShopTree.etc 后 clear out，
//   逐 token："[skill tree]" 段内循环 ScanStr 职业名 → getCharacterJob 转枚举、
//   ScanStr 树名，pair 构造后 insert 进 out map；ScanStr 失败跳出内层循环回到
//   外层 do；遇到 "[/skill tree]" 或 EOF 结束返回 1。
// ---------------------------------------------------------------------------
// b28d-solo-SkillShopGuideTextData 判词：语义完成（p167 CLEAR 24=24；帧 0x50、
//   槽位 token/-0x20、bool/-0x9、job/-0x24、treeStr/-0x28、jobStr/-0x2c、
//   pair/-0x14、insert 返回/-0x1c 与 ORIG 全等）。byte 层 N 封顶：内层 ScanStr
//   检查 ORIG 为「xor $1;test;je body;jmp outer」两跳形，4.4 恒产
//   「xor $1;test;jne outer」单跳 + 中继 nop 岛（P164 循环旋转族）；外层/析构段
//   另有 N7 %esi↔%ebx 角色差。源码形不可择，不再耗轮次。
// ============================================================================
int importSkillShopTree(
    const char* path,
    std::map<ENUM_CHARACTERJOB, std::string, std::less<ENUM_CHARACTERJOB>,
             std::allocator<std::pair<ENUM_CHARACTERJOB const, std::string> > >& out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x32eb,
                              "ERROR : %s \xed\x8c\x8c\xec\x9d\xbc\xec\x9d\x84 \xec\xb0\xbe\xec\x9d\x84\xec\x88\x98 \xec\x97\x86\xec\x8a\xb5\xeb\x8b\x88\xeb\x8b\xa4.", path);
        return 0;
    }
    // 声明序/clear 位置按 ORIG 0x089142de..0x08914309：token(-0x20)、treeStr(-0x28)、
    // jobStr(-0x2c) 三个 string 构造完成后才调 out.clear()。
    std::string token;
    // ORIG 0x089142e9 有 movb $0x0,-0x9(%ebp)（token 构造后的一次性 bool 清零，
    // 全函数不再读写）= 原码具名 bool 局部死初始化，照抄（P171/P232）。
    bool parseOk = false;
    (void)parseOk;
    // ORIG 0x08914374 用 mov %eax,-0x24(%ebp) 写该值 ⇒ 原码 job 声明在函数头
    // （槽 -0x24 夹在 token 与 treeStr 之间），循环内仅赋值。
    ENUM_CHARACTERJOB job;
    std::string treeStr;
    std::string jobStr;
    out.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[skill tree]")
        {
            for (;;)
            {
                // 4.4 对本形产「xor $1;test;jne 共享出口」；ORIG 为
                // 「xor $1;test;je body;jmp outer」两跳——P164 旋转族不可择
                // （X1/裸内联/语句表达式/goto 尾标签四形实测均不闭合）。
                if (!ScanStr(&jobStr))
                {
                    break;
                }
                else
                {
                    job = (ENUM_CHARACTERJOB)getCharacterJob(jobStr.c_str());
                    if (!ScanStr(&treeStr))
                    {
                        break;
                    }
                    else
                    {
                        // ORIG 0x0891439f：直接就地构造 value_type
                        // （_ZNSt4pairIK17ENUM_CHARACTERJOBSsEC1IRS0_RSsEEOT_OT0_ =
                        //   pair<const ENUM_CHARACTERJOB,string> 的 pair<JOB&,string&> 构造）
                        // 后 map::insert(value_type const&)；make_pair 会多出转换临时。
                        out.insert(
                            std::map<ENUM_CHARACTERJOB, std::string,
                                     std::less<ENUM_CHARACTERJOB>,
                                     std::allocator<std::pair<ENUM_CHARACTERJOB const,
                                     std::string> > >::value_type(job, treeStr));
                    }
                }
            }
            continue;
        }
        if (token == "[/skill tree]")
            break;
    }
    return 1;
}