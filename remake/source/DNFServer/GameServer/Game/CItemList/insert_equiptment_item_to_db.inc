// [b28d-20] insert_equiptment_item_to_db（ORIG 0x0861d87e，2891/2881，nd 54）：
//   §6.2 三件套 —— p167 调用点差集 488/488 **CLEAR**（0 missing/0 extra）、
//   gap=10（全为寄存器角色互换 esi↔ebx 与一处 jmp 岛）、槽位集一致 ⇒ 语义完成、
//   byte 层 N7/N82 封顶。
// [b28d-solo-CItemList] DIFF（ndiff 95→89）| p167 CLEAR（488/488 calls）。
//   本轮 T3 直证修 ani 循环守卫：ORIG `movzbl 0x38(%edx,%eax,1),%eax;
//   test %al,%al; je`（无 xor/setcc）⇒ m_usable 为 bool 且源码正极性包裹
//   `if (m_usable[i] != 0) { body }`（`==0 continue` 被 4.4 折成
//   `xor $1; test; jne`，N122）。残 = 数组基址/两段中转寄存器角色
//   （ORIG `mov mem,%eax; mov %eax,%ebx` vs 4.4 单段）⇒ N18a/N86b/N107。
bool CSyncScript::insert_equiptment_item_to_db(STEquipmentScript* script,
                                              unsigned int idx, int mode,
                                              int key)
{
    // 依据 docs/class_func_reports/sync_script__CSyncScript/
    // insert_equiptment_item_to_db.md（ORIG 0x861d87e）反编译 C 整函数转写：
    // 全部字段以权威 STEquipmentScript/STItemScript/EquipmentParameterInfo
    // 偏移直接访问，删除 EquipScriptFull 镜像布局（其声明序与权威偏移不符）。
    // switch 块序 = 源码 case 序（ORIG 物理 case0@+0xaa → case1@+0x26d →
    // case2@+0x27c，4.1 直落 case0 与跳板一致）。
    // 残差豁免（O=2898/N=2887，canon 级全部为跨版本形态）：
    //   [N18a] &buf[i] 元素址：ORIG mov/lea/shl/add/mov 5 段（结果先 eax
    //     后拷 callee-saved），4.4 恒 4 段直出 ebx（×6）；
    //   [N18a] ", '" 实参两跳：ORIG mov mem,%eax; mov %eax,%ebx，4.4 折
    //     mov mem,%ebx（×2）；
    //   [P23] GetEscapeString arg3 预计算位：ORIG +0x1001 提前 lea 入 ebx
    //     跨调用保持，4.4 挪至 push 位（等长窗移）。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // ORIG：先声明占浅槽 → local_154@-0x150（arg5&/比较/后输出），
    // local_158@-0x154（arg4&/先输出）。
    int local_154 = 0;
    int local_158 = 0;
    int foundAni;
    for (int i = 0; i < 0xb; ++i)
    {
        if (script->m_usable[i] != 0)
        {
            foundAni = i;
            break;
        }
    }
    int local_74 = 0;
    switch (mode)
    {
    // ORIG +0xaa case0 直落、case1@+0x26d、case2@+0x27c：物理块序 0,1,2
    case 0:
    {
        if (!GetType(script->m_strbc, (ENUM_CHARACTERJOB)foundAni,
                     script->m_1b8, local_158, local_154, true))
        {
            return false;
        }
        idx = (unsigned int)script->m_field0;
        // ORIG：map<int,EquipmentParameterInfo>（+0x678）逐档递归；
        // 递归实参为整对象 STEquipmentScript temp（C1 构造，m_param1c0 = it->second）。
        for (std::map<int, EquipmentParameterInfo>::iterator it =
                 script->m_paramMap678.begin();
             it != script->m_paramMap678.end(); ++it)
        {
            EquipmentParameterInfo& par = it->second;
            STEquipmentScript temp;
            temp.m_param1c0 = par;
            insert_equiptment_item_to_db(&temp, idx, 1, it->first);
        }
        break;
    }
    case 1:
        local_74 = idx;
        idx = (unsigned int)-1;
        break;
    case 2:
        local_74 = idx;
        idx = (unsigned int)-2;
        break;
    }
    // ---- socket（local_162[5..9]）：0x6cc ushort 段 + 0x6c0 avatar 段 ----
    char socketChars[5] = {0};
    int socketCount = 0;
    if (script->m_vec6cc.size() != 0)
    {
        std::vector<unsigned short>::iterator vit = script->m_vec6cc.begin();
        while (vit != script->m_vec6cc.end())
        {
            if (socketCount > 4)
            {
                break;
            }
            unsigned short usVal = *vit;
            socketChars[socketCount] = GetSocketTypeCharacter(usVal);
            ++socketCount;
            vit++;
        }
    }
    if (script->m_avatar6c0.size() != 0)
    {
        std::vector<stAvatarTypeSelect_t>::iterator ait =
                 script->m_avatar6c0.begin();
        while (ait != script->m_avatar6c0.end())
        {
            // ORIG：*it 整对象拷贝到局部（8 dword mov），再从副本读字段。
            stAvatarTypeSelect_t av = *ait;
            if (av.m_field10 > 0)
            {
                int k = 0;
                while (k < av.m_field10)
                {
                    if (socketCount > 4)
                    {
                        break;
                    }
                    socketChars[socketCount] =
                        GetSocketTypeCharacter(av.m_socketType[k]);
                    ++socketCount;
                    ++k;
                }
            }
            ait++;
        }
    }
    // ---- hide_equipment（local_1fc）：0x768 ENUM_EQUIPMENTTYPE 段 ----
    char local_1fc[64] = {0};
    if (script->m_equipTypes768.size() != 0)
    {
        int equipCount = 0;
        std::vector<ENUM_EQUIPMENTTYPE>::const_iterator eit =
                 script->m_equipTypes768.begin();
        while (eit != script->m_equipTypes768.end())
        {
            // ORIG：先 *it 存局部（local_50），两分支共用一份实参。
            int hideType = *eit;
            char tmp[8] = {0};
            if (equipCount == 0)
            {
                sprintf(tmp, "%d", hideType);
            }
            else
            {
                sprintf(tmp, "*%d", hideType);
            }
            strcat(local_1fc, tmp);
            eit++;
            ++equipCount;
        }
    }
    // ---- ani（local_2fc/3fc/4fc/5fc）----
    // ORIG：遍历 0..0xa 中 m_usable[i] 非 0 的职业，getAniInfo 为空即 break；
    // 四帧号任一非 -1 才处理；expand!=0 → (m_30,m_34)+m_ani18 → 3fc/5fc；
    // expand==0 → (m_c,m_10)+m_ani0 → 2fc/4fc（ORIG if 体为 expand，else 为普通层）。
    char local_2fc[256] = {0};
    char local_3fc[256] = {0};
    char local_4fc[256] = {0};
    char local_5fc[256] = {0};
    bool local_69 = 0;
    for (int i = 0; i < 0xb; ++i)
    {
        // [b28d-solo] T3 直证（ORIG 0x0862xxxx `movzbl 0x38(%edx,%eax,1),%eax;
        //   test %al,%al; je`，无 xor/setcc）⇒ m_usable 为 bool 且源码是
        //   正极性包裹 `if (m_usable[i] != 0) { body }`（`== 0 continue` 在 4.4
        //   产 `xor $1; test; jne`，N122）。
        if (script->m_usable[i] != 0)
        {
        EquipmentAniInfoScript* aniInfo =
            script->getAniInfo((ENUM_CHARACTERJOB)i);
        if (aniInfo == 0)
        {
            break;
        }
        if (aniInfo->m_30 == -1 && aniInfo->m_34 == -1 &&
            aniInfo->m_c == -1 && aniInfo->m_10 == -1)
        {
            continue;
        }
        if (aniInfo->m_expand14 != 0)
        {
            // expand 层：帧 (m_30, m_34)，序列 m_ani18
            if (aniInfo->m_30 == -1 && aniInfo->m_34 == -1)
            {
                continue;
            }
            char tmp[0x100] = {0};
            sprintf(tmp, "%02d%02d", aniInfo->m_30, aniInfo->m_34);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", i);
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", (int)aniInfo->m_ani18.size());
            strcat(tmp, t3);
            std::vector<STEquipmentAniScript>::const_iterator ait =
                     aniInfo->m_ani18.begin();
            while (ait != aniInfo->m_ani18.end())
            {
                // ORIG：每轮先清 src/wb 两缓冲（deref 之前），去 '_' 为跨缓冲拷贝、
                // 不写终止符（预零即终止）。
                char src[0x100] = {0};
                char wb[0x100] = {0};
                const STEquipmentAniScript* entry = &*ait;
                strcat(tmp, "*");
                strcpy(src, entry->m_str60.c_str());
                int w = 0;
                unsigned int r = 0;
                while (r < strlen(src))
                {
                    if (src[r] != '_')
                    {
                        wb[w] = src[r];
                        ++w;
                    }
                    ++r;
                }
                strcat(tmp, wb);
                ait++;
            }
            if (!local_69)
            {
                strcpy(local_3fc, tmp);
                local_69 = 1;
            }
            else
            {
                strcpy(local_5fc, tmp);
            }
        }
        else
        {
            // 普通层：帧 (m_c, m_10)，序列 m_ani0
            if (aniInfo->m_c == -1 && aniInfo->m_10 == -1)
            {
                continue;
            }
            char tmp[0x100] = {0};
            sprintf(tmp, "%02d%02d", aniInfo->m_c, aniInfo->m_10);
            if (local_69 != 0)
            {
                char t2[8] = {0};
                sprintf(t2, "*%d", i);
                strcat(tmp, t2);
            }
            char t3[8] = {0};
            sprintf(t3, "*%d", (int)aniInfo->m_ani0.size());
            strcat(tmp, t3);
            std::vector<STEquipmentAniScript>::const_iterator ait =
                     aniInfo->m_ani0.begin();
            while (ait != aniInfo->m_ani0.end())
            {
                char src[0x100] = {0};
                char wb[0x100] = {0};
                const STEquipmentAniScript* entry = &*ait;
                strcat(tmp, "*");
                strcpy(src, entry->m_str60.c_str());
                int w = 0;
                unsigned int r = 0;
                while (r < strlen(src))
                {
                    if (src[r] != '_')
                    {
                        wb[w] = src[r];
                        ++w;
                    }
                    ++r;
                }
                strcat(tmp, wb);
                ait++;
            }
            if (!local_69)
            {
                strcpy(local_2fc, tmp);
                local_69 = 1;
            }
            else
            {
                strcpy(local_4fc, tmp);
            }
        }
        }
    }
    // ---- hide_growtype_avatar（growtype[0..4]）：script+0x780 数值段 ----
    char growtype[5] = {0};
    // ORIG（+0xc80）：growtype 源是 script+0x780 的 vector<int>（非 +0x198
    // extra_icon 列，后者只进 for_each 输出）。
    if (!script->m_vec780.empty())
    {
        int cnt = 0;
        std::vector<int>::const_iterator iit = script->m_vec780.begin();
        while (iit != script->m_vec780.end())
        {
            if (cnt > 4)
            {
            break;
            }
            int v = *iit;
            growtype[cnt] = v + '0';
            ++cnt;
            iit++;
        }
    }
    char local_3e90[0x2c00] = {0};
    unsigned int local_68 = 4;
    // local_168：0xb4 空则取 0x6a4
    std::string local_168;
    // ORIG（+0xdac）：string::length() != 0 判非空（setne 物化），先 b4 后 6a4。
    if (script->m_strb4.length() != 0)
    {
        local_168 = script->m_strb4;
    }
    else
    {
        local_168 = script->m_str6a4;
    }
    char local_9fc[1024] = {0};
    if (!CodePage::script2Database((char*)script->m_strac.c_str(), local_9fc))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x398, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), script->m_strac.c_str(),
                      __PRETTY_FUNCTION__, 0x398);
        return false;
    }
    char local_1290[1024] = {0};
    if (!CodePage::script2Database((char*)local_168.c_str(), local_1290))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x39f, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      // ORIG（+0xf09）：trace 实参取 script+0xb4 的 c_str()。
                      CodePage::database(), script->m_strb4.c_str(),
                      __PRETTY_FUNCTION__, 0x39f);
        return false;
    }
    CStreamBuffer buf;
    buf << idx;
    buf << ", '" << GetEscapeString(db, local_9fc, local_3e90) << "'";
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_strb0.c_str(), &local_3e90[0x400])
        << "'";
    buf << ", '" << GetEscapeString(db, local_1290, local_3e90 + 0x800) << "'";
    buf << ", " << local_158;
    buf << ", " << local_154;
    buf << ", '" << this->GetUsableCharacter(script) << "'";
    buf << ", " << script->m_field4;
    buf << ", '" << this->GetAttachType(script->m_field8) << "'";
    buf << ", " << script->m_field5c;
    buf << ", " << script->m_184;
    buf << ", " << script->m_fieldc;
    buf << ", " << script->m_field10;
    buf << ", " << script->m_field34;
    buf << ", " << script->m_field14;
    buf << ", " << script->m_field18;
    buf << ", " << script->m_field1c;
    buf << ", " << script->m_1bc;
    buf << ", " << script->m_field68;
    buf << ", " << script->m_param1c0.m_hpMax;
    buf << ", " << script->m_param1c0.m_mpMax;
    buf << ", " << script->m_param1c0.m_physicalAttack;
    buf << ", " << script->m_param1c0.m_physicalDefense;
    buf << ", " << script->m_param1c0.m_magicalAttack;
    buf << ", " << script->m_param1c0.m_magicalDefense;
    buf << ", " << script->m_param1c0.m_phyAtt[0];
    buf << ", " << script->m_param1c0.m_phyDef[0];
    buf << ", " << script->m_param1c0.m_magAtt[0];
    buf << ", " << script->m_param1c0.m_magDef[0];
    buf << ", " << script->m_param1c0.m_fireResistance;
    buf << ", " << script->m_param1c0.m_waterResistance;
    buf << ", " << script->m_param1c0.m_darkResistance;
    buf << ", " << script->m_param1c0.m_lightResistance;
    buf << ", " << script->m_param1c0.m_allElementalResistance;
    buf << ", " << script->m_param1c0.m_slowResistance;
    buf << ", " << script->m_param1c0.m_freezeResistance;
    buf << ", " << script->m_param1c0.m_poisonResistance;
    buf << ", " << script->m_param1c0.m_stunResistance;
    buf << ", " << script->m_param1c0.m_curseResistance;
    buf << ", " << script->m_param1c0.m_blindResistance;
    buf << ", " << script->m_param1c0.m_lightningResistance;
    buf << ", " << script->m_param1c0.m_stoneResistance;
    buf << ", " << script->m_param1c0.m_sleepResistance;
    buf << ", " << script->m_param1c0.m_burnResistance;
    buf << ", " << script->m_param1c0.m_weaponBreakResistance;
    buf << ", " << script->m_param1c0.m_bleedingResistance;
    buf << ", " << script->m_param1c0.m_confuseResistance;
    buf << ", " << script->m_param1c0.m_holdResistance;
    buf << ", " << script->m_param1c0.m_allActiveStatusResistance;
    buf << ", " << script->m_param1c0.m_piercingResistance;
    buf << ", " << script->m_param1c0.m_stuckResistance;
    buf << ", " << script->m_param1c0.m_inventoryLimit;
    buf << ", " << script->m_param1c0.m_hpRegenSpeed;
    buf << ", " << script->m_param1c0.m_mpRegenSpeed;
    buf << ", " << script->m_param1c0.m_moveSpeed;
    buf << ", " << script->m_param1c0.m_attackSpeed;
    buf << ", " << script->m_param1c0.m_castSpeed;
    buf << ", " << script->m_param1c0.m_hitRecovery;
    buf << ", " << script->m_param1c0.m_jumpPower;
    buf << ", '" << this->GetAttackElement(&script->m_param1c0) << "'";
    buf << ", " << script->m_param1c0.m_activeStatusType;
    buf << ", " << script->m_param1c0.m_activeStatusValue;
    buf << ", " << script->m_param1c0.m_activeStatusLevel;
    buf << ", " << script->m_param1c0.m_pushAside;
    buf << ", " << script->m_param1c0.m_liftUp;
    // ORIG（+0x1b37/+0x1b66）：movzbl 后走 operator<<(int)（byte→int 提升），
    // 不是 uint 重载。
    buf << ", " << script->m_param1c0.m_hpDrain;
    buf << ", " << script->m_param1c0.m_mpDrain;
    buf << ", " << script->m_param1c0.m_physicalCriticalHit;
    buf << ", " << script->m_param1c0.m_stuck;
    buf << ", " << script->m_param1c0.m_ignoreDefense;
    buf << ", '"
        << std::for_each(script->m_param1c0.m_levelUpSkills[0].begin(),
                         script->m_param1c0.m_levelUpSkills[0].end(),
                         CStringMaker()).c_str()
        << "'";
    // ORIG（+0x1d83）：三目直接物化到寄存器（mov $ptr,%eax），不占命名槽。
    buf << ", '"
        << ((script->m_str668.compare("") == 0 && script->m_vec66c.size() == 0 &&
             script->m_6a0 == -1 && script->m_798 == -1)
                ? "n"
                : "y")
        << "'";
    buf << ", '" << socketChars << "'";
    register char* escDst1 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_param1c0.m_str27c.c_str(), escDst1)
        << "'";
    register char* escDst2 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_strb8.c_str(), escDst2)
        << "'";
    buf << ", " << script->m_param1c0.m_antiEvil;
    buf << ", " << script->m_field24;
    buf << ", " << script->m_184;
    buf << ", '" << script->m_pair28.first << "," << script->m_pair28.second << "'";
    buf << ", " << script->m_param1c0.m_physicalAbsoluteDamage;
    buf << ", " << script->m_param1c0.m_physicalDamageReduce;
    buf << ", " << script->m_param1c0.m_physicalAbsoluteDefense;
    buf << ", " << script->m_param1c0.m_magicalAbsoluteDamage;
    buf << ", " << script->m_param1c0.m_magicalDamageReduce;
    buf << ", " << script->m_param1c0.m_magicalAbsoluteDefense;
    buf << ", " << script->m_param1c0.m_fireAttack;
    buf << ", " << script->m_param1c0.m_waterAttack;
    buf << ", " << script->m_param1c0.m_darkAttack;
    buf << ", " << script->m_param1c0.m_lightAttack;
    buf << ", " << script->m_param1c0.m_allElementalAttack;
    buf << ", " << script->m_param1c0.m_weaponBreakResistance;
    buf << ", " << script->m_param1c0.m_armorBreakResistance;
    buf << ", " << script->m_param1c0.m_allActiveStatusResistance;
    buf << ", " << script->m_param1c0.m_rigidity;
    buf << ", '"
        << std::for_each(script->m_param1c0.m_aura188.begin(),
                         script->m_param1c0.m_aura188.end(),
                         CStringMaker()).c_str()
        << "'";
    buf << ", " << script->m_param1c0.m_magicalCriticalHit;
    register char* escDst3 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_str668.c_str(), escDst3)
        << "'";
    buf << ", '"
        << std::for_each(script->m_vec66c.begin(), script->m_vec66c.end(),
                         CStringMaker()).c_str()
        << "'";
    register char* escDst4 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_str6b0.c_str(), escDst4)
        << "'";
    register char* escDst5 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_str6b4.c_str(), escDst5)
        << "'";
    register char* escDst6 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_param1c0.m_str278.c_str(), escDst6)
        << "'";
    register char* escDst7 = local_3e90 + local_68 * 0x400;
    ++local_68;
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_param1c0.m_str27c.c_str(), escDst7)
        << "'";
    buf << ", " << script->m_798;
    buf << ", '" << local_1fc << "'";
    buf << ", '"
        << std::for_each(script->m_param1c0.m_skill1b8.begin(),
                         script->m_param1c0.m_skill1b8.end(),
                         CStringMaker()).c_str()
        << "'";
    buf << ", " << local_74;
    buf << ", " << key;
    buf << ", " << script->m_6a0;
    buf << ", '" << local_2fc << "'";
    buf << ", '" << local_3fc << "'";
    buf << ", '" << local_4fc << "'";
    buf << ", '" << local_5fc << "'";
    buf << ", '" << growtype << "'";
    buf << ", " << script->m_param1c0.m_roomListMoveSpeedRate;
    // ORIG（+0x29c6）：dst = 公共转义缓冲 +0xc00 先物化进 callee-saved %edi
    // 跨 GetURL 调用存活 ⇒ register 指针。
    register char* urlDst = &local_3e90[0xc00];
    buf << ", '"
        << GetEscapeString(
               db, this->GetURL((char*)script->m_str6c.c_str(),
                                (int)script->m_str6c.length(),
                                script->m_field70),
               urlDst)
        << "'";
    buf << ", " << script->m_fieldc4;
    buf << ", '"
        << std::for_each(script->m_vec198.begin(),
                         script->m_vec198.end(),
                         CStringMaker()).c_str()
        << "'";
    buf << ", " << script->m_param1c0.m_hpMaxRate;
    buf << ", " << script->m_param1c0.m_mpMaxRate;
    db->set_query(
        "inSert into dnf_item_info(it_no, it_name, it_eng_name, it_explain, "
        "master_type, sub_type, job, class, revert, level, skill, "
        "create_ratio, rarity, weight, price, cash, medal, durability, "
        "cooltime, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, "
        "equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, "
        "ref_fire, ref_water, ref_dark, ref_light, ref_all, ref_slow, "
        "ref_freeze, ref_poison, ref_stun, ref_cus, ref_blind, ref_lite, "
        "ref_ston, ref_sleep, ref_deekement, ref_deadlystrike, ref_bleeding, "
        "ref_confuse, ref_hold, ref_all_stat, ref_pierce, ref_stuck, "
        "inven_max, hp_regenrate, mp_regenrate, mov_speed, att_speed, quest, "
        "hit_recovery, jump, att_element, att_active_status, "
        "att_active_status_ratio, att_active_status_pow, att_backforce, "
        "att_upforce, att_hp_drain, att_mp_drain, criticalhit_rate, "
        "stuck_rate, att_defenseIgnore, skill_levelup, set_type, jewel_type, "
        "detail_explain, flavor_text, anti_evil, value, required_skill, "
        "need_material, physical_absolute_damage, physical_damage_reduce, "
        "physical_absolute_defense, magical_absolute_damage, "
        "magical_damage_reduce, magical_absolute_defense, fire_attack, "
        "water_attack, dark_attack, light_attack, all_attack, "
        "weapon_break_resistance, armor_break_resistance, "
        "all_activestatus_resistance, rigidity, item_aura, "
        "magical_critical_hit, set_name, set_item, fullset_basic_explain, "
        "fullset_detail_explain, parameter_basic_explain, "
        "parameter_detail_explain, part_set_index, hide_equipment, "
        "skill_data_up, it_set_no, db_piece_count, set_item_master, "
        "ani_variation, ani_variation_expand, ani_variation2, "
        "ani_variation_expand2, hide_growtype_avatar, "
        "room_list_move_speed_rate, url, icon_mark_number, "
        "extra_icon_idx_list, hp_max_rate, mp_max_rate) values(%s)",
        buf.c_str());
    CFieldLengthChecker fc;
    fc.AddField(db->GetQueryString());
    if (!db->exec(true))
    {
        return false;
    }
    if (!script->m_param1c0.m_avatar264.empty())
    {
        int abilityIdx = 0;
        for (std::vector< ::STAvatarAbilityVariation>::iterator avit =
                 script->m_param1c0.m_avatar264.begin();
             avit != script->m_param1c0.m_avatar264.end(); ++avit)
        {
            // ORIG（+0x2ca4..+0x2d77）：*it 指针入槽后逐字段重读（-0x1c reload），
            // 全部实参先行求值（skill+8→深槽、skill+4→edi、job 三目→edx、
            // f8 flds、f4→esi、f0→ebx、idx++→ecx），再压参调 set_query。
            ::STAvatarAbilityVariation* pAv = &*avit;
            db->set_query(
                "inSert into avatar_select_ability(it_no, ability_no, "
                "ability_type, rate_change, value, job, skill_index, "
                "skill_level) values(%d,%d,%d,%d,%f,%d,%d,%d)",
                script->m_field0, abilityIdx++, pAv->m_field0, pAv->m_field4,
                pAv->m_field8,
                (pAv->m_skill.m_job == 0xb) ? 0x63 : pAv->m_skill.m_job,
                pAv->m_skill.m_field4, pAv->m_skill.m_field8);
            if (!db->exec(true))
            {
                return false;
            }
        }
    }
    // ORIG（+0x2db5..+0x2f07）：flags 全部预初始化（4×movb+edi）+ 末端统一
    // dtor 链 ⇔ || 短路链内联临时（allocator ctor + string(char*,alloc)+D1），
    // 比较 SITEMTYPE.m_field4(+0x4 ushort) 与 local_154（mode0 GetType sub_type
    // 输出；其余 mode 为 0）。
    // ORIG（+0x2e25）：movzwl 0x4(ushort) → 32 位扩展后与 uint local_154 比较
    // （无 (unsigned short) 截断路径）。
    if (m_itemTypeMap[std::string("artifact red")].m_field4 == local_154 ||
        m_itemTypeMap[std::string("artifact blue")].m_field4 == local_154 ||
        m_itemTypeMap[std::string("artifact green")].m_field4 == local_154)
    {
        this->insert_artifact_info_to_db(script);
    }
    return true;
}