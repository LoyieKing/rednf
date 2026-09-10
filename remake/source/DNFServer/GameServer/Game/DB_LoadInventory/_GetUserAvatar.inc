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
                    inven[idx].m_field1 = Inven_Item::ITEM_TYPE_AVATAR;
                    inven[idx].m_fieldb = static_cast<unsigned short>(abilityNo);
                    inven[idx].m_addInfo = itId;
                    inven[idx].m_addInfo2 = uiId;
                    inven[idx].m_amp.SetLock(lockKey);
                    inven[idx].m_fieldSplit.m_fielddLo =
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
                        inven[idx].m_upgradeSep.m_bits.m_flag = 1;
                    }
                    else
                    {
                        inven[idx].m_upgradeSep.m_bits.m_flag = 0;
                    }
            }
        }
        else
        {
            if (slot >= 10) goto L_avatarJoin;
            {
                    // ORIG 0840c956..0840ca3f：slot<10 分支基址 `add 0x14(%ebp),%eax`
                    // = 第 4 形参（调用点 sig+0xa3a1）——与镜像分支相反，勿再对调。
                    mirror[slot].m_field1 = Inven_Item::ITEM_TYPE_AVATAR;
                    mirror[slot].m_fieldb = static_cast<unsigned short>(abilityNo);
                    mirror[slot].m_addInfo = itId;
                    mirror[slot].m_addInfo2 = uiId;
                    mirror[slot].m_amp.SetLock(lockKey);
                    mirror[slot].m_fieldSplit.m_fielddLo =
                        static_cast<short>(hiddenOption);
                    mirror[slot].m_fieldSplit.m_avatarEndur =
                        static_cast<unsigned short>(emblemEndurance);
                    if (tradeRestrict != 0)
                    {
                        mirror[slot].m_upgradeSep.m_bits.m_flag = 1;
                    }
                    else
                    {
                        mirror[slot].m_upgradeSep.m_bits.m_flag = 0;
                    }
            }
        }
        L_avatarJoin:;
        ++row;
    }
    return 1;
