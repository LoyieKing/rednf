int exportCharacterScript(STCharacterScript* script, const char* path)
{
    static char saveFileName[0x100];
    getScriptFileFullPath(saveFileName, 0x100,
                          g_szCharacterScriptBaseDirectory, path);
    FILE* file = fopen(saveFileName, "wb");
    if (!file)
        return 0;

    fprintf(file, "[job] `%s`\n", getCharacterJobName(ENUM_CHARACTERJOB(script->m_job)));
    unsigned int i = 0;                     // P191：ORIG 声明 + for-init 双初始化（0x8a1030d/0x8a1032a）
    fwrite("[growtype name]", 1, 0xf, file);
    for (i = 0; i < 6; ++i)                 // P2：ORIG cmpl $0x5 + setbe ⇒ unsigned 计数
        fprintf(file, " `%s`", script->m_growtypeName[i].c_str());
    fputc(10, file);
    fprintf(file, "[width] %d %d\n", script->m_width, script->m_height);
    fputc(10, file);
    fprintf(file, "[jump speed] %d\n", script->m_jumpSpeed);
    fputc(10, file);
    fwrite("[initial value]\n", 1, 0x10, file);
    exportCharacterStatusScript(file, script->m_status);
    exportCharacterGiveSkillScript(file, &script->m_skillList[0]);
    exportCharacterEtcStatsScript(file, &script->m_etcStats[0]);
    for (i = 1; i < 7; ++i)                 // ORIG 复用同一 unsigned i（槽 -0x1c，0x8a1046a 尾测 setbe）
    {
        fprintf(file, "\n[\xbe\xd5\xd0\xd1\xbc\xbc%d]\n", i);  // ORIG "\n[觉醒技%d]\n"
        // ORIG 0x8a1038b：`imul $0x57; add $0x20; add this; add $4` ⇒ 基址取
        // m_initialStatus(+0x24) + i*0x57（与 m_growStatus[i-1] 同址，P30 两步折叠形）。
        exportCharacterStatusScript(file, &script->m_status[i]);
        exportCharacterGiveSkillScript(file, &script->m_skillList[i]);
        exportCharacterEtcStatsScript(file, &script->m_etcStats[i]);
    }
    fputc(10, file);

    if (script->m_motionWaiting != "")
        fprintf(file, "[waiting motion] `%s`\n", script->m_motionWaiting.c_str());
    if (script->m_motionMove != "")
        fprintf(file, "[move motion] `%s`\n", script->m_motionMove.c_str());
    if (script->m_motionSit != "")
        fprintf(file, "[sit motion] `%s`\n", script->m_motionSit.c_str());
    if (script->m_motionDamage[0] != "")        // S248 STCharacterScript.h 数组化同步
        fprintf(file, "[damage motion 1] `%s`\n", script->m_motionDamage[0].c_str());
    if (script->m_motionDamage[1] != "")
        fprintf(file, "[damage motion 2] `%s`\n", script->m_motionDamage[1].c_str());
    if (script->m_motionDown != "")
        fprintf(file, "[down motion] `%s`\n", script->m_motionDown.c_str());
    if (script->m_motionOverturn != "")
        fprintf(file, "[overturn motion] `%s`\n", script->m_motionOverturn.c_str());
    if (script->m_motionJump != "")
        fprintf(file, "[jump motion] `%s`\n", script->m_motionJump.c_str());
    if (script->m_motionJumpattack != "")
        fprintf(file, "[jumpattack motion] `%s`\n", script->m_motionJumpattack.c_str());
    if (script->m_motionRest != "")
        fprintf(file, "[rest motion] `%s`\n", script->m_motionRest.c_str());
    if (script->m_throwMotion1[0] != "")
        fprintf(file, "[throw motion 1-1] `%s`\n", script->m_throwMotion1[0].c_str());
    if (script->m_throwMotion2[0] != "")
        fprintf(file, "[throw motion 1-2] `%s`\n", script->m_throwMotion2[0].c_str());
    if (script->m_throwMotion1[1] != "")
        fprintf(file, "[throw motion 2-1] `%s`\n", script->m_throwMotion1[1].c_str());
    if (script->m_throwMotion2[1] != "")
        fprintf(file, "[throw motion 2-2] `%s`\n", script->m_throwMotion2[1].c_str());
    if (script->m_throwMotion1[2] != "")
        fprintf(file, "[throw motion 3-1] `%s`\n", script->m_throwMotion1[2].c_str());
    if (script->m_throwMotion2[2] != "")
        fprintf(file, "[throw motion 3-2] `%s`\n", script->m_throwMotion2[2].c_str());
    if (script->m_throwMotion1[3] != "")
        fprintf(file, "[throw motion 4-1] `%s`\n", script->m_throwMotion1[3].c_str());
    if (script->m_throwMotion2[3] != "")
        fprintf(file, "[throw motion 4-2] `%s`\n", script->m_throwMotion2[3].c_str());
    if (script->m_motionDash != "")
        fprintf(file, "[dash motion] `%s`\n", script->m_motionDash.c_str());
    if (script->m_motionDashattack != "")
        fprintf(file, "[dashattack motion] `%s`\n", script->m_motionDashattack.c_str());
    if (script->m_motionGetitem != "")
        fprintf(file, "[getitem motion] `%s`\n", script->m_motionGetitem.c_str());
    if (script->m_motionBuff != "")
        fprintf(file, "[buff motion] `%s`\n", script->m_motionBuff.c_str());

    if (!script->m_attackMotion.empty())
    {
        fwrite("[attack motion]\n", 1, 0x10, file);
        for (std::vector<std::string>::iterator it = script->m_attackMotion.begin();
             it != script->m_attackMotion.end(); it++)   // P167/P28：ORIG deEv + ppEi
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack motion]\n", 1, 0x11, file);
    }
    if (!script->m_etcMotion.empty())
    {
        fwrite("[etc motion]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it = script->m_etcMotion.begin();
             it != script->m_etcMotion.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc motion]\n", 1, 0xe, file);
    }
    fputc(10, file);

    if (script->m_jumpattackInfo != "")
        fprintf(file, "[jumpattack info] `%s`\n", script->m_jumpattackInfo.c_str());
    if (script->m_dashattackInfo != "")
        fprintf(file, "[dashattack info] `%s`\n", script->m_dashattackInfo.c_str());
    if (!script->m_attackInfo.empty())
    {
        fwrite("[attack info]\n", 1, 0xe, file);
        for (std::vector<std::string>::iterator it = script->m_attackInfo.begin();
             it != script->m_attackInfo.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack info]\n", 1, 0xf, file);
    }
    if (!script->m_etcAttackInfo.empty())
    {
        fwrite("[etc attack info]\n", 1, 0x12, file);
        for (std::vector<std::string>::iterator it = script->m_etcAttackInfo.begin();
             it != script->m_etcAttackInfo.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc attack info]\n", 1, 0x13, file);
    }
    fputc(10, file);

    for (std::vector<WeaponInfo>::size_type w = 0; w < script->m_weaponWav.size(); ++w)
    {
        fwrite("[weapon wav]\n", 1, 0xd, file);
        std::vector<std::string>& wavs = script->m_weaponWav[w].m_wavs;
        // P167：ORIG 用 size()+operator[]（MISSING ixEj/size），非 begin/end 迭代器
        for (std::vector<std::string>::size_type i = 0; i < wavs.size(); ++i)
            fprintf(file, "`%s`\n", wavs[i].c_str());
        fwrite("[/weapon wav]\n", 1, 0xe, file);
    }
    fputc(10, file);

    fwrite("[weapon hit info]\n", 1, 0x12, file);
    for (std::vector<WeaponInfo>::size_type w = 0; w < script->m_weaponWav.size(); ++w)
    {
        const WeaponInfo& info = script->m_weaponWav[w];
        if (info.m_hitType == 0)
            fwrite("`[blow]` ", 1, 9, file);
        else if (info.m_hitType == 1)
            fwrite("`[cut]` ", 1, 8, file);
        else if (info.m_hitType == 2)
            fwrite("`[burn]` ", 1, 9, file);
        else if (info.m_hitType == 3)
            fwrite("`[etc]` ", 1, 8, file);
        // ORIG 0x8a110f1：`test %al,%al; je <no blood 块>` ⇒ 正极性 != 0 前置
        if (info.m_blood != 0)
            fwrite("`[blood]` ", 1, 0xa, file);
        else
            fwrite("`[no blood]` ", 1, 0xd, file);
        fprintf(file, "%d %.2f\n", info.m_field14, info.m_rate18);
    }
    fwrite("[/weapon hit info]\n", 1, 0x13, file);

    fwrite("[weapon skill info]\n", 1, 0x14, file);
    for (std::vector<WeaponInfo>::size_type w = 0; w < script->m_weaponWav.size(); ++w)
    {
        const WeaponInfo& info = script->m_weaponWav[w];
        fprintf(file, "%.2f %.2f %.2f %.2f\n",
                info.m_skillRates[0], info.m_skillRates[1],
                info.m_skillRates[2], info.m_skillRates[3]);
    }
    fwrite("[/weapon skill info]\n", 1, 0x15, file);

    fclose(file);
    convertBinaryToText(saveFileName);
    return 1;
