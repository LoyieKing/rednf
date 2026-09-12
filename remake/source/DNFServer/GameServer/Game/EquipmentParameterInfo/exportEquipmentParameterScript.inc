void exportEquipmentParameterScript(FILE* file, EquipmentParameterInfo* info)
{
    // [A18 本波] 删多余判空（P171 照抄 ORIG）：ORIG 0x89ad 头部无 file/info 判空，
    // 直接 `mov 0xc(%ebp),%eax; mov 0x4(%eax),%eax; test; je`（判 info->m_field4）。
    // avatar 能力标签表（47 项，与 import [avatar select ability] 同序）
    // [OurOnly33 符号名轴 2026-09-11] 同上：表体已合并为文件级 AVTVARIATION_TAG_NAME
    // （ORIG `093f4840 D AVTVARIATION_TAG_NAME`）。
    // 基础属性（ORIG：非 0 才输出）
    if (info->m_hpMax != 0)            fprintf(file, "[HP MAX] %+d\n", info->m_hpMax);
    if (info->m_mpMax != 0)            fprintf(file, "[MP MAX] %+d\n", info->m_mpMax);
    if (info->m_physicalAttack != 0)   fprintf(file, "[physical attack] %+d\n", info->m_physicalAttack);
    if (info->m_physicalDefense != 0)  fprintf(file, "[physical defense] %+d\n", info->m_physicalDefense);
    if (info->m_magicalAttack != 0)    fprintf(file, "[magical attack] %+d\n", info->m_magicalAttack);
    if (info->m_magicalDefense != 0)   fprintf(file, "[magical defense] %+d\n", info->m_magicalDefense);

    // 装备能力数组（经访问器）
    if (info->getEquipmentPhysicalAttack(0) != 0 || info->getEquipmentPhysicalAttack(1) != 0)
        fprintf(file, "[equipment physical attack] %+d %+d\n",
                info->getEquipmentPhysicalAttack(0), info->getEquipmentPhysicalAttack(1));
    if (info->getEquipmentPhysicalDefense(0) != 0 || info->getEquipmentPhysicalDefense(1) != 0)
        fprintf(file, "[equipment physical defense] %+d %+d\n",
                info->getEquipmentPhysicalDefense(0), info->getEquipmentPhysicalDefense(1));
    if (info->getEquipmentMagicalAttack(0) != 0 || info->getEquipmentMagicalAttack(1) != 0)
        fprintf(file, "[equipment magical attack] %+d %+d\n",
                info->getEquipmentMagicalAttack(0), info->getEquipmentMagicalAttack(1));
    if (info->getEquipmentMagicalDefense(0) != 0 || info->getEquipmentMagicalDefense(1) != 0)
        fprintf(file, "[equipment magical defense] %+d %+d\n",
                info->getEquipmentMagicalDefense(0), info->getEquipmentMagicalDefense(1));
    if (info->m_separateAttack[0] != 0 || info->m_separateAttack[1] != 0)
        fprintf(file, "[separate attack] %+d %+d\n", info->m_separateAttack[0], info->m_separateAttack[1]);

    // [separate rate]（+0x390/+0x394 float）：与 1.0 的偏差超过 epsilon 才输出
    {
        // [A18 本波] 删具名 sa/sb + 改真 fabs：ORIG @0x89ad 段为
        // `flds 0x390(%eax); fld1; fsubrp; fabs; fstpl -0x160` + `flds 0x390;
        // fabs; fmulp`（字段内存直取、无槽中转、fabs 为 x87 指令）；三元
        // `(x<0?-x:x)` 在 4.4 恒生 fchs+fucomip+条件分支（N85 伪形）。
        // [R13fixX87] x87 flags 物化极性：ORIG 0x89ad2fd/0x89ad33d 为
        // `fldl; fxch %st(1); fucompp; fnstsw %ax; test $0x5,%ah; sete; xor $1`
        // （NaN 无序 ⇒ 判真）；写成 `A > B` 时 4.4+i486 产 `fucompp; fnstsw; sahf;
        // seta`（NaN ⇒ 判假，分叉）。改为 `!(A <= B)`（等价且 NaN 归位）。
        if ((!(fabs(info->m_separateRate[0] - 1.0f) <=
               std::numeric_limits<float>::epsilon() * fabs(info->m_separateRate[0]))) ||
            (!(fabs(info->m_separateRate[1] - 1.0f) <=
               std::numeric_limits<float>::epsilon() * fabs(info->m_separateRate[1]))))
            // [R4a18 ④ 复刻 ORIG] ORIG 0x89ad385/0x89ad3a6 对两实参做 `fnstcw+fldcw $0xc,%ah
            // +fistpl`（float→int 截断）后按 **4 字节 int** 传 varargs，格式串仍 "%+f"
            // （本函数 fistp 共 4 点：0x89ad385/0x89ad3a6 = 本处两实参、0x89ad456、0x89ad514）
            // （Ghidra 的 (int)ROUND(...) 即此，ORIG 自身 UB）；原直传 double（fstpl 8B）偏离。
            fprintf(file, "[separate rate] %+f %+f\n",
                    (int)info->m_separateRate[0], (int)info->m_separateRate[1]);
    }

    if (info->m_physicalAbsoluteDamage != 0)   fprintf(file, "[physical absolute damage] %+d\n", info->m_physicalAbsoluteDamage);
    if (info->m_physicalDamageReduce != 0)     fprintf(file, "[physical damage reduce] %+d\n", info->m_physicalDamageReduce);
    // [R4a18 ④] ORIG 0x89ad456 fistpl ⇒ int 实参（同 separate rate，UB 复刻）
    if (info->m_physicalAbsoluteDefense != 0.0f) fprintf(file, "[physical absolute defense] %+.2f\n", (int)info->m_physicalAbsoluteDefense);
    if (info->m_magicalAbsoluteDamage != 0)    fprintf(file, "[magical absolute damage] %+d\n", info->m_magicalAbsoluteDamage);
    if (info->m_magicalDamageReduce != 0)      fprintf(file, "[magical damage reduce] %+d\n", info->m_magicalDamageReduce);
    // [R4a18 ④] ORIG 0x89ad514 fistpl ⇒ int 实参（同 separate rate，UB 复刻）
    if (info->m_magicalAbsoluteDefense != 0.0f) fprintf(file, "[magical absolute defense] %+.2f\n", (int)info->m_magicalAbsoluteDefense);

    // 抗性
    if (info->m_fireResistance != 0)        fprintf(file, "[fire resistance] %+d\n", info->m_fireResistance);
    if (info->m_waterResistance != 0)       fprintf(file, "[water resistance] %+d\n", info->m_waterResistance);
    if (info->m_darkResistance != 0)        fprintf(file, "[dark resistance] %+d\n", info->m_darkResistance);
    if (info->m_lightResistance != 0)       fprintf(file, "[light resistance] %+d\n", info->m_lightResistance);
    if (info->m_allElementalResistance != 0) fprintf(file, "[all elemental resistance] %+d\n", info->m_allElementalResistance);
    if (info->m_slowResistance != 0)        fprintf(file, "[slow resistance] %+d\n", info->m_slowResistance);
    if (info->m_freezeResistance != 0)      fprintf(file, "[freeze resistance] %+d\n", info->m_freezeResistance);
    if (info->m_poisonResistance != 0)      fprintf(file, "[poison resistance] %+d\n", info->m_poisonResistance);
    if (info->m_stunResistance != 0)        fprintf(file, "[stun resistance] %+d\n", info->m_stunResistance);
    if (info->m_curseResistance != 0)       fprintf(file, "[curse resistance] %+d\n", info->m_curseResistance);
    if (info->m_blindResistance != 0)       fprintf(file, "[blind resistance] %+d\n", info->m_blindResistance);
    if (info->m_lightningResistance != 0)   fprintf(file, "[lightning resistance] %+d\n", info->m_lightningResistance);
    if (info->m_stoneResistance != 0)       fprintf(file, "[stone resistance] %+d\n", info->m_stoneResistance);
    if (info->m_sleepResistance != 0)       fprintf(file, "[sleep resistance] %+d\n", info->m_sleepResistance);
    if (info->m_burnResistance != 0)        fprintf(file, "[burn resistance] %+d\n", info->m_burnResistance);
    if (info->m_weaponBreakResistance != 0) fprintf(file, "[weapon break resistance] %+d\n", info->m_weaponBreakResistance);
    if (info->m_bleedingResistance != 0)    fprintf(file, "[bleeding resistance] %+d\n", info->m_bleedingResistance);
    if (info->m_allActiveStatusResistance != 0) fprintf(file, "[all activestatus resistance] %+d\n", info->m_allActiveStatusResistance);
    if (info->m_piercingResistance != 0)    fprintf(file, "[piercing resistance] %+d\n", info->m_piercingResistance);
    if (info->m_stuckResistance != 0)       fprintf(file, "[stuck resistance] %+d\n", info->m_stuckResistance);
    if (info->m_confuseResistance != 0)     fprintf(file, "[confuse resistance] %+d\n", info->m_confuseResistance);
    if (info->m_holdResistance != 0)        fprintf(file, "[hold resistance] %+d\n", info->m_holdResistance);
    if (info->m_armorBreakResistance != 0)  fprintf(file, "[armor break resistance] %+d\n", info->m_armorBreakResistance);
    if (info->m_inventoryLimit != 0)        fprintf(file, "[inventory limit] %+d\n", info->m_inventoryLimit);
    if (info->m_hpRegenSpeed != 0)          fprintf(file, "[HP regen speed] %+d\n", info->m_hpRegenSpeed);
    if (info->m_mpRegenSpeed != 0)          fprintf(file, "[MP regen speed] %+d\n", info->m_mpRegenSpeed);
    if (info->m_mpRegenSpeedRate != 0.0f)   fprintf(file, "[MP regen speed rate] %+.2f\n", (double)info->m_mpRegenSpeedRate);
    if (info->m_moveSpeed != 0)             fprintf(file, "[move speed] %+d\n", info->m_moveSpeed);
    if (info->m_attackSpeed != 0)           fprintf(file, "[attack speed] %+d\n", info->m_attackSpeed);
    if (info->m_castSpeed != 0)             fprintf(file, "[cast speed] %+d\n", info->m_castSpeed);
    if (info->m_hitRecovery != 0)           fprintf(file, "[hit recovery] %+d\n", info->m_hitRecovery);
    if (info->m_jumpPower != 0)             fprintf(file, "[jump power] %+d\n", info->m_jumpPower);

    if (info->m_elementalProperty != 4)
        fprintf(file, "[elemental property] `%s`\n",
                getElementName(static_cast<ENUM_ELEMENT>(info->m_elementalProperty)));

    if (info->m_allElementalAttack != 0)
        fprintf(file, "[all elemental attack] %+d\n", info->m_allElementalAttack);

    // [active status]：名称段 16 条常量 fwrite（ORIG 0x89adb49..0x89ade98：
    // switch(m_activeStatusType)+逐条 fwrite(str,1,len,file)，无参数字符串不
    // 经 fprintf）＋ 值/等级共享尾（0x12 不输出名称段）。
    // [A18 本波 真语义修正] ORIG 比较常量序实证（0x089add6d..0x089ade75）：
    // 0..0xa, **0x11**, 0xb, 0xc, 0xd, 0xe —— 旧实现把 armor break 挂在 0xb 并把
    // bleeding/haste/bless/element 顺移一格（11..15）⇒ 名称/索引整体错位。
    // 且 ORIG 无具名 t 缓存（`mov 0x144(%eax),%eax; test/cmp $K,%eax` 每链直读，
    // P157/P183）⇒ 删 `int t`。
    {
        if (info->m_activeStatusType == 0)
            fwrite("[active status] `[slow]`", 1, 0x18, file);
        else if (info->m_activeStatusType == 1)
            fwrite("[active status] `[freeze]`", 1, 0x1a, file);
        else if (info->m_activeStatusType == 2)
            fwrite("[active status] `[poison]`", 1, 0x1a, file);
        else if (info->m_activeStatusType == 3)
            fwrite("[active status] `[stun]`", 1, 0x18, file);
        else if (info->m_activeStatusType == 4)
            fwrite("[active status] `[curse]`", 1, 0x19, file);
        else if (info->m_activeStatusType == 5)
            fwrite("[active status] `[blind]`", 1, 0x19, file);
        else if (info->m_activeStatusType == 6)
            fwrite("[active status] `[lightning]`", 1, 0x1d, file);
        else if (info->m_activeStatusType == 7)
            fwrite("[active status] `[stone]`", 1, 0x19, file);
        else if (info->m_activeStatusType == 8)
            fwrite("[active status] `[sleep]`", 1, 0x19, file);
        else if (info->m_activeStatusType == 9)
            fwrite("[active status] `[burn]`", 1, 0x18, file);
        else if (info->m_activeStatusType == 10)
            fwrite("[active status] `[weapon break]`", 1, 0x20, file);
        else if (info->m_activeStatusType == 0x11)
            fwrite("[active status] `[armor break]`", 1, 0x1f, file);
        else if (info->m_activeStatusType == 11)
            fwrite("[active status] `[bleeding]`", 1, 0x1c, file);
        else if (info->m_activeStatusType == 12)
            fwrite("[active status] `[haste]`", 1, 0x19, file);
        else if (info->m_activeStatusType == 13)
            fwrite("[active status] `[bless]`", 1, 0x19, file);
        else if (info->m_activeStatusType == 14)
            fwrite("[active status] `[element]`", 1, 0x1b, file);
        if (info->m_activeStatusType != 0x12)   // ORIG 0x89adea6：共享值尾（0x12 无名段亦无值段）
            fprintf(file, " %.2f %d\n",
                    (double)info->m_activeStatusValue, info->m_activeStatusLevel);
    }

    if (info->m_pushAside != 0)          fprintf(file, "[push aside] %+d\n", info->m_pushAside);
    if (info->m_liftUp != 0)             fprintf(file, "[lift up] %+d\n", info->m_liftUp);
    if (info->m_physicalCriticalHit != 0.0f) fprintf(file, "[physical critical hit] %+.2f\n", (double)info->m_physicalCriticalHit);
    if (info->m_magicalCriticalHit != 0.0f) fprintf(file, "[magical critical hit] %+.2f\n", (double)info->m_magicalCriticalHit);
    if (info->m_humanDamageRate != 0.0f) fprintf(file, "[human damage rate] %+.1f\n", (double)info->m_humanDamageRate);
    if (info->m_fortDamageRate != 0.0f)  fprintf(file, "[fort damage rate] %+.1f\n", (double)info->m_fortDamageRate);
    if (info->m_monsterDamageRate != 0.0f) fprintf(file, "[monster damage rate] %+.1f\n", (double)info->m_monsterDamageRate);
    if (info->m_stuck != 0.0f)           fprintf(file, "[stuck] %+.2f\n", (double)info->m_stuck);
    if (info->m_hpDrain)                 fwrite("[HP drain] 1\n", 1, 0xd, file);
    if (info->m_mpDrain)                 fwrite("[MP drain] 1\n", 1, 0xd, file);
    if (info->m_ignoreDefense)           fwrite("[ignore defense] 1\n", 1, 0x13, file);

    // skill levelup（job 名经 getCharacterJobName，common 输出 [common]）
    if (!info->m_levelUpSkills[0].empty())
    {
        fprintf(file, "[skill levelup]\n");
        // [b28d-solo-EquipmentParameterInfo] P28 增量侧：ORIG 0x089ae21b 增量段为
        // `ppEi`（后置、带 sret 临时槽）⇒ 源形必须写 it++。
        for (std::vector<LevelUpSkill>::iterator it = info->m_levelUpSkills[0].begin();
             it != info->m_levelUpSkills[0].end(); it++)
        {
            if ((*it).m_job == 0xb)
                fwrite("[common]", 1, 8, file);
            else
                fprintf(file, "`%s`", getCharacterJobName(
                    static_cast<ENUM_CHARACTERJOB>((*it).m_job)));
            fprintf(file, " %d %d\n", (*it).m_field4, (*it).m_field8);
        }
        fwrite("[/skill levelup]\n", 1, 0x11, file);
    }

    // avatar select ability（ORIG 0x089ae286 `call size; test eax; setne al; test al;
    // je <room list 块>` ⇒ 正极性 then=avatar / else=room list move speed rate）
    if (info->m_avatar264.size() != 0)
    {
        fprintf(file, "[avatar select ability]\n");
        if (info->m_avatar264[0].m_field0 != 0)
        {
            // ORIG 0x089ae2e8..0x089ae311：`movl $0x64,-0x30; call size;
            // mov %eax,-0x2c; call std::min<int>` ⇒ 源码是 std::min(size,100)。
            int n = std::min((int)info->m_avatar264.size(), 100);
            // ORIG 0x89ae3xx..：逐条先 snprintf 进 0x100 栈缓冲再 fprintf(file,buf)；
            // 字段读取逐次重读 operator[]（P157），job 名经临时 string c_str()。
            for (int i = 0; i < n; ++i)
            {
                char buf[0x100];
                int idx = info->m_avatar264[i].m_field0;
                if (idx == 0x2b)
                {
                    // ORIG 0x089ae343..0x089ae368：显式 `std::string job("")` 形 ——
                    // 先构造临时 allocator（C1Ev）再 C1EPKcRKSaIcE，正常路径 D1Ev 收尾。
                    std::string job("");
                    if (info->m_avatar264[i].m_skill.m_job == 0xb)
                        job = "[common]";
                    else
                        job = getCharacterJobName(
                            static_cast<ENUM_CHARACTERJOB>(info->m_avatar264[i].m_skill.m_job));
                    snprintf(buf, 0x100, "`%s` `%s` %d %d\n", AVTVARIATION_TAG_NAME[idx], job.c_str(),
                             info->m_avatar264[i].m_skill.m_field4, info->m_avatar264[i].m_skill.m_field8);
                }
                else
                {
                    const char* sign = (info->m_avatar264[i].m_field4 == 0) ? "+" : "*";
                    snprintf(buf, 0x100, "`%s` `%s` %.2f\n", AVTVARIATION_TAG_NAME[idx], sign,
                             (double)info->m_avatar264[i].m_field8);
                }
                fprintf(file, buf);
            }
        }
        fwrite("[/avatar select ability]\n", 1, 0x19, file);
    }
    else
    {
        if (info->m_roomListMoveSpeedRate != 0.0f)
            fprintf(file, "[room list move speed rate] %.2f\n",
                    (double)info->m_roomListMoveSpeedRate);
    }

    if (info->m_rigidity != 0)       fprintf(file, "[rigidity] %d\n", info->m_rigidity);
    if (info->m_fireAttack != 0)     fprintf(file, "[fire attack] %d\n", info->m_fireAttack);
    if (info->m_waterAttack != 0)    fprintf(file, "[water attack] %d\n", info->m_waterAttack);
    if (info->m_darkAttack != 0)     fprintf(file, "[dark attack] %d\n", info->m_darkAttack);
    if (info->m_lightAttack != 0)    fprintf(file, "[light attack] %d\n", info->m_lightAttack);

    // active status probability / level（18 项表，非 0 输出）
    for (int i = 0; i < 0x12; ++i)
    {
        if (info->m_activeStatusProbability[i] != 0)
            fprintf(file, "[active status probability] `%s` %d\n",
                    ACTIVE_STATUS_NO_BOX_BRACKET_TAG[i], info->m_activeStatusProbability[i]);
    }
    for (int i = 0; i < 0x12; ++i)
    {
        if (info->m_activeStatusLevelTable[i] != 0)
            fprintf(file, "[active status level] `%s` %d\n",
                    ACTIVE_STATUS_NO_BOX_BRACKET_TAG[i], info->m_activeStatusLevelTable[i]);
    }
}