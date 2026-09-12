// ORIG execute 0x084aed04：LOTTERY 类物品抽样统计并把结果文件内容分包回显。
// [b28d-47] verdict=DIFF(routed) [N78/N82] ORIG 0x084aed04 orig/ours=269/241
//   残差：EXTRA memset（N78 内联清零差）+ 全帧槽号平移。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=29 [N7/memset-edx] p167 CLEAR(33/33)；
// memset 第三实参 edx 中转 + 虚调用 receiver 装载序（N7）+ 跳表基址折叠。
// [b28d-solo-2] verdict=DIFF：orig/ours=269/273。残余（逐条核对）：①首 memset
// 实参序；②`if (!goldLottery)` ORIG `cmpb $0,-0x1a; je` vs 4.4 `movzbl; xor $1; test; je`
// （+2 条，P122/N122 封顶，探针 p17 g1..g6 全为 xor 物化）；③虚调用 receiver
// 寄存器角色（N7）；④跳表基址 `.rodata` 重定位（AE）。
// [b28e-solo-2] ①已修：同 CBindCubeTest::execute，ORIG 函数头 fileBuf 清零是
// 数组初始化形（size 经 %edx 中转），`char fileBuf[95000] = {0};` 精确复现。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%edx; mov -0x20(%ebp),%eax;
//   mov %eax,(%esp); call *%edx` vs 4.4.7 `mov (%eax),%eax; … call *%eax`（residual 4 行）。
void CBoosterTest::execute()
{
    CUser* user = GetUser();

    // [b28d-solo-GmCmds_A] P312：ORIG 对 fileName[0xff](-0x153) 发内联对齐剥离
    // 清零（无 memset call，p167 EXTRA memset）。
    char fileName[0xff] = {0};
    char fileBuf[95000] = {0};

    // [R11fixE5] ORIG 0x084aed98 `movl $0x8c7fffc,0x4(%esp)` ⇒ 字面量直传（.rodata 匿名串）；
    // 旧 `static char sMsgStart[]` 落 .data 且多发 local 符号（strdiff 报 7 处 ORIG-only）。
    CBoosterTest::SendChatMsg(user, "LOTTERY \xbe\xc6\xc0\xcc\xc5\xdb \xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb8\xa6 \xbd\xc3\xc0\xdb\xc7\xd5\xb4\xcf\xb4\xd9");

    if (m_mode == -1)
    {
        // [R11fixE5] ORIG 0x084aedb6 串=0x8c80021（raw 逐字节同下）。
        CBoosterTest::SendChatMsg(user, "\xbe\xc6\xc0\xcc\xc5\xdb \xc0\xce\xb5\xa6\xbd\xba\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xcf\xbc\xbc\xbf\xe4");
        return;
    }

    CItem* item = G_CDataManager()->find_item(m_mode);
    if (item == 0)
    {
        // [R11fixE5] ORIG 0x084aedee 串=0x8c8003c（raw 逐字节同下）。
        CBoosterTest::SendChatMsg(user, "\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9");
        return;
    }

    if (m_count > 1000000 || m_count < 0)
    {
        // [R11fixE5] ORIG 0x084aee1d 串=0x8c80058（raw 逐字节同下）。
        CBoosterTest::SendChatMsg(user, "0 ~ 1000000\xb9\xf8 \xc5\xeb\xb0\xe8\xb1\xee\xc1\xf6\xb8\xb8 \xc1\xf6\xbf\xf8\xb5\xcb\xb4\xcf\xb4\xd9");
        return;
    }
    if (m_count == 0)
    {
        m_count = 10000;
    }

    char countMsg[21];
    sprintf(countMsg, "%d\xb9\xf8 \xc5\xeb\xb0\xe8\xc0\xd4\xb4\xcf\xb4\xd9", m_count);
    CBoosterTest::SendChatMsg(user, countMsg);

    bool goldLottery = this->is_gold_lottery_item(item->get_index());

    std::map<int, int> result;
    switch (item->GetItemType())                       // ORIG vtbl[3]
    {
    // [b28e-solo-2] P324：ORIG 叶块物理序 = 0xf/0x10 → 6/7/0xd → 0x1a/0x1b
    // → 0x14 → default（ORIG 84aeedb/84aeee5/84aef38/84aef53/84aef82）⇒ 按
    // ORIG 块地址序重排 case（case 顺序不改语义，仅决定 .text 块序）。
    case 0xf:
    case 0x10:
        this->getBoosterResult(item, result);
        break;
    case 6:
    case 7:
    case 0xd:
        // [b28e-solo-2] ORIG 正向 `cmpb $0,-0x1a; je <getJar>`（4.4 对
        // `if (bool)` 直发 cmpb 2 条；`if (!bool)` 恒 xor 物化 3 条，N122）+
        // get_gold 块在前 ⇒ 写正极性。
        if (goldLottery)
        {
            this->get_gold_lottery_result(item, result);
        }
        else
        {
            this->getJarResult(item, result);
        }
        break;
    case 0x1a:
    case 0x1b:
        this->getRandomboxResult(item, result);
        break;
    case 0x14:
        if (!this->getCompoundEmblemResult(item, result))
        {
            // [R11fixE5] ORIG 0x084aef6a 串=0x8c8007c（raw 逐字节同下）。
            CBoosterTest::SendChatMsg(GetUser(), "\xbf\xa5\xba\xed\xb7\xbd \xc7\xd5\xbc\xba \xb4\xeb\xbb\xf3\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9.");
        }
        break;
    default:
    {
        // [R11fixE5] ORIG 0x084aef87 串=0x8c80099（raw 逐字节同下）。
        CBoosterTest::SendChatMsg(GetUser(), "LOTTERY \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xc6\xb4\xd5\xb4\xcf\xb4\xd9");
        return;
    }
    }

    bool more = true;
    memcpy(fileBuf, "item_id\titem_name\trarity\titem_cnt\tprice\tsell_price\tusable_lv\n",
           0x3e);
    while (more)
    {
        // [b28e-solo-2] 同 switch：ORIG `cmpb $0,-0x1a; je <makeFileContents>`
        // 且 make_file_gold_contents 块在前 ⇒ 正极性。
        if (goldLottery)
        {
            CBoosterTest::make_file_gold_contents(user, fileBuf, result);
        }
        else
        {
            CBoosterTest::makeFileContents(user, fileBuf, result);
        }
        sprintf(fileName, "lottery_%d_%d.txt", m_mode, m_count);
        if (result.empty())
        {
            more = false;
        }
        CBoosterTest::printClient(user, more, fileName, fileBuf);
        memset(fileBuf, 0, sizeof(fileBuf));
    }

    // [R11fixE5] ORIG 0x084af090 串=0x8c80104（raw 逐字节同下）。
    CBoosterTest::SendChatMsg(user, "LOTTERY \xbe\xc6\xc0\xcc\xc5\xdb \xb0\xe1\xb0\xfa\xc3\xbc\xc5\xa9\xb0\xa1 \xbf\xcf\xb7\xe1\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9");
}