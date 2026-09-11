// [b28d-20] insert_stackable_item_to_db（ORIG 0x08620968，1437/1443，nd 738）：
//   §6.2 三件套 —— p167 差集仅 EH/string 临时族（-2 `_Unwind_Resume`、
//   +2 `~basic_string`、+1 `string::C1ERKSs`），无 MISSING 语义调用；gap=6；
//   残差全为 N104（EH 尾块相对序 rotate）+ N91（数组/临时 string cleanup）
//   族 ⇒ 语义完成、byte 层 N 封顶。
// [b28d-solo-CItemList] DIFF | 本轮修：去具名 typeName，GetType 内联实参
//   script->m_strbc（ORIG 0x86209b7 的 C1ERKSs 是**按值形参**拷贝、参数槽 -0x5c）
//   ⇒ p167 309/309 **CLEAR**（原 -2 _Unwind_Resume / +2 D1Ev / +1 C1ERKSs 全消）；
//   残 N104（EH 尾块相对序）+ N82（槽/帧）⇒ 语义完成、byte 层 N 封顶。
// [b28d-solo-CItemList] DIFF | p167 CLEAR（309/309 calls）。本轮 P171 照抄：
//   ORIG 0x086209ab-0x086209c2 无 out1/out2 清零（GetType 引用出参写入）⇒
//   删我方伪造的两条 `movl $0,-0x5c/-0x60`。残 = 相邻四数组零初始化合并
//   （ORIG 0x08620a43 一次 rep stos 0x400 dwords 覆盖 4×1024B；4.4 分开清，
//   实测加 `={0}` 反致 +3 rep stos ⇒ N78 封顶，保持 3 数组初始化原形）。
// [b28d-tu-CItemList] 真结构修正 + 块序翻案：ndiff 660→57，三数不变。
//   ① 缓冲重构（真语义）：前判「四数组一次 rep stos」系误——ORIG
//     0x08620a43 单条 rep stos 0x400 dwords = **一块 4KB escBuf**（基
//     -0x1870），GetEscapeString 四调用 dst = +0/+0x400/+0x800/+0xc00
//     （0x08620be6/0c3f/0cab/2215）；原六独立 1KB 数组系 Ghidra 镜像误读
//     ⇒ 改 3 缓冲形（escBuf 4KB + ac/b4 两个 1KB，b4 缓冲声明在两 if 之间
//     复现 ORIG memset 位次）。
//   ② ep==0/ep!=0 整块换位 + 条件翻 `if (ep != 0)`：ORIG 0x08620cc7
//     `cmpl $0x0,-0x20; je <ep==0 远块>`，原负极性形致 ~600 行 difflib
//     位移 ⇒ 单此一件 625→61。
//   ③ 出参槽序：声明序改 `int out2; int out1;`（ORIG out2@-0x60/out1@
//     -0x64，4.4 先声明占浅槽）⇒ @@@21-46/@@215 五块归零。
//   ④ moduleKey 声明提前（赋值原位复现 ORIG `movl $0x13,-0x38` 位次）：
//     槽 -0x64→-0x5c，距 ORIG -0x38 尚差 8 槽位 ⇒ 4.1 浅区碎片 gap
//     （-0x28..-0x34/-0x3c..-0x54 空置）4.4 恒紧凑分配，N82/N95 封顶。
//   残 57 行 = ④ 槽差 4B 级联（pf/temp 8 块）+ escDst2/dst4 预计算两段形
//   （ORIG `lea base; lea +0x400,%ebx` callee-saved 跨 call，4.4 用点后
//   `lea+add` ⇒ N86b）+ `mov 0x10(%eax),%eax; mov %eax,%ebx` 两段中转 ×2
//   （N18a）+ 尾 fc 析构/EH landing 块序与 `return` bool xor 岛（`== true`
//   形实测被 4.4 折叠，N104）。p167 复验 309/309 CLEAR。
//   [§3 取证补记] docs/class_func_reports/sync_script__CSyncScript/
//   insert_stackable_item_to_db.md 已复核：『## 汇编』段（L80 `mov $0x400,%edx`
//   rep stos@-0x1870、L195 `lea 0x400(%eax)` dst2 段）与本轮 4KB escBuf 重构
//   一致；『## 反编译 C』段（L1484-1487 四独立 1KB 数组）系 Ghidra 聚合假象 =
//   原误转写来源，按汇编段否定。[修正版 p167 复判] 309=309，严格 mangled 差 =
//   8 对 1:1 镜像 C2↔C1/D2↔D1 clone 别名，豁免后零差（Main 裁定口径）。
bool CSyncScript::insert_stackable_item_to_db(STStackableScript* script)
{
    // 依据 docs/class_func_reports/sync_script__CSyncScript/
    // insert_stackable_item_to_db.md（ORIG 0x8620968）：ORIG 汇编 + Ghidra C
    // 逐调用转写。字段走权威 STItemScript/STStackableScript 偏移命名，map 走
    // m_module4bc（ORIG operator[]@0x8628bc6，key 为局部 ENUM_MODULE_TYPE 0x13）；
    // 流输出用权威 ::CStreamBuffer，列表用 std::for_each(..., CStringMaker)。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    // [b28d-tu-CItemList] 槽对齐（P326 时代重排）：ORIG 浅槽序 ep@-0x20、
    //   db@-0x24、moduleKey@-0x08620?? 的 -0x38、GetType 值形参串@-0x5c、
    //   out2@-0x60、out1@-0x64（`lea -0x64→0x10(%esp)`=out1、`-0x60→0x14`=out2）
    //   ⇒ moduleKey 声明紧随 db、出参声明序 out2 先于 out1；初始化仍在原位。
    ENUM_MODULE_TYPE moduleKey;
    int out2;
    int out1;
    // [b28d-solo-CItemList] P167/P183：ORIG 0x86209b7 的 `C1ERKSs` 是 GetType
    //   **按值形参**（mangled `7GetTypeESs17ENUM_CHARACTERJOBiRiS2_b`）的实参
    //   拷贝（参数槽 -0x5c，调用后 0x8620a05 D1Ev + landing 0x8620a1a D1Ev +
    //   _Unwind_Resume），**无具名 typeName 局部**——我方具名形多 1×C1ERKSs
    //   + 2×D1Ev EXTRA ⇒ 内联实参。
    if (!GetType(script->m_strbc, (ENUM_CHARACTERJOB)0, 0, out1, out2, false))
    {
        return false;
    }
    CStreamBuffer buf;
    // [b28d-tu-CItemList] 真结构修正：ORIG 0x08620a43 单次 rep stos 0x400
    //   dwords ⇒ 一块 4KB escape 大缓冲（槽基 -0x1870），三处
    //   GetEscapeString 的 dst 依次取其 +0/+0x400/+0x800 段
    //   （ORIG 0x08620be6/0x08620c3f/0x08620c9c）；两个 script2Database
    //   出参缓冲各 1KB（-0x470 memset@0x08620a59 顶部、-0x870
    //   memset@0x08620b14 在 ac 转换成功之后 ⇒ b4 缓冲声明位于两 if 之间）。
    //   原六块独立 1KB 数组（3 具名 dst + 3 未初始化）系误转写。
    char local_1870[0x1000] = {0};
    char local_470[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_strac.c_str(), local_470))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x523, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), script->m_strac.c_str(),
                      __PRETTY_FUNCTION__, 0x523);
        return false;
    }
    char local_870[0x400] = {0};
    if (!CodePage::script2Database((char*)script->m_strb4.c_str(), local_870))
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x52a, 5)("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
                      CodePage::database(), script->m_strb4.c_str(),
                      __PRETTY_FUNCTION__, 0x52a);
        return false;
    }
    buf << script->m_field0;
    buf << ", '" << GetEscapeString(db, local_470, local_1870) << "'";
    buf << ", '"
        << GetEscapeString(db, (char*)script->m_strb0.c_str(), local_1870 + 0x400)
        << "'";
    buf << ", '" << GetEscapeString(db, local_870, local_1870 + 0x800) << "'";
    buf << ", " << out1;
    buf << ", " << out2;
    buf << ", '" << this->GetUsableCharacter(script) << "'";
    buf << ", " << script->m_field4;
    buf << ", '" << this->GetAttachType(script->m_field8) << "'";
    buf << ", " << script->m_field5c;
    buf << ", " << 0;
    buf << ", " << script->m_fieldc;
    buf << ", " << script->m_field10;
    buf << ", " << script->m_field34;
    buf << ", " << script->m_field14;
    buf << ", " << script->m_field18;
    buf << ", " << script->m_field1c;
    buf << ", " << 0;
    buf << ", " << script->m_field68;
    // ORIG（+0x61b）：局部 ENUM_MODULE_TYPE = 0x13 取引用查 m_module4bc。
    //   [b28d-tu] 声明已提前（槽对齐），赋值原位。
    moduleKey = (ENUM_MODULE_TYPE)0x13;
    EquipParamInfo* ep = (EquipParamInfo*)script->m_module4bc[moduleKey];
    // [b28d-tu-CItemList] P222 正极性翻转（整块换位）：ORIG 0x08620cc7
    //   `cmpl $0x0,-0x20(%ebp); je <ep==0 远块>` ⇒ ep!=0 落体在前。
    //   原 `if (ep == 0){A}else{B}` 在 4.4 生 `jne <B>` + A 落体，
    //   与 ORIG 物理块序整体倒置（difflib 计 ~600 行位移残差）。
    //   改 `if (ep != 0){B}else{A}` 后 4.4 生 `je <A>`，块序与 ORIG 一致。
    if (ep != 0)
    {
        EquipParamInfo* e2 = ep;
        buf << ", " << e2->m_hpMax;
        buf << ", " << e2->m_mpMax;
        buf << ", " << e2->m_phyAtt;
        buf << ", " << e2->m_phyDef;
        buf << ", " << e2->m_magAtt;
        buf << ", " << e2->m_magDef;
        buf << ", " << e2->m_equipPhyAtt;
        buf << ", " << e2->m_equipPhyDef;
        buf << ", " << e2->m_equipMagAtt;
        buf << ", " << e2->m_equipMagDef;
        buf << ", " << e2->m_refFire;
        buf << ", " << e2->m_refWater;
        buf << ", " << e2->m_refDark;
        buf << ", " << e2->m_refLight;
        buf << ", " << e2->m_refAll;
        buf << ", " << e2->m_refSlow;
        buf << ", " << e2->m_refFreeze;
        buf << ", " << e2->m_refPoison;
        buf << ", " << e2->m_refStun;
        buf << ", " << e2->m_refCus;
        buf << ", " << e2->m_refBlind;
        buf << ", " << e2->m_refLite;
        buf << ", " << e2->m_refSton;
        buf << ", " << e2->m_refSleep;
        buf << ", " << e2->m_refDeekement;
        buf << ", " << e2->m_refDeadlystrike;
        buf << ", " << e2->m_refBleeding;
        buf << ", " << e2->m_refConfuse;
        buf << ", " << e2->m_refHold;
        buf << ", " << e2->m_refAllStat;
        buf << ", " << e2->m_refPierce;
        buf << ", " << e2->m_refStuck;
        buf << ", " << e2->m_invenMax;
        buf << ", " << e2->m_hpRegenRate;
        buf << ", " << e2->m_mpRegenRate;
        buf << ", " << e2->m_movSpeed;
        buf << ", " << e2->m_attSpeed;
        buf << ", " << e2->m_quest;
        buf << ", " << e2->m_hitRecovery;
        buf << ", " << e2->m_jump;
        buf << ", '"
            << this->GetAttackElement((EquipmentParameterInfo*)e2) << "'";
        buf << ", " << e2->m_attActiveStatus;
        buf << ", " << e2->m_attActiveStatusRatio;
        buf << ", " << e2->m_attActiveStatusPow;
        buf << ", " << e2->m_attBackforce;
        buf << ", " << e2->m_attUpforce;
        // ORIG（+0xe08/+0xe37/+0xebc）：movzbl → operator<<(int)。
        buf << ", " << e2->m_attHpDrain;
        buf << ", " << e2->m_attMpDrain;
        buf << ", " << e2->m_criticalHitRate;
        buf << ", " << e2->m_stuckRate;
        buf << ", " << e2->m_attDefenseIgnore;
        buf << ", " << e2->m_roomListMoveSpeedRate;
        buf << ", " << e2->m_hpMaxRate;
        buf << ", " << e2->m_mpMaxRate;
        buf << ", " << e2->m_fireAttack;
        buf << ", " << e2->m_waterAttack;
        buf << ", " << e2->m_darkAttack;
        buf << ", " << e2->m_lightAttack;
        buf << ", " << e2->m_allAttack;
        buf << ", " << e2->m_rigidity;
        buf << ", " << e2->m_magicalCriticalHit;
        buf << ", " << e2->m_physAbsDmg;
        buf << ", " << e2->m_physDmgReduce;
        buf << ", " << e2->m_physAbsDef;
        buf << ", " << e2->m_magAbsDmg;
        buf << ", " << e2->m_magDmgReduce;
        buf << ", " << e2->m_magAbsDef;
        buf << ", " << e2->m_armorBreakRes;
        buf << ", '"
            << std::for_each(e2->m_levelUpSkills.begin(),
                             e2->m_levelUpSkills.end(),
                             CStringMaker()).c_str()
            << "'";
    }
    else
    {
        // ORIG 字面量序列（Ghidra ep==0 分支逐条）：40×", 0"、", ''"
        // （att_element）、10×", 0"、3×", 0.0"、14×", 0"、", ''"
        // （skill_levelup）。
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
        buf << ", ''";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
        buf << ", 0.0";
        buf << ", 0.0";
        buf << ", 0.0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
    buf << ", 0";
        buf << ", ''";
    }
    // ORIG（+0x18ad）：两分支汇合后固定输出 set_type 空串（DAT_08ce36e4）。
    buf << ", ''";
    buf << ", '"
        << GetEscapeString(
               db,
               this->GetURL((char*)script->m_str6c.c_str(),
                            (int)script->m_str6c.length(), script->m_field70),
               // [b28d-tu-CItemList] 真结构修正：ORIG 0x08622215 第 4 处
               // GetEscapeString 的 dst = 大缓冲 +0xc00 段（lea -0x1870;
               // lea 0xc00(%eax),%edi），非独立 1KB 数组。
               local_1870 + 0xc00)
        << "'";
    buf << ", " << script->m_fieldc4;
    // [b28d-tu-CItemList2 2026-09-09] 求值序对齐：ORIG call 序列
    //   0x086222d7 c_str → 0x086222ee set_query → 0x086222f9
    //   CFieldLengthChecker::C1Ev ⇒ 原码先 set_query（含 buf.c_str() 求值）
    //   再构造 fc；原重建把 fc 声明写在 set_query 之前，调用序整体前移 2 位。
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
        "stuck_rate, att_defenseIgnore, room_list_move_speed_rate, hp_max_rate, "
        "mp_max_rate, fire_attack, water_attack, dark_attack, light_attack, "
        "all_attack, rigidity, magical_critical_hit, physical_absolute_damage, "
        "physical_damage_reduce, physical_absolute_defense, "
        "magical_absolute_damage, magical_damage_reduce, "
        "magical_absolute_defense, armor_break_resistance, skill_levelup, "
        "set_type, url, icon_mark_number) values(%s)",
        buf.c_str());
    CFieldLengthChecker fc;
    fc.AddField(db->GetQueryString());
    // [b28d-tu-CItemList] P59：ORIG 0x0862232b-0x0862233e `xor $0x1,%eax;
    //   test %al,%al; je <mov $1>` 物化岛 ⇒ 原码 `== true` 比较形（仅看 al
    //   LSB，与 4.1 char-bool 语义一致），非裸 return。
    return db->exec(true) == true;
}