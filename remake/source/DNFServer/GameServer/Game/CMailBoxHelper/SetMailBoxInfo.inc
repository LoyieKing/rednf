// 残余 DIFF 归因（语义/调用序/字段序已按 ORIG 逐段恢复，insns 1086 vs 1117）：
//  N1 族——ORIG(4.1.2) 条目基址 combine 拆叠 `i*0x47; add $0x7e0; add info;
//        add $0x7`（0x08552822：0x7a4+0x43 拆为 dword 对齐段 0x7c0/0x7e0 前置
//        + 尾余 0x7/0x9）、`add $0x20; mov 0xa(%eax)`（0x08552a45）——4.4.7
//        恒折 0x7a4+0x43 或 info-先行形态，i-first 表达式序已试，不可复现；
//  P112——4.4.7 将浅层 dead 间隔槽位合并（nowSec 并入 len 层），致 -0x40..-0x4c
//        一层平移 4 字节；声明序重排已把 -0x1c..-0x3c 与 -0x2b0..-0x384 深区
//        对齐，剩余层不可控；
//  P75/P52——avatar/creature 段 ORIG 先物化 3 int 至 edi/esi/ebx 再调 getter，
//        4.4.7 先求值链式对象表达式，装载时刻跨版本调度差异。
// [b28d-L8] N82 | 全帧槽平移（-0x2e4 vs -0x2e0，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] N 封顶（§6.2 三件套，p167 CLEAR 125/125，1086 vs 1100）：
// [P329-布局 本泳道 2026-09-09] ndiff 801 → 704（orig=1083 ours=1101）。
// 唯一改动的语义等价重构：明细条目循环的 if/else **体交换 + 条件反转**
// （`if (FindPackageLoadLack(...)!=0) {insert} else {put_*}`
//  → `if (FindPackageLoadLack(...)==0) {put_*} else {insert}`），
// 并把内层 isUnlimitMail 三元也照 ORIG `==0 → put_int(+0x1141)` 反转。
// 判据（callee 序列对照，非行 diff）：两侧 call 多重集 125=125 且**有序序列**只差
// 一处 —— make_pair/pair::pair/map::insert 三连在 ORIG 落位于明细段 put_* 之后
// （call[103:106]），我方却在前（call[91:94]）。⇒ ORIG 源码的 then 侧是打包路径，
// insert 是被后置的冷块；体交换前，我方整段明细与 ORIG 错行（单个 diff 块达 82 行）。
// 另核 isUnlimitMail/FindPackageLoadLack 两处条件均按 ORIG `xor $0x1; test; je` 取反。
//
// 残差 704 归因（已按 P329→P183 两筛子走完，不再试旋钮）：
//  · N82/P112 巨件全帧槽平移（本函数 30+ 个单行 diff 块，均为同指令不同槽号）；
//  · N107/P321-T5 地址常数拆分：ORIG `add $0x7e0; add 0xc(%ebp); add $0x7`
//    vs 4.4 合并成 `add $0x7a4 ... add $0x43`，以及 ORIG 单条
//    `lea (%edx,%eax,1),%eax` vs 4.4 两条 `add/add`（已逐偏移核对 0x7e7/0x7c9/
//    0x7b0 三处**完全等值**，非字段错位）；
//  · P183 物化次数：`imul $0x12b` 15=15 ✓、`imul $0x47` 6=6 ✓、
//    `imul $0x61` 我方 35 vs ORIG 29（+6，MB_ENTRY 重复物化，定位需逐段比 ORIG
//    缓存点，收益 ≈25 行，本波未做）；
//  · strlen@plt（链接形式，ORIG 已链接 vs 我方 .o 重定位）。
// [本波 2026-09-09] P292 全链 bool 化：isUnlimitMail 返回 bool + 两处条件改
// `== 0` 极性（ORIG 0x085530xx/0x085535xx 均 `xor $0x1; test %al; je`）⇒
// 归一化 ours 1101→1099（orig 1083 不变）。其余残差仍为上述 N82/N107/P183 族。
int CMailBoxHelper::SetMailBoxInfo(CUser* user, SIG_MAILBOX_LIST const* info)
{
    // ORIG 0x085525a6：DB 邮箱列表落地 → 增量/全量 0x61 列表包 + 信件明细。
    // 结构按 ORIG 汇编逐段：首段 0x61 header/字节写在 mail-map 循环处
    // （0x08552661..），但其 count/short 回填推迟到 SetNotLoadedMailCount 之后
    // （0x08552ed b 区），与信件明细共用同一个 packet 顺序追加；明细前还要
    // 先遍历 lackSet 把已在 m_letterInfo(+0x90) 暂存的信件补进包里并 erase
    // （0x08552f90..0x085531d5，上一版缺失段）。
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return 0;

    box->incMailLoadCount();
    bool bFirstLoad = !box->IsLoaded();

    PacketGuard packet;
    long nowSec = GlobalData::s_systemTime_.getCurSec();

    // +0x333a：PackageLoadLack 表（20 项）
    std::set<unsigned int> lackSet;
    box->SetPackageLoadLack(MBV->m_lackTable, 0x14, lackSet);

    int count = 0;
    int idx = 0;
    if (!bFirstLoad)
    {
        packet.put_header(0, 0x61);
        idx = packet.get_index();
        packet.put_byte(MBV->m_mailCount);
        packet.put_byte(1);
        for (MailMap::const_iterator it = mail_map(box).begin();
             it != mail_map(box).end(); ++it)   // ORIG 0x085526c4..0x085527bd
        {
            // [2026-09-09 SweepCMailBoxHelper-2 真 bug] 原写 `== end()` ⇒ 4.4 出
            // `je` 跳过 MakePacket（相等=不在 lackSet 才打包），ORIG 0x0852775 是
            // `jne 85527ae` 跳过（不相等=在 lackSet 中才打包）⇒ 极性相反。
            // ORIG 序列：0x0852753 `call set::find` → 0x085276e `call operator==`
            // → 0x0852773 `test %al,%al` → 0x0852775 `jne`（跳过 0x0852777..
            // 0x08527ac 的 MakePacket+count++）。语义 = 缺包表列出的信件才重发。
            // 必须保持 `operator==` 调用形（写 `!=` 会被 4.4 改调 `operator!=`，
            // 符号与 ORIG 不符）；否定式 `!(a == b)` 才产 ORIG 的 `test;jne`。
            // [2026-09-09 Lane20] ORIG 0x08552775 形为 `call operator==;
            // test %al,%al; jne <循环增量>`（正极性直测，无 xor）⇒ 写
            // `if (相等) continue;`（P168a：callee bool + 正极性 continue）。
            // 旧 `if (!(...))` 形多一条 `xor $0x1` 且 je 极性相反。
            if (lackSet.find(it->second->GetLetterId()) == lackSet.end())
                continue;
            it->second->MakePacket(&packet, nowSec, user);
            count = count + 1;
        }
    }

    // +0x7a0 avatar 注册区，步长 0x47
    for (int i = 0; i < MBV->m_avatarCount; ++i)
    {
        user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(
            MB_AVATAR(i)->m_uid, MB_AVATAR(i)->m_itemIdx, MB_AVATAR(i)->m_expire,
            MB_AVATAR(i)->m_name, MB_AVATAR(i)->m_emblem,
            MB_AVATAR(i)->m_expansion, false);
    }

    // +0xd30 creature 临时期限区，步长 0x25。ORIG 0x0855298a
    // RegistTempPeriod(iii) 三 int 自 base+4/+8/+0xc（base=info+0xd30+i*0x25）。
    for (int i = 0; i < MBV->m_creatureCount; ++i)
    {
        user->getCurCharacInvenW()->GetCreatureMgrW()->RegistTempPeriod(
            MB_CREATURE(i)->m_itemIdx, MB_CREATURE(i)->m_period,
            MB_CREATURE(i)->m_expire2);
    }

    box->SetLoadState(true, MBV->m_loadTime);

    // 信件条目区：count @+0x794，步长 0x61
    for (int i = 0; i < MBV->m_mailCount; ++i)
    {
        Inven_Item item;
        if (MB_ENTRY(i)->m_itemIdx == 0)
        {
        }
        else if (MB_ENTRY(i)->m_itemIdx == 1)
        {
            item.m_addInfo = MB_ENTRY(i)->m_itemIdx;
            item.m_addInfo2 = MB_ENTRY(i)->m_addInfo2;
            item.m_endurance = MB_ENTRY(i)->m_field33;
        }
        else
        {
            CItem* proto = G_CDataManager()->find_item(MB_ENTRY(i)->m_itemIdx);
            if (proto != 0)
            {
                proto->make_item(item);
                item.m_seal = MB_ENTRY(i)->m_itemField0;
                item.m_addInfo = MB_ENTRY(i)->m_itemIdx;
                item.m_addInfo2 = MB_ENTRY(i)->m_addInfo2;
                item.SetItemAttr(MB_ENTRY(i)->m_itemAttr);
                item.m_endurance = MB_ENTRY(i)->m_field33;
                item.m_extendInfo = MB_ENTRY(i)->m_field35;
                // ORIG 0x08552b4d/0x08552b6e/0x08552b78：item+0x15/0x19/0x1d
                // = amp guid（m_amp.m_guid，ORIG 0x8552b4a 逐条 mov 宽 int/int/word）。
                // [P331 本泳道实测反例 ⇒ 维持每句重发] ORIG 该段确实只物化一次基址
                // （`imul $0x61 … add $0x30,%eax` 后连读 +0x9/+0xd/+0x11），但把
                // `MB_ENTRY(i)` 提成 `Entry const* ep` 缓存后 4.4 并不像 4.1 那样把
                // base 保在 %eax：它把 ep 落栈槽、每句 `mov slot,%eax; mov (%eax)` 解引用，
                // 实测 ndiff 704 → **965**（多 261 行，且帧槽整体再平移）⇒ 已回滚。
                // 同族反例见 makeSendMailData 判词（N76/P183"引用局部会多一枚指针槽"）。
                item.m_amp.m_guid.m_serverId = MB_ENTRY(i)->m_field39;
                item.m_amp.m_guid.m_datetime = MB_ENTRY(i)->m_field3d;
                item.m_amp.m_guid.m_sequenceNo =
                    MB_ENTRY(i)->m_field41;
                item.m_amp.assign(MB_ENTRY(i)->m_ampType, MB_ENTRY(i)->m_ampIdx);
                // RandomOption（&item.m_random.m_random = item+0x25，ORIG 0x8552bd5
                // 实证 -0x34c）整块序列化恢复。旧偏移 0x1f/0x23/0x27/0x2b 系基址
                // 漂移，按 ORIG 逐条 int/int/int/word 归位到 +0x25/0x29/0x2d/0x31。
                InvenRandBlobV& randBlob =
                    *reinterpret_cast<InvenRandBlobV*>(&item.m_random.m_random);
                randBlob.m_w0 = MB_ENTRY(i)->m_field46;
                randBlob.m_w4 = MB_ENTRY(i)->m_field4a;
                randBlob.m_w8 = MB_ENTRY(i)->m_field4e;
                randBlob.m_wc = MB_ENTRY(i)->m_field52;
                item.m_upgradeSep.SetUpgradeSeparateAndTradeRestriction(
                    MB_ENTRY(i)->m_upgradeSep);
                if (MB_ENTRY(i)->m_attachType == 4)
                {
                    if (IsTradeLimitAttachTypeItem(item))
                    {
                        unsigned char limit = item.GetTradeLimitCount();
                        item.SetTradeLimitCount(limit + 0xff);
                        user->m_historyLog.ChangeItemLimitCount(
                            1, -1, (int)MB_ENTRY(i)->m_itemIdx, -1, 4);
                    }
                }
            }
        }

        // 邮件类型：+0x59 money / +0x5a coin → 1/2/0
        int pkgType = 0;
        if (MB_ENTRY(i)->m_moneyFlag != 0)
            pkgType = 1;
        else if (MB_ENTRY(i)->m_coinFlag != 0)
            pkgType = 2;

        // stAddNewMailInput 写入顺序 = ORIG 0x08552d05..0x08552dd4 逐槽序
        // （+0x24 item 指针最先，+0x10 attach→m_remainSec 最后）。
        CMailBox::stAddNewMailInput input;
        input.m_packageItem = &item;
        input.m_unlimit = MB_ENTRY(i)->m_unlimit;
        input.m_letterId = MB_ENTRY(i)->m_letterId;
        input.m_mailIndex = MB_ENTRY(i)->m_mailIdx;
        input.m_packageMoney = MB_ENTRY(i)->m_unk25;
        input.m_packageType = pkgType;
        input.m_period = MB_ENTRY(i)->m_money;
        input.m_senderCharacNo = MB_ENTRY(i)->m_characNo;
        input.m_senderName = MB_ENTRY(i)->m_senderName;
        input.m_remainSec = MB_ENTRY(i)->m_attachType;

        CMailBox::CMail* added = box->AddNewMail(input);
        // [R15a12 形态修复] ORIG 0x08552df2/0x08552e19 为**两个独立直跳守卫**
        // （`cmpl $0,槽; je skip` + `test %al,%al; jne skip`，无 bool 物化岛）
        // ⇒ 源码须拆嵌套 if；`&&` 链令 4.4 生成 `xor $1; test; je; mov $1/%eax;
        //   jmp; mov $0/%eax; test; je` 五条物化（P360/P225）。
        if (added != 0)
        {
            // P354 反向 if-else（空 then + else）消 4.4 的 `xor $0x1` 物化：
            // FindPackageLoadLack 返回 bool（CMailBoxHelper.h:165），ORIG
            // 0x08552e19 `test %al,%al; jne <skip>` 无 xor；写 `== 0` 正极性
            // 会令 4.4 多发一条 xor（P292/P354）。
            if (box->FindPackageLoadLack(MB_ENTRY(i)->m_letterId) != 0)
            {
            }
            else
            {
                count = count + 1;
                if (!bFirstLoad) {
                    added->MakePacket(&packet, nowSec, user);
                }
            }
        }
    }

    box->SetNotLoadedMailCount(MBV->m_notLoadedMailCount);
    if (bFirstLoad)
    {
        MakeMailBoxListPacket(user, &packet);
        if (MBV->m_mailCount > 0)
            SetPeriodCheckTimer(user);
        box->ClearLetterKeepCount();
        box->SetLoadedLetterCount(0);
    }
    else
    {
        // ORIG 0x08552edb：首包 count/未载数回填（与明细共用同一 packet，
        // finalize/Send 推迟到明细段之后）。
        packet.put_byte(idx, count);
        packet.put_short(MBV->m_notLoadedMailCount);
    }

    box->SetLoadedLetterCount(box->GetLoadedLetterCount() + MBV->m_loadedAdd);

    // ---- 明细段：ORIG 0x08552f41..0x0855356e。同一 packet 上继续追加：
    // get_index 暂存回填位 → put_short(明细总数) → 先消费 m_letterInfo 中
    // 与 lackSet 匹配的暂存信件 → 再遍历明细条目区（步长 0x12b）。----
    count = 0;
    idx = packet.get_index();
    packet.put_short(MBV->m_detailCount);

    for (std::set<unsigned int>::const_iterator sit = lackSet.begin();
         sit != lackSet.end(); ++sit)
    {
        std::map<unsigned int, SIG_LETTER_INFO>::iterator mit =
            box->m_letterInfo.find(*sit);
        if (mit != box->m_letterInfo.end())
        {
            // ORIG 每字段重读 operator->（0x08553004..0x085531a3，P157/P48：
            // strlen 双调用、isUnlimitMail 用 value+0x0）。
            packet.put_int(mit->second.m_letterId);
            packet.put_int(mit->second.m_characNo);
            packet.put_int((int)std::strlen(mit->second.m_title));
            packet.put_str(mit->second.m_title,
                           (int)std::strlen(mit->second.m_title));
            packet.put_int((int)std::strlen(mit->second.m_message));
            packet.put_str(mit->second.m_message,
                           (int)std::strlen(mit->second.m_message));
            // [R15a12 形态修复] ORIG 0x0855313e 实证 `test %al,%al; je <else>`
            // （**无 xor**）、then = put_int(0) ⇒ 源码为正极性 `!= 0`；负极性
            // `== 0` 令 4.4 多发 `xor $0x1,%eax`（P354）。
            if (isUnlimitMail(info, mit->second.m_letterId) != 0)
                packet.put_int(0);
            else
                packet.put_int(mit->second.m_period);
            packet.put_short((int)mit->second.m_flag);
            count = count + 1;
            box->m_letterInfo.erase(mit);
        }
    }

    for (int i = 0; i < MBV->m_detailCount; ++i)
    {
        // [P329-布局] ORIG 0x085534xx：FindPackageLoadLack 结果 `xor $0x1; test; je`
        // ⇒ 非缺乏（put 路径）fall-through，insert 块被 GCC 后置。源码等价反转
        // （`== 0` 时打包，否则暂存）才能复现该基本块序；反前置 ours 整段与 ORIG 错行。
        if (box->FindPackageLoadLack(MB_DETAIL(i)->m_letterId) == 0)
        {
            packet.put_int((int)MB_DETAIL(i)->m_letterId);
            packet.put_int((int)MB_DETAIL(i)->m_characNo);
            // ORIG 0x08553407 段：strlen 结果存同一局部 local_40 复用
            // （put_int 与 put_str 各用一次，每字段仅 1 次 strlen）。
            int len = (int)std::strlen(MB_DETAIL(i)->m_title);
            packet.put_int(len);
            packet.put_str(MB_DETAIL(i)->m_title, len);
            len = (int)std::strlen(MB_DETAIL(i)->m_message);
            packet.put_int(len);
            packet.put_str(MB_DETAIL(i)->m_message, len);
            // [R15a12 形态修复] ORIG 0x0855338e 实证 `test %al,%al; je <else>`
            // （**无 xor**）、then = put_int(0) ⇒ 正极性。
            if (isUnlimitMail(info, MB_DETAIL(i)->m_letterId) != 0)
                packet.put_int(0);
            else
                packet.put_int(MB_DETAIL(i)->m_period);
            packet.put_short((int)MB_DETAIL(i)->m_flag);
            count = count + 1;
        }
        else
        {
            // ORIG 0x08553407：make_pair(key 引用, value 引用) 同址
            // （0x101c 既作 key dword 又作 SIG_LETTER_INFO 首字段），
            // decay 后临时 pair → insert(pair const&)（0x08558848/886/88e2）。
            box->m_letterInfo.insert(
                std::make_pair(MB_DETAIL(i)->m_letterId, *MB_DETAIL(i)));
        }
        unsigned int lastIdx = (unsigned int)box->GetLastLoadLetterIdx();
        box->SetLastLoadLetterIdx(std::max(lastIdx, MB_DETAIL(i)->m_letterId));
        if (MB_DETAIL(i)->m_flag == 3)
            box->InsertLetterKeepCount(MB_DETAIL(i)->m_letterId);
    }
    packet.put_short(idx, count);
    packet.finalize(true);
    user->Send(packet);

    if (!bFirstLoad)
    {
        packet.clear();
        packet.put_header(1, 0x63);
        packet.put_byte(1);
        packet.put_short(box->GetNotLoadedMailCount());
        packet.finalize(true);
        user->Send(packet);
    }
    return 1;
}