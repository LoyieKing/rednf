// ===========================================================================
// importCommonSkillIndexes —— ORIG T 0x08913f48
// （docs/class_func_reports/_lt_global_gt_/importCommonSkillIndexes.md）
// 清空两输出容器后按脚本填充："[common skill]" 后连续 ScanInt 进 vector<int>；
// "[except skill info]" 块内每轮 ScanStr(jobName)→getCharacterJob + 连续
// ScanInt 进 set<int>，插入 map<job,set>；loadRDARScriptFile 失败记日志返 0，
// 成功返 1。
// 验收：差集 35/35 归零；残余为 set/pair 临时链槽位序（N82）+ 块排布镜像。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] NEAR N95（临时对象槽序）：ORIG 0x08913f48。
// 本波修：①token/ok 声明提到两 clear 之前、job 提到 skills 之前（P309 先声明浅）
// ⇒ 局部槽 -0x54/-0x55/-0x5c/-0x60/-0x78/-0x7c/-0x7d/-0x84 全部归位；
// ②okJob 守卫改正极性 if/else-break 形 ⇒ `test;je` 与 ORIG 同助记符。
// 残余 103 条 = 临时对象槽布局：ORIG 4.1 按使用序分配（skills.insert 返回值
// -0x10 → make_pair sret -0x2c → pair 临时 -0x48 → map insert 返回值 -0x50），
// 4.4 按对象大小降序（28B -0x24 → 24B -0x40 → 8B -0x48 → 8B -0x50）；
// 两者对象集合相同、语义全同，无源码旋钮（P178 具名 entry 反致全帧平移）
// ⇒ N95/N82 族封顶。p167 CLEAR（35/35），语义闭合。
int importCommonSkillIndexes(
    const char* path,
    std::vector<int>& commonSkill,
    std::map<ENUM_CHARACTERJOB, std::set<int> >& exceptSkill)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              // Q7S4: ORIG 0x08913f74 movl $0x8e103d8 = 完整前缀串（我方此前仅 "%s"）
                              __FUNCTION__, 0x329a,
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }
    // [b28d-solo] ORIG 声明序 token/ok 先于两 clear，job 先于 skills（P309 槽序）。
    std::string token;
    bool ok = false;                        // ORIG -0x55 函数级一次初始化
    commonSkill.clear();
    exceptSkill.clear();
    for (;;)
    {
        if (!ScanType(token, true))
            break;                              // ORIG 共享出口（P63）
        if (token == "[common skill]")
        {
            for (;;)
            {
                int v = ScanInt(&ok);
                if (!ok)
                    break;
                commonSkill.push_back(v);
            }
        }
        else if (token == "[except skill info]")
        {
            ENUM_CHARACTERJOB job = ENUM_CHARACTERJOB(0xb);   // ORIG -0x60 外层一次
            std::set<int> skills;                       // ORIG -0x78 外层块构造一次
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (token == "[/except skill info]")
                    break;
                job = ENUM_CHARACTERJOB(0xb);           // ORIG 每轮 movl $0xb,-0x60
                skills.clear();
                std::string jobName;                    // ORIG -0x7c 每轮构造
                if (!ScanStr(&jobName))
                    return 0;
                job = ENUM_CHARACTERJOB(getCharacterJob(jobName.c_str()));
                bool okJob;                             // ORIG -0x7d 无初始化
                for (;;)
                {
                    int v = ScanInt(&okJob);
                    if (okJob)
                        skills.insert(v);
                    else
                        break;
                }
                exceptSkill.insert(std::make_pair(job, skills));
            }
        }
        else if (token == "[/common skill]")
        {
        }
    }
    return 1;
}