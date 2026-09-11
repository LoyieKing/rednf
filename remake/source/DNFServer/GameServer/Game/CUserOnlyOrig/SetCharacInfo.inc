// ============================================================================
// CUser::SetCharacInfo(int count, char* data)   ORIG T 0x08649c6a。
// 报告 docs/class_func_reports/CUser/SetCharacInfo.md 全文（汇编+反编译 C）已读。
// 逐记录（stride 0x94）填充栈上 _Charac_info（Reset→characNo/name/字段区/
// 角色快照表/建号时间/删除截止/禁玩与视图标记→CInventory/CCargo 绑共享
// InventoryMemory）后 push_back 至 m_characList(+0x796e8)。avatar 上报分支
// 依赖 scratch(local_748) 计数——ORIG 中该缓冲仅 memset 未写入，分支恒假，
// 按 ORIG 原样保留（Ghidra 同口径死分支）。
// ============================================================================
// [b28d-22 N82] ORIG 0x08649c6a：p167=CLEAR，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08649c6a：N82 封顶——帧/槽差（p167 CLEAR，gap 1）。
void CUser::SetCharacInfo(int count, char* data)
{
    char scratch[0x70c];
    memset(scratch, 0, 0x70c);                         // local_748：恒零载荷（ORIG 同）
    _Charac_info info;
    const int curSec = GlobalData::s_systemTime_.getCurSec();

    for (int i = 0; i < count; ++i)
    {
        // [L6 2026-09-10 P183 试形**实测反劣**] 删 `char* rec = data + i*0x94;` 缓存 +
        // 逐点 `rec + i*0x94` 内联（并删 `char* p = (char*)&info;`）后读数：
        // gap 0→**-52**、ndiff 430→**548**（ORIG-only imul 19→3 改善，但 OURS-only
        // lea 22/shl 9/add 18 新增 —— 4.4 对 `char* + i*0x94 + K` 折 lea/shl 链而非
        // ORIG 的 imul+独立 add）⇒ 已回滚。ORIG 0x08649cxx 的 19 处
        // `imul $0x94,%eax,%eax; add -0x1c(%ebp),%eax` 属 P30/N107 地址物化族封顶。
        char* rec = data + i * 0x94;
        info.Reset();                                  // _Charac_info::Reset
        char* p = reinterpret_cast<char*>(&info);
        info.m_characNo = *reinterpret_cast<int*>(rec);                  // characNo
        strncpy(info.m_name, rec + 4, 0x1e);                             // m_name
        p[0x18] = rec[0x18];
        p[0x19] = rec[0x19];
        *reinterpret_cast<short*>(p + 0x1a) =
            *reinterpret_cast<short*>(rec + 0x1a);

        for (int k = 0; k < 0xc; ++k)                  // 背包槽镜像表（0x3d stride）
        {
            *reinterpret_cast<int*>(p + 0xf8 + k * 0x3d) =
                *reinterpret_cast<int*>(rec + (k + 4) * 8 + 8);
        }
        for (int k = 0; k < 0xc; ++k)                  // 外观槽数组（8 字节槽，info+0x75c）
        {
            *reinterpret_cast<int*>(p + 0x75e + k * 8) =
                *reinterpret_cast<int*>(rec + (k + 4) * 8 + 8);
            *reinterpret_cast<short*>(p + 0x762 + k * 8) =
                *reinterpret_cast<short*>(rec + (k + 4) * 8 + 0xc);
            *reinterpret_cast<short*>(p + 0x764 + k * 8) =
                *reinterpret_cast<short*>(rec + (k + 4) * 8 + 0xe);
        }

        // growType：==1 或 ==0xff 原样，否则 0（→ info+0x14c6）
        const unsigned char grow =
            static_cast<unsigned char>(rec[0x22]);
        info.m_viewState =
            (grow == 1 || grow == static_cast<unsigned char>(-1))
                ? static_cast<int>(grow) : 0;

        reinterpret_cast<CommonTime*>(info.m_delReqTime)
            ->SetCreateTime(*reinterpret_cast<long*>(rec + 0x88));

        // [CoBatchA 2026-09-11 负结论·已回滚] 分支物化形（`if/else` 双赋）实测
        // **316→318 反劣**（4.4 仍发 setcc 且多 2 条）；ORIG 0x08649eb3/0x08649eb9 的
        // `jg; movb $0; jmp; movb $1` 属 4.1 条件物化族，4.4 不可复现 ⇒ 维持表达式形。
        const char expireFlag = (char)(curSec <
            *reinterpret_cast<int*>(rec + 0x90));
        info.m_fieldE66 = static_cast<unsigned char>(expireFlag);       // local_db3
        info.m_field12ad =
            *reinterpret_cast<int*>(rec + 0x90);                      // 删除截止
        info.m_fielde5d =
            *reinterpret_cast<int*>(rec + 0x8c);                      // local_dbc
        // [A06 2026-09-10 P0 具名 bool 试行记录·已回退] ORIG 0x08649f9x 段为
        // `movzbl 0x1c(%eax),%eax; and $0x1,%eax; test %al,%al; setne %al; mov %al,-0x9d5(%ebp)`
        // （bool 物化落槽）；补具名 `const bool` 局部实测**反劣**（316→318、delta 279→281）
        // ⇒ 该段属 4.1 转换路径产物（AND→bool 转换的 setne 在 4.4 被折），维持内联形并封 N。
        info.m_pad1240 = static_cast<char>((rec[0x1c] & 1) != 0);      // 禁玩标记
        info.m_field14b7 = static_cast<unsigned char>(rec[0x1d]);      // powerSide
        info.m_field14b8 =
            *reinterpret_cast<int*>(rec + 0x1e);                      // wpBonus

        {   // local_dd9：&0xfd | 1 | bit*2（rec[0x1c] bit4）
            // [Lane6 2026-09-10 真语义 bug 修复] ORIG 0x0864c4xx `movzbl 0x1c(%eax),%eax;
            // shr $0x4,%al; and $0x1,%eax; shl $0x7,%eax; sar $0x7,%al; and $0x1,%eax`
            // = **bit4 单 bit**（末尾 and $1 归一）。旧译文 `((x >> 4) << 7) >> 7` 在 4.4
            // 下恒等 ⇒ 得到完整半字节（0..15），使 `| (bit*2)` 污染 dst 高位 ⇒ 语义不同。
            const unsigned char bit = static_cast<unsigned char>(
                (static_cast<unsigned char>(rec[0x1c]) >> 4) & 1);
            unsigned char& dst =
                info.m_tagVisibleFlags.m_flag;   // +0xe40 tagCharacVisibleFlag 首字节成员（权威名）
            dst = static_cast<unsigned char>((dst & 0xfd) | 1 | (bit * 2));
        }
        p[0x1c04] = expireFlag;                                       // local_15

        reinterpret_cast<CInventory*>(p + 0xda)
            ->SetInventoryMemory(&m_inventoryMemory);                 // this+0x79794
        reinterpret_cast<CCargo*>(p + 0xdaa)
            ->SetInventoryMemory(&m_inventoryMemory);
        m_characList.push_back(info);
    }

    if (*reinterpret_cast<int*>(scratch) > 0)          // ORIG 恒假死分支（原样保留）
    {
        if (*reinterpret_cast<int*>(scratch) < 0x72)
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0xcb3);
            CStreamGuard guard(s, true);
            // ORIG 每条 << 前 call operator*()（0x080c8c46），P167 缺 2 处 operator*
            // （CStreamGuard::operator<< 成员直调不产 operator*，须显式 operator*()-> 代理链形）。
            guard.operator*()->operator<<((int)0x33);
            guard.operator*()->operator<<((int)GetUID());
            guard->put_binary(scratch, 0x70c);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        }
        else
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0xcaf, 5);
            t("SetCharacInfo AVATAR INFO PROBLEM items<%d> user_no<%s> characnum<%d>",
              *reinterpret_cast<int*>(scratch),
              NumberToString(get_acc_id(), 0), count);
        }
    }
}