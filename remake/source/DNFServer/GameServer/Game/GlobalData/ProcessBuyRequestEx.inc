bool ItemVendingMachine::ProcessBuyRequestEx(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
{
    if (user == 0)
    {
        cMyTrace tr(kPrettyProcessBuy, 0x290, 0);
        tr("There is no User in ItemVendingMachine");
        return false;
    }

    // ORIG 0x08176a4e..0x08176abd 初始化序（Ghidra C 与反汇编双向核对）：
    //   0x08176a4e movl $0xffffffff,-0x78（groupIdx=-1）
    //   0x08176a55/-0x74、0x08176a5c/-0x70、0x08176a63/-0x6c、0x08176a6a/-0x68（4 累计量=0）
    //   0x08176a71 call vector<pair<int,int> >::vector（local_170=-0x16c）
    //   0x08176a7f rep stos 0x24 dword @-0x280（related 指针数组，非单变量）
    //   0x08176a95 rep stos 0x24 dword @-0x310（goods 指针数组）
    //   0x08176aab call CUser::resetUsedCera
    // 0x081769f6..0x081783e9 全区间不存在 $0x23 立即数 ⇒ 原码没有条目数上限钳制；
    // 循环上界每处现读 msg 字节（0x08176bc5/0x08177dcc 区 movzbl 0x36(%eax)）。
    int groupIdx = -1;
    int totalCera = 0, totalGold = 0, totalWin = 0, totalMileage = 0;

    std::vector<std::pair<int, int> > pairs;
    // ORIG 0x08176a7f/0x08176a9b：两数组各 `mov $0x24,%edx; rep stos`（内联清零，
    // 无 call memset）⇒ 源形用 = {0} 触发 4.4 内联 rep stos（P328⑨）。
    const CItem* related[0x24] = {0};   // ORIG local_284（逐条目 _CheckEnableItemCondition 输出）
    CCeraShopGoods* goods[0x24] = {0};  // ORIG local_314

    user->resetUsedCera();

    const unsigned int itemCount = msg.m_scalars.m_itemCount;
    // [F2a-A轴·已试无效·N] 去 `const`（A 轴常量传播旋钮）：本件 ORIG 该两处
    //   （A08 筛查 idx 1250/1315，我方原为 `cmpl $0x22,-0x20(%ebp)`）确为槽读+寄存器
    //   比较 ⇒ 机理成立，但**方向不利**：横测 ours 1606→**1608**（+2，两处各 +1），
    //   ORIG 1600 ⇒ gap −6→**−8（劣化）**；本件我方指令数已多于 ORIG（N82 帧/槽膨胀），
    //   A 轴只会加深负 gap ⇒ 按「gap 严格减小」判据回滚，恢复 `const`。
    // 两处循环上限防御删除后本常量已无使用点，**结构保留不删**（Main 裁决 N）：
    //   ORIG 全函数无 `$0x22/$0x23` 只证「未参与比较」，不证「该局部不存在」；
    //   且 ORIG 帧 0x75c > 我方 0x69c ⇒ 删 4 字节会令帧更偏离 ORIG（P0 判据=ORIG 有无该槽）。
    const unsigned int MAX_ENTRIES = 0x23;   // 条目数上报上限（ProcessError 实参用）

    // ---- 第一遍：逐条目校验、总价累计 ----
    for (int i = 0; i < (int)itemCount; ++i)
    {
        int productNo = msg.m_entrySection.m_arr[i].m_productNo;
        unsigned char slotByte = msg.m_entrySection.m_arr[i].m_avatarType0;
        unsigned char tagByte = msg.m_entrySection.m_arr[i].m_avatarType1;
        (void)slotByte; (void)tagByte;

        // ORIG 0x08176ad5 G_CDataManager() → 0x08176ae8 直调
        // _ZNK12CDataManager9FindGoodsEi（0x0835fde4，内部转发
        // CItemList::FindGoods），结果 0x08176aed 直存 goods[i]（无 getItemList 层）。
        goods[i] = G_CDataManager()->FindGoods(productNo);
        if (goods[i] == 0)
        {
            m_ceraShop.ProcessError(user, 0x15, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            // ORIG 0x08176b95/0x08176bbb：每个校验失败分支 ProcessError + goods[i]=0
            // 之后固定跟一对 cMyTrace（具名槽 -0x150）：tr("[%s][%d]", PRETTY, line)。
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2ad, 0);
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2ad);
            continue;
        }

        groupIdx = goods[i]->GetGroupIdx();

        if (!_CheckEnableGiftCondition(goods[i], user, (char)msg.m_scalars.m_buyFlag))
        {
            m_ceraShop.ProcessError(user, 8, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2b4, 0);   // ORIG 0x08176ca2/0x08176cc8
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2b4);
            continue;
        }

        // IsBuyableRestrictItem(user, productNo, 1, i)：1=可购买（取反判不可购）。
        if (!CerashopAddRestrict::Manager::IsBuyableRestrictItem(user, productNo, 1, i))
        {
            m_ceraShop.ProcessError(user, 0x71, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2bb, 0);   // ORIG 0x08176d9f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2bb);
            continue;
        }

        if (!_CheckEnableItemCondition(goods[i], related[i], pairs))
        {
            m_ceraShop.ProcessError(user, 0x11, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2c2, 0);   // ORIG 0x08176e9f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2c2);
            continue;
        }

        if (!_CheckEnableStateCondition(goods[i], user))
        {
            m_ceraShop.ProcessError(user, 7, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2c8, 0);   // ORIG 0x08176f89
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2c8);
            continue;
        }

        int buyingCode = 0;
        if (!m_ceraShop._CheckBuyingCondition(user, goods[i], productNo,
                                              msg.m_entrySection.m_arr[i].m_avatarType0,
                                              (unsigned short)msg.m_entrySection.m_arr[i].m_avatarType1,
                                              (char)msg.m_scalars.m_buyFlag, buyingCode))
        {
            m_ceraShop.ProcessError(user, (unsigned short)buyingCode, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo,
                                    i + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2d6, 0);   // ORIG 0x08177105
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2d6);
            continue;
        }

        if (groupIdx == 0)
        {
            stAvatarTypeSelect_t avatarSel;
            if (!_CheckEnableAvatarCondition(goods[i],
                                             (char)msg.m_entrySection.m_arr[i].m_avatarType1,
                                             msg.m_entrySection.m_arr[i].m_avatarType0,
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx,
                                        msg.m_entrySection.m_arr[i].m_productNo,
                                        i + 1, itemCount,
                                        (char)msg.m_scalars.m_buyFlag);
                goods[i] = 0;
                cMyTrace tr(__PRETTY_FUNCTION__, 0x2ec, 0);   // ORIG 0x08177234
                tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2ec);
                continue;
            }
            totalCera += avatarSel.m_fieldc;
            totalGold += avatarSel.m_field8;
            totalWin += avatarSel.m_field4;
        }
        else
        {
            totalCera += goods[i]->GetCeraPrice();
            totalGold += goods[i]->GetGoldPrice();
            totalWin += goods[i]->GetWinPoint();
        }
        totalMileage += goods[i]->GetMileagePrice();

        if (msg.m_scalars.m_buytype == 1 && goods[i]->isCeraPointDisallowance_())
        {
            m_ceraShop.ProcessError(user, 0x138b, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x305, 0);   // ORIG 0x0817738f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x305);
            continue;
        }
    }

    // ---- 总校验 ----
    if (!m_ceraShop._CheckValidCount((const CCeraShopGoods**)goods))
    {
        m_ceraShop.ProcessError(user, 1, -1, msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                                (char)msg.m_scalars.m_buyFlag);
        return true;
    }

    // ---- 总价资金校验 ----
    int errCode = 0;
    if (user->GetCera() < totalCera) errCode = 0xb;
    else if (user->getCurCharacMoney() < totalGold) errCode = 10;
    else if (user->GetWinPoint() < totalWin) errCode = 0xc;
    else if (user->GetMileage() < totalMileage) errCode = 0x3ed;
    if (msg.m_scalars.m_buytype == 1)
    {
        errCode = 0;
        if (user->GetCeraPoint() < (unsigned int)totalCera)
            errCode = 0x138a;
    }
    if (errCode != 0)
    {
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x34b,
            "[ERROR] Not enough UserID[%s] ACera[%d] AGold[%d] AWP[%d] AMile[%d]",
            user->getCurCharacName(), totalCera, totalGold, totalWin, totalMileage);
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x34c,
            " / UCera[%d] UGold[%d] UWP[%d] AMile[%d]\n",
            user->GetCera(), user->getCurCharacInvenR()->get_money(),
            user->GetWinPoint(), user->GetMileage());
        m_ceraShop.ProcessError(user, (unsigned short)errCode, -1,
                                msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                                (char)msg.m_scalars.m_buyFlag);
        return true;
    }

    // ---- 总价资金校验 ----
    // ORIG 余额槽宽度实证（64 位比较序列 + max 模板实参 mangled）：
    //   0x0817765a call getCurCharacMoney → 0x08177661 `sar $0x1f,%edx`（**符号**
    //     扩展）→ mov %eax,-0x50 / mov %edx,-0x4c ⇒ curGold 是 signed long long。
    //   0x08177670 call GetWinPoint → 0x08177675 `mov $0x0,%edx`（**零**扩展）
    //     → -0x48/-0x44 ⇒ curWin 是 unsigned long long。
    //   0x0817768d call GetMileage → mov %eax,-0x38（单 dword）⇒ curMileage 是 int。
    // 扣减用 _ZSt3maxIi（0x08177991）+ _ZSt3maxIx ×2（0x081779e2/0x08177a37；
    //   c++filt 实证 `x` = **long long**（有符号），非 unsigned long long
    //   ⇒ 两个实参槽均须 long long 类型才能推导出同一实例）。
    long long curGold = user->getCurCharacMoney();
    long long curWin = user->GetWinPoint();
    int curMileage = user->GetMileage();
    int curSec = GlobalData::s_systemTime_.getCurSec();

    for (unsigned int j = 0; j < itemCount; ++j)
    {
        if (goods[j] == 0)
            continue;
        groupIdx = goods[j]->GetGroupIdx();

        int ceraPrice = 0, goldPrice = 0, winPrice = 0, avatarField = 0;
        if (groupIdx == 0)
        {
            stAvatarTypeSelect_t avatarSel;
            if (!_CheckEnableAvatarCondition(goods[j],
                                             (char)msg.m_entrySection.m_arr[j].m_avatarType1,
                                             msg.m_entrySection.m_arr[j].m_avatarType0,
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx,
                                        msg.m_entrySection.m_arr[j].m_productNo,
                                        j + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
                goods[j] = 0;
                cMyTrace tr(__PRETTY_FUNCTION__, 0x37b, 0);   // ORIG 0x081777d6
                tr("[%s][%d]", __PRETTY_FUNCTION__, 0x37b);
                continue;
            }
            ceraPrice = avatarSel.m_fieldc;
            goldPrice = avatarSel.m_field8;
            winPrice = avatarSel.m_field4;
            avatarField = avatarSel.m_field0;
        }
        else
        {
            ceraPrice = goods[j]->GetCeraPrice();
            goldPrice = goods[j]->GetGoldPrice();
            winPrice = goods[j]->GetWinPoint();
        }
        int mileagePrice = goods[j]->GetMileagePrice();
        int productNo = msg.m_entrySection.m_arr[j].m_productNo;

        int perErr = 0;
        if (user->GetCera() < (unsigned int)ceraPrice) perErr = 0xb;
        else if (user->getCurCharacInvenR()->get_money() < goldPrice) perErr = 10;
        else if (user->GetWinPoint() < (unsigned int)winPrice) perErr = 0xc;
        if (msg.m_scalars.m_buytype == 1)
        {
            perErr = 0;
            if (user->GetCeraPoint() < (unsigned int)ceraPrice)
                perErr = 0x138a;
        }
        // 0x08177917..0x08177962：三条**余额**校验（不是现读 user）。
        //  ① -0x50/-0x4c(curGold) vs sext(goldPrice) → 有符号 64 位比较，
        //     `jl/jg/cmp lo;jbe` 三段式 ⇒ `curGold < goldPrice` → perErr=10。
        //  ② -0x48/-0x44(curWin) vs zero-ext(winPrice) → 无符号 64 位比较 → 0xc。
        //  ③ -0x38(curMileage) vs -0x3c(mileagePrice) 32 位 `jge skip` → 0x3ed。
        // 0x08177917..0x08177a44：ORIG 把三条余额校验与**扣减**全部排在
        // 0x08177a47 `cmpl $0x0,-0x28(perErr); je 正常路径` 之前 ⇒ 即使本条目
        // 判错，余额照样被扣（原码行为，非笔误）。扣减三处 = std::max：
        // _ZSt3maxIi @0x08177991、_ZSt3maxIx @0x081779e2、@0x08177a37
        // （Itanium ABI `x` = unsigned long long，解释 i386 上为何读 0x4(%eax)）；
        // 声明顺序必须 diff 在前、zero 在后（ORIG 先 `mov %eax,-0xc0/-0xb8/-0xa8`
        // 后 `movl $0x0,-0xbc/-0xb0/-0xa0`，技巧 21 后声明占近 ebp 槽）。
        if (curGold < goldPrice) perErr = 10;         // 0x08177917 有符号 64 位比较
        if (curWin < (unsigned int)winPrice) perErr = 0xc;   // 0x08177935 winPrice 零扩展
        if (curMileage < mileagePrice) perErr = 0x3ed;             // 0x08177953 32 位
        {
            int diffMileage = curMileage - mileagePrice;   // ORIG -0xc0（先算差值）
            int zeroMileage = 0;                           // ORIG -0xbc
            curMileage = std::max(zeroMileage, diffMileage);   // 0x08177991
        }
        {
            long long diffGold = curGold - goldPrice;      // ORIG sub/sbb → -0xb8/-0xb4
            long long zeroGold = 0;                        // ORIG -0xb0/-0xac
            curGold = (long long)std::max(zeroGold, diffGold);   // 0x081779e2
        }
        {
            long long diffWin = curWin - (unsigned int)winPrice;  // ORIG -0xa8/-0xa4
            long long zeroWin = 0;                                // ORIG -0xa0/-0x9c
            curWin = std::max(zeroWin, diffWin);           // 0x08177a37
        }

        if (perErr != 0)
        {
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b3,
                "IPG ERROR [Dispatcher_BuyCeraShopItem::dispatch_sig] UserID[%s] ACera[%d] AGold[%d] AWP[%d] AMile[%d]",
                user->getCurCharacName(), ceraPrice, goldPrice, winPrice, mileagePrice);
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b4,
                " / UCera[%d] UGold[%d] UWP[%d] UMile[%d]\n",
                user->GetCera(), user->getCurCharacInvenR()->get_money(),
                user->GetWinPoint(), user->GetMileage());
            // 0x08177b1e..0x08177b30：第三处 logFormat 的 %d 实参是**现读**
            // msg 条目（`mov -0x2c,%eax; imul $0x1cf; lea (msg,…); add $0x30;
            // mov 0x9(%eax)` = entry+0x39 = m_productNo），不是局部 productNo。
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b9,
                "Error Goods No : %d", msg.m_entrySection.m_arr[j].m_productNo);
            m_ceraShop.ProcessError(user, (unsigned short)perErr, groupIdx,
                                    msg.m_entrySection.m_arr[j].m_productNo,
                                    j + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
            goods[j] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x3bb, 0);   // ORIG 0x08177bf4
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x3bb);
            continue;
        }

        // （扣减已按 ORIG 0x08177962..0x08177a44 前移至上方 perErr 判定之前）

        if (!user->IsRestrictedGoods(goods[j]->GetFeaturedIdx()))
        {
            Stream* stream = GlobalData::s_stream_pool->Acquire(
                "localjapan/AppendVendingMachine.cpp", 0x3cb);
            CStreamGuard guard(stream, false);
            // ORIG 0x08177d3b..0x08177d59：`lea -0x178; mov %eax,(%esp);
            // call CStreamGuard::operator->(); mov %eax,(%esp);
            // call GetInBuffer<stCeraShopIPGRequest_t>()` ⇒ 源码就是
            // guard->GetInBuffer<...>()（CStreamGuard::operator-> 返回 this），
            // 不得引入 g 中转变量（技巧 35/36：代理链 + 删中转槽）。
            stCeraShopIPGRequest_t* req =
                guard->GetInBuffer<stCeraShopIPGRequest_t>();
            req->m_productNo = productNo;
            req->m_groupIdx = goods[j]->GetGroupIdx();
            req->m_stackCount = (unsigned short)goods[j]->GetStackCount();
            req->m_ceraPrice = (unsigned int)ceraPrice;
            req->m_goldPrice = (unsigned int)goldPrice;
            req->m_winPoint = (unsigned int)winPrice;
            req->m_itemIdx = goods[j]->GetItemIdx();
            req->m_avatarType0 = msg.m_entrySection.m_arr[j].m_avatarType0;
            req->m_avatarType1 = msg.m_entrySection.m_arr[j].m_avatarType1;
            req->m_avatarField = (unsigned int)avatarField;
            req->m_featuredIdx = goods[j]->GetFeaturedIdx();
            req->m_itemCount = itemCount;
            req->m_seq = j + 1;
            req->m_curSec = curSec;
            req->m_buyFlag = msg.m_scalars.m_buyFlag;
            req->m_accId = msg.m_scalars.m_accId;
            req->m_characNo = msg.m_scalars.m_characNo;
            req->m_buytype = msg.m_scalars.m_buytype;
            req->m_subCount = msg.m_entrySection.m_arr[j].m_subCount;
            // [F2a-轴B+A 双旋钮·命中保留] ORIG 0x08177f2d/0x08177f30：
            //   `movzbl 0x3d(%eax),%eax; movzbl %al,%eax; cmp -0x20(%ebp),%eax;
            //    setg %al; test %al,%al; jne <回边>`（**有符号序 + 单条比较**）。
            //   我方原形：`unsigned int k` + `k < m_subCount && k < MAX_ENTRIES`
            //   ⇒ `cmp -0x20(%ebp),%eax; jbe` + 多出 `cmpl $0x22,-0x20(%ebp); ja`
            //   （发明上限防御；ORIG 全函数无任何 `$0x22/$0x23` 立即数，经报告段
            //    17..1620 全文 grep 实证 ⇒ 按 P171 照抄 ORIG 删除）。
            //   实改（单点）：`int k` + `(int)req->m_subCount` + 去 `&& k < MAX_ENTRIES`
            //   ⇒ 本函数 insns **1606→1600 = ORIG 1600（gap −6→0）**，该段四条形
            //   与 ORIG 逐条一致（cmp/setg/test/jne）；TU 四数
            //   899/1791/11/38 逐值不变（净 identical 未降）。
            //   **[语义轴：删除发明防御，不受 gap/ndiff 闸门约束]**（该件本为 DIFF、
            //   无 identical 回退风险；口径见 Main 分流判据）。
            //   [Main 裁决已落地] optCount 循环同族亦删（见下）；两点全删后
            //   insns **1606→1596**（ORIG 1600，gap −6→**+4**）——差值系**删除我方
            //   发明防御**所致（非劣化），Main 口径：「删除发明/多写代码以忠实 ORIG
            //   不受 gap 闸门约束」（该件本为 DIFF，无 identical 回退风险）。
            for (int k = 0; k < (int)req->m_subCount; ++k)
            {
                // 子项数组：entry+0x0e 起每项 5 字节 { int idx; uchar val; }
                // ORIG 0x08177ed6/0x08177f04：两处各自重算源地址直读直存
                // （`mov 0xe(%eax),%edx; mov %edx,0x8(..)` / `movzbl 0x42(%eax);
                //  mov %dl,0xc(..)`），无 memcpy call、无源字段缓存（P183/P329）。
                // [Main「视图 cast 反模式」修法 · R2 批准 · 探针 /tmp/l7/probe7.cpp]
                // 具名数组视角直访问：ORIG 0x08177edf `mov %edx,0x8(%eax,%ecx,8)` /
                // 0x08177f0d `mov %dl,0xc(%eax,%ecx,8)`（stride 8 scaled 直存）；
                // 旧 `*reinterpret_cast<int*>(&m_subEntries[k][0])=` 形被 4.4 折成
                // add+shl 再普通 store（probe7 f1）⇒ 改 SigTypes_E.h 的 union 具名
                // 视角 m_subItems（probe7 f2 = ORIG 形）。布局/sizeof/mangled 不变。
                req->m_subItems[k].m_itemIdx =
                    msg.m_entrySection.m_arr[j].m_subEntries[k].m_itemIdx;
                req->m_subItems[k].m_value =
                    msg.m_entrySection.m_arr[j].m_subEntries[k].m_value;
            }
            req->m_optCount = msg.m_entrySection.m_arr[j].m_optCount;
            // [F2a-轴B+照抄 ORIG·Main 裁决落地] optCount 循环同族：ORIG
            //   0x08178010/0x08178013 = `movzbl 0xed(%eax),%eax; movzbl %al,%eax;
            //   cmp -0x1c(%ebp),%eax; setg %al; test %al,%al; jne <回边>`
            //   （有符号序 + 单条比较；ORIG 全函数无 `$0x22/$0x23` 立即数）。
            //   Main 裁决：删除我方发明防御不受 gap 闸门约束（结构忠实优先）⇒
            //   `int k` + `(int)req->m_optCount` + 去 `&& k < MAX_ENTRIES`。
            //   实测：删后 insns 1600→1596、该段现为 `cmp -0x1c(%ebp),%eax; setg %al;
            //   test %al,%al; jne <回边>`（与 ORIG 逐条一致），全函数已无 `$0x22`；
            //   TU 四数 899/1791/11/38 仍逐值不变。
            for (int k = 0; k < (int)req->m_optCount; ++k)
            {
                // 选项数组：entry+0xbe 起每项 8 字节 { ulong itemIdx; ushort a; ushort b; }
                //（ORIG k 达 0x22 时读入邻条目尾，线格式遗留，数组越界读等价保留）
                // 选项项布局：{ ulong itemIdx(4, at +0); ushort a(2, at +4); ushort b(2, at +6) }
                //（dest 为 (k+0x2c)*8 + {4,8,10}，即 m_optEntries[k] 的 +0/+4/+6）。
                // ORIG 0x08177f87/0x08177fb4/0x08177fe3：三次独立重算源地址直读直存
                // （`mov 0xe(%eax),%edx; mov %edx,0x4(%eax,%ecx,8)` 形），
                // 无 memcpy call、无 srcOpt/itemIdx/a/b 具名缓存（P183/P329）。
                // 同上：ORIG 0x08177f87/0x08177fb4/0x08177fe3 三处 scaled 直存
                // （`mov …,0x4/0x8/0xa(%eax,%ecx,8)`）⇒ 用 m_optItems 具名视角。
                req->m_optItems[k].m_itemUid =
                    msg.m_entrySection.m_arr[j].m_optEntries[k].m_itemUid;
                req->m_optItems[k].m_a =
                    msg.m_entrySection.m_arr[j].m_optEntries[k].m_a;
                req->m_optItems[k].m_b =
                    msg.m_entrySection.m_arr[j].m_optEntries[k].m_b;
            }

            guard.operator*()->operator<<(mileagePrice);

            // ORIG 清零全部内联（4.1 rep stos/展开，无 call memset）：
            //   0x0817803b ceraPriceStr[-0x1f0] 0x24 → rep stos 0x9 dwords
            //   0x08178051 itemIdxStr[-0x18c] 0x14 → 5×movl
            //   0x08178083 uniqueStr[-0x1a1] 0x15 → 对齐剥离慢路
            //   0x081780e7 itemStr[-0x710] 0x400 → rep stos 0x100 dwords
            //   0x081780fd productStr[-0x1a9] 0x8 → 2×movl
            // ⇒ 源形用 = {0} 触发 4.4 内联清零（P312/P328⑨）；memset 在 4.4 恒 call。
            char itemStr[0x400] = {0};
            char ceraPriceStr[0x24] = {0};
            char productStr[8] = {0};
            char itemIdxStr[0x14] = {0};   // ORIG -0x18c（0x08178137 取址目标，结果未被使用）
            char uniqueStr[0x15] = {0};
            // ORIG 三次 snprintf 的**顺序与实参**（全部 OS_API::snprintf）：
            //  ① 0x08178140 snprintf(-0x18c, 0x14, "%d", goods[j]->GetItemIdx())
            //     （0x0817811e call _ZNK14CCeraShopGoods10GetItemIdxEv 结果作 %d，
            //      输出缓冲此后不再被读 ⇒ 原码遗留的死写入，照抄）
            //  ② 0x08178165 snprintf(-0x1f0, 0x24, "%d", ceraPrice[-0x58])
            //  ③ 0x0817819c snprintf(-0x1a9, 0x8, "%d", 现读 msg entry+0x39)
            OS_API::snprintf(itemIdxStr, sizeof(itemIdxStr), "%d",
                             goods[j]->GetItemIdx());
            OS_API::snprintf(ceraPriceStr, sizeof(ceraPriceStr), "%d", ceraPrice);
            OS_API::snprintf(productStr, sizeof(productStr), "%d",
                             msg.m_entrySection.m_arr[j].m_productNo);
            (void)itemIdxStr;
            _GetItemString(goods[j], itemStr, 0x400);
            // ORIG 0x081781d6..0x08178360（P5）：三分支各取 s_pIPGHelper vptr+0x18/
            // +0x10/+0x14 槽直调（slot6 IPGOutputSimulation / slot4 IPGOutputPoint /
            // slot5 IPGOutput，TaiwanCash.h 声明序实证）。每分支前各有一对
            // 0x081781eb/0x081781f3（及 0x0817827c/0x08178284、0x081782fc/0x08178304）
            // `call CStreamGuard::operator->()` + `call CStreamGuard::Get()` ⇒
            // 尾实参必须写 guard->Get()（链式，3 次），不得缓存到 theStream；
            // 也不得显式 guard.~CStreamGuard()（ORIG 仅 2 次 D1：作用域尾 1 次 +
            // EH 0x081783b0 1 次；显式调用会造成双重析构）。
            // 另：uniqueNo 与 characName 同为**分支内内联实参**（ORIG 三次
            // 0x08178210/0x081782a1/0x08178321 GenUniqueNo + 三次
            // 0x08178200/0x08178291/0x08178311 getCurCharacName）⇒ 不得提到
            // 分支外缓存成局部（否则各分支少 2 次 call）。
            if (ceraPrice == 0)
                GlobalData::s_pIPGHelper->IPGOutputSimulation(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
            else if (msg.m_scalars.m_buytype == 1)
                GlobalData::s_pIPGHelper->IPGOutputPoint(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
            else
                GlobalData::s_pIPGHelper->IPGOutput(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
        }
        else
        {
            m_ceraShop.ProcessError(user, 0x14, groupIdx,
                                    msg.m_entrySection.m_arr[j].m_productNo,
                                    j + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
            goods[j] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x3c1, 0);   // ORIG 0x08177cdc/0x08177d02
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x3c1);
        }
    }

    return true;
