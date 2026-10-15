// ============================================================================
// DB_LoadInventory::_GetUserAvatar @0x0840be8a（static bool）
// §6.2 收口注记（b28d 复攻，gap 76→1）：语义/列序/行号与 ORIG 全对齐。
// 本轮修复（逐段对照 ORIG 汇编 + LCS 段级归一化多重集）：
//   ①【真语义 bug】slot<10 与 slot>=10 的目标数组写反：ORIG 0840c971 的
//     slot<10 分支基址是 `add 0x14(%ebp),%eax`（第 4 形参 = 调用点 sig+0xa3a1），
//     0840c895 的 idx 分支是 `add 0x10(%ebp),%eax`（第 3 形参 = sig+0x4044）；
//     旧码 slot<10 写 inven（第 3 参）、slot>=10 写 mirror（第 4 参）——反了。
//   ② P183 去缓存：删 `AvatarDetailInfo& e = out[row]` 与 `Inven_Item* item`，
//     每写点重发 `mov row,%eax; imul $0x4b; add 0xc(%ebp),%eax`（14 写点 ×3 条）。
//   ③ 列索引具名局部 `int col = 7;` + `get_*(col++)`（8 处 ×2 条）+ col10/col11
//     错误分支 P227 全表达式临时链用 `col - 1`。
//   ④ P312：ipgAgencyNo 声明 0x19 字节 ⇒ 落非 4 对齐槽 -0x5d，4.4 发与 ORIG
//     同形的「对齐剥离 + rep stosl」慢路（0840bf59..0840bfbd）。
//   ⑤ 声明序照 ORIG 清零序 uiId→clearAvatarId→count→abilityNo；slot/itId 去 `=0`。
//   ⑥ color1/color2 写点前移到 get_byte(14) 之前（ORIG 0840c6fe/712 < 0840c726）。
//   ⑦ P223 三失败出口 goto 共享尾标签 ⇒ 复现 0840ca41/44/47 三个 `nop;jmp` 岛。
//   ⑧ trade_restrict 位：常量位域赋值 `m_bits.m_flag = 1/0` 复现 ORIG 单次基址
//     `movzbl 0x33(%eax); or $0x20/and $0xdf; mov %dl,0x33(%eax)`（m_field 复合
//     赋值 4.4 恒两次基址 +3 条/处；探针 /tmp/p_sep.cpp f1 vs f4）。
// 剩余真缺口 = 0；唯一净差 1 条 = count 赋值 `mov %eax,%edx; mov 0x18(%ebp),%eax;
//   mov %edx,(%eax)`（4.1 RHS-first+中转 vs 4.4 LHS-first 直存）⇒ N76/P211 封顶：
//   异型化旋钮在共享头 MySQL::get_n_rows 返回类型（改它牵动全仓符号性比较），
//   按 S2 不在本泳道改；unsigned 视图/cast/字节视图三形实测均劣
//   （698/710/712 vs 695，且视图形占用 %ebx/%edx 破坏 callee-saved 布局）。
// 残差余项 = 槽号分区（db -0x1c vs -0x18、trace -0x7c vs -0x12c）⇒ N82/N95 豁免。
// p167 调用点差集 = C1/C2 同址别名伪差（59/59，P93 豁免）。
// user_items 15 列（limit 0x73）。AvatarDetailInfo 条目步长 0x4b：
//   +0 ui_id / +4 expire / +8 reg / +0xc ipgAgencyNo(0x18) / +0x25 clearAvatarId
//   / +0x29 jewelSocket(0x1e) / +0x47 color1 / +0x49 color2。
// slot<10 → 第 4 形参（mirror 名，调用点 sig+0xa3a1）；10<=slot-10<0x69 →
// 第 3 形参（inven 名，调用点 sig+0x4044）。Inven_Item 写：m_field1=8、
// m_endurance=ability_no、m_addInfo=it_id、m_addInfo2=ui_id、m_amp lock、
// m_hiddenOption=hidden、m_avatarEndur=endurance、+0x33 trade_restrict 位 0x20。
// 错误行号 0x26dc..0x277d（col12 失败仅记日志）。
// b28d-solo-DB_LoadInventory：17 处 trace 改 __PRETTY_FUNCTION__（ORIG 有
// _ZZ...E19__PRETTY_FUNCTION__ 符号）+ 前 6 处 db 检查改共用 bool ok（ORIG
// `mov %al,-0x19`）。
// [L3CUserPremDBInven 本波复核（ORIG 0x0840bd3a）]
//   本波复算 699(切片 698+注释)/698：**全函数唯一缺口 = 上述 1 条 N76**（ORIG
//   0x0840bfdc `call get_n_rows; mov %eax,%edx; mov 0x18(%ebp),%eax; mov %edx,(%eax)`
//   vs 我方 `call; mov 0x18(%ebp),%edx; mov %eax,(%edx)`，opcode 序列 100% 等长
//   仅此 1 条中转 ⇒ 距 IDENTICAL 仅 1 条）⇒ 维持 N76（跨版本 RHS-first 固有差，
//   异型化旋钮在共享头 MySQL.h，非本泳道写域）。
// ============================================================================
bool DB_LoadInventory::_GetUserAvatar(unsigned int characNo,
                                      AvatarDetailInfo* out,
                                      Inven_Item* inven, Inven_Item* mirror,
                                      int& count)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slot,it_id,unix_timestamp(expire_date),ability_no,"
                  "ipg_agency_no,unix_timestamp(reg_date),ui_id,"
                  "clear_avatar_id,jewel_socket,item_lock_key, hidden_option,"
                  " emblem_endurance, color1, color2, trade_restrict"
                  " from user_items where charac_no=%u and expire_date>now()"
                  " and stat=0 limit %d", characNo, 0x73);
    // ORIG：exec/fetch/get_int(0)/get_int(1)/get_uint(2)/get_int(3) 六处共用
    // 函数级 bool 槽 -0x19（`mov %al,-0x19; movzbl`），其余 get_* 内联 xor 判。
    bool ok = db->exec(true);
    if (!ok)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x26dc, 5);
        t("DB_LoadInventory::GetUserAvatar, exec() ERROR charac_no=%u", characNo);
        return 0;
    }

    unsigned int expireDate = 0;
    int regDate = 0;
    // b28d-214③/N78：ORIG rep stos 内联清零（无 memset call）⇒ 数组零初始化器。
    // P312：ORIG 数组落在 -0x155（非 4 对齐）且 rep stos 展开的 len 立即数为
    // 0x19（0840bf5f `mov $0x19,%edx`，dword 计数前的字节数）⇒ 原码数组 25 字节。
    // 声明 0x19 字节使 4.4 把起始槽顶到非 4 对齐（-0x5d），复现「对齐剥离 +
    // rep stosl」慢路（0840bf59..0840bfbd 共 34 条）；get_str 仍只写 0x18。
    char ipgAgencyNo[0x19] = {0};
    // ORIG 清零/赋值序（0840bfbd..0840bfe3）：uiId(-0x15c) → clearAvatarId
    // (-0x160) → get_n_rows/count → abilityNo(-0x164) ⇒ 声明序照抄。
    int uiId = 0;
    int clearAvatarId = 0;
    // ORIG 0840bfdc 为 `mov %eax,%edx; mov 0x18(%ebp),%eax; mov %edx,(%eax)`
    // （RHS-first + 中转）；我方 LHS-first。S2 取证（2026-09-09）判 N76：
    // ① nm 两侧 get_n_rows 同 size 0xb 同体、返回类型不入 mangled；
    // ② 231=231 个调用点，比较指令逐点全等（ORIG 0843da14 `cmp $0x32,%eax; ja`
    //    ＝ 我方 082eaa3a；0842b04d `jbe` ＝ 082ea4ce；0862dea5 `seta` ＝ 0879419f；
    //    0840ca52 `setg` ＝ 08294e85）⇒ 返回类型两侧相同；
    // ③ store 装载序两侧各自双向（ORIG 08429c25 LHS-first 反例；我方 08322e69
    //    RHS-first 反例）⇒ 非类型驱动，属 N76 跨版本固有差，禁改 MySQL.h。
    count = db->get_n_rows();
    int abilityNo = 0;
    int row = 0;
    while (row < db->get_n_rows())
    {
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x26f3, 5);
            t("DB_LoadInventory::GetUserAvatar, fetch() ERROR charac_no=%u,%d",
              characNo, row);
            break;
        }
        // ORIG 0840c061：get_int 目标槽直接取址（`lea -0x130(%ebp),%eax`），
        // 无预清零 ⇒ 不得写 `int slot = 0;`（多一条 movl $0x0）。
        int slot;
        ok = db->get_int(0, slot);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x26fa, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(0) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        int itId;
        ok = db->get_int(1, itId);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2701, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(1) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        ok = db->get_uint(2, expireDate);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2708, 5);
            t("DB_LoadInventory::GetUserAvatar, get_uint(2) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        ok = db->get_int(3, abilityNo);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x270f, 5);  // ORIG 0840c1fc
            t("DB_LoadInventory::GetUserAvatar, get_int(3) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        if (!db->get_str(4, ipgAgencyNo, 0x18))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2727, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(4) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        if (!db->get_int(5, regDate))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x272e, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(5) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        if (!db->get_int(6, uiId))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2734, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(6) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        // ORIG 0840c39c：列索引具名局部 `col` 从 7 起后自增（每处
        // `mov -0x10(%ebp),%eax; addl $0x1,-0x10(%ebp)`），错误分支日志用 col-1。
        int col = 7;
        if (!db->get_int(col++, clearAvatarId))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x273b, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(6) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        stAvatarEmblemInfo_t jewel;
        jewel.init();
        if (!db->get_binary(col++, &jewel, 0x1e))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2742, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(6) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        unsigned char lockKey = 0;
        if (!db->get_ubyte(col++, lockKey))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2749, 5);
            t("DB_LoadInventory::GetUserAvatar, get_ubyte(8) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        unsigned int hiddenOption = 0;     // ORIG local_190[1]（col10）
        if (!db->get_uint(col++, hiddenOption))
        {
            // ORIG 0840c547 在 ctor 前 `mov -0x10,%eax; lea -0x1(%eax),%ebx`
            // 跨 ctor 保活 ⇒ P227 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x275c, 5)
                ("DB_LoadInventory::GetUserAvatar, get_int(%d) ERROR"
                 " charac_no=%u", col - 1, characNo);
            return 0;
        }
        unsigned int emblemEndurance = 0;  // ORIG local_190[0]（col11）
        if (!db->get_uint(col++, emblemEndurance))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2762, 5)
                ("DB_LoadInventory::GetUserAvatar, get_int(%d) ERROR"
                 " charac_no=%u", col - 1, characNo);
            return 0;
        }
        short color1 = 0;
        if (!db->get_short(col++, color1))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x276a, 5);
            t("DB_LoadInventory::GetUserAvatar, get_short(10) ERROR charac_no=%u",
              characNo);
            // ORIG：本列失败仅记录日志，继续执行
        }
        short color2 = 0;
        if (!db->get_short(col++, color2))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2771, 5);
            t("DB_LoadInventory::GetUserAvatar, get_short(10) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        // ORIG 0840c6fe/0840c712：两个 color 写点在 tradeRestrict 初始化
        // （0840c726）与 get_byte(14) 之前 —— 原码写序如此，勿合并到尾部。
        out[row].m_expansionInfo.m_color[0] = color1;  // +0x47（movw）
        out[row].m_expansionInfo.m_color[1] = color2;  // +0x49（movw）
        char tradeRestrict = 0;
        if (!db->get_byte(col++, tradeRestrict))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x277d, 5);
            t("DB_LoadInventory::GetUserAvatar, get_byte(11) ERROR charac_no=%u",
              characNo);
            return 0;
        }

        // ---- AvatarDetailInfo 条目（步长 0x4b，ORIG _GetUserAvatar.md 写点：
        // +0x47/+0x49 movw short、+0x04/+0x00/+0x08 movl、+0x0c strncpy 0x18、
        // +0x25 movl、+0x29 起 7×movl+1×movw 共 0x1e 字节）----
        // P183：ORIG 每个写点重发 `mov -0x14(%ebp),%eax; imul $0x4b,%eax,%eax;
        // add 0xc(%ebp),%eax`（0840c6fe/712/71b/…/805），禁缓存 `T& e` 引用。
        out[row].m_expireDate = expireDate;            // +0x04（movl）
        out[row].m_key = uiId;                         // +0x00（movl）
        out[row].m_registTime = regDate;               // +0x08（movl）
        strncpy(out[row].m_ipgAgencyNo, ipgAgencyNo, 0x18);  // +0x0c
        out[row].m_clearAvatarId = clearAvatarId;      // +0x25（movl）
        // b28d-83/84：ORIG +0x29 为 7×movl+1×movw 结构整体赋值（非 memcpy call）
        // ⇒ 用 stAvatarEmblemInfo_t 结构赋值（同 CInventory.cpp:5554 镜像法）。
        out[row].m_jewelSocket = jewel;                // +0x29（ORIG 逐字段 8 写）

        // ---- Inven_Item 槽位（avatar 主区 / equip 镜像区）----
        // ORIG 0840c85b：`cmp $0x72; jg 尾` → `cmp $9; jle 主区`（主区物理在后）
        // ⇒ 镜像分支 fall-through、主区分支后置。主区入口 0840c956 还有一次
        // 冗余 `cmp $9; jg 尾`（原码嵌套 if 遗留，4.1 -O0 照发）。
        // P223：ORIG 三个失败出口各自在函数尾成 `nop; jmp join` 岛
        // （0840ca42/0840ca45/0840ca47 前的 jmp），4.4 复现形 = 负条件 goto
        // 到共享尾空标签。
        if (slot >= 0x73) goto L_avatarJoin;
        if (slot >= 10)
        {
            int idx = slot - 10;
            if (idx >= 0x69) goto L_avatarJoin;
            {
                    // ORIG 0840c88f..0840ca3f：idx 分支基址 `add 0x10(%ebp),%eax`
                    // = 第 3 形参（调用点 sig+0x4044）。P183：每写点重发
                    // `mov -0xc(%ebp),%eax; imul $0x3d,%eax,%eax; add 0x10(%ebp),%eax`。
                    inven[idx].m_itemType = Inven_Item::ITEM_TYPE_AVATAR;
                    inven[idx].m_endurance = static_cast<unsigned short>(abilityNo);
                    inven[idx].m_addInfo = itId;
                    inven[idx].m_addInfo2 = uiId;
                    inven[idx].m_amp.SetLock(lockKey);
                    inven[idx].m_fieldSplit.m_hiddenOption =
                        static_cast<short>(hiddenOption);
                    inven[idx].m_fieldSplit.m_avatarEndur =
                        static_cast<unsigned short>(emblemEndurance);
                    // ORIG 0840c92f/0840c947：单次基址 `movzbl 0x33(%eax),%edx;
                    // or $0x20,%edx; mov %dl,0x33(%eax)`（else 为 and $0xdf）。
                    // = 常量位域赋值 if/else 形（探针 /tmp/p_sep.cpp f4 实证 4.4
                    // 单次基址 6 条/分支；m_field 复合赋值 f1 恒两次基址 9 条/分支，
                    // 三目位域形反产读-改-写位域全展开）。
                    if (tradeRestrict != 0)
                    {
                        inven[idx].m_upgradeSep.m_bits.m_tradeRestriction = 1;
                    }
                    else
                    {
                        inven[idx].m_upgradeSep.m_bits.m_tradeRestriction = 0;
                    }
            }
        }
        else
        {
            if (slot >= 10) goto L_avatarJoin;
            {
                    // ORIG 0840c956..0840ca3f：slot<10 分支基址 `add 0x14(%ebp),%eax`
                    // = 第 4 形参（调用点 sig+0xa3a1）——与镜像分支相反，勿再对调。
                    mirror[slot].m_itemType = Inven_Item::ITEM_TYPE_AVATAR;
                    mirror[slot].m_endurance = static_cast<unsigned short>(abilityNo);
                    mirror[slot].m_addInfo = itId;
                    mirror[slot].m_addInfo2 = uiId;
                    mirror[slot].m_amp.SetLock(lockKey);
                    mirror[slot].m_fieldSplit.m_hiddenOption =
                        static_cast<short>(hiddenOption);
                    mirror[slot].m_fieldSplit.m_avatarEndur =
                        static_cast<unsigned short>(emblemEndurance);
                    if (tradeRestrict != 0)
                    {
                        mirror[slot].m_upgradeSep.m_bits.m_tradeRestriction = 1;
                    }
                    else
                    {
                        mirror[slot].m_upgradeSep.m_bits.m_tradeRestriction = 0;
                    }
            }
        }
        L_avatarJoin:;
        ++row;
    }
    return 1;
}