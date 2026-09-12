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
        // [R11a06 2026-09-12 ④ 修复] 旧译文 `p[0x18]=rec[0x18]; p[0x19]=rec[0x19];
        //   *(short*)(p+0x1a)=*(short*)(rec+0x1a)` 的目标偏移**错**：+0x18..+0x1b 落在
        //   m_name[0x14..0x17]（角色名被覆写），而 m_field26(+0x26 job)/m_level(+0x27)/
        //   m_field29(+0x29 STGrowthType) 从未被写（恒 Reset 值）。ORIG 反汇编实证
        //   （raw 字节，VA 0x08649d10..0x08649d4e）：
        //     0x08649d20 `88 85 11 e4 ff ff` = mov %al,-0x1bef(%ebp)  → base(-0x1c15)+0x26 ← rec[0x18]
        //     0x08649d36 `88 85 14 e4 ff ff` = mov %al,-0x1bec(%ebp)  → base+0x29 ← rec[0x19]
        //     0x08649d4c `66 89 85 12 e4 ff ff` = mov %ax,-0x1bee(%ebp) → base+0x27(short) ← rec[0x1a]
        // [R11a06 2026-09-12 ④ 修复·形选] 三写按 ORIG 的「逐点重算 imul $0x94 + 加基址」
        // 形书写（ORIG 0x08649d10/0x08649d26/0x08649d3c 各 5 条：mov i; imul $0x94; add base;
        // movzbl/movzwl disp(%eax); mov 槽）。已实测三形：缓存 rec（4 条/处，len 310）/
        // 本形（6 条/处，len 317，ndiff 389 ratio 0.3855）/ 数组视图形（len 318，ndiff 402）
        // ⇒ 取本形（长度与 ORIG 316 最近、ndiff 最优）。
        info.m_field26 = *(data + i * 0x94 + 0x18);                      // +0x26 job
        info.m_field29.m_growthType =
            static_cast<unsigned char>(*(data + i * 0x94 + 0x19));       // +0x29
        info.m_level = *reinterpret_cast<short*>(data + i * 0x94 + 0x1a);  // +0x27 short

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
        // [R21a13 2026-09-13 形态修复·逐槽证据] 旧文用具名 `const unsigned char grow`
        //   占栈槽（我方 .o `movzbl %al,%eax; mov %al,-0x1e(%ebp)`），ORIG 0x08649cf7/
        //   0x08649d0d/0x08649d1f 为**三次逐点重读** `imul $0x94; add 基址;
        //   movzbl 0x22(%eax)`（无 grow 槽）⇒ 按 P0 律去槽：条件与取值处各自内联
        //   `static_cast<unsigned char>(rec[0x22])`。语义等价（同址同宽只读）。
        info.m_viewState =
            (static_cast<unsigned char>(rec[0x22]) == 1 ||
             static_cast<unsigned char>(rec[0x22]) ==
                 static_cast<unsigned char>(-1))
                ? static_cast<int>(static_cast<unsigned char>(rec[0x22]))
                : 0;

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

        {   // local_dd9：先 `|=1`、再 `&0xfd | bit*2`（rec[0x1c] bit4）
            // [Lane6 2026-09-10 真语义 bug 修复] ORIG 0x0864c4xx `movzbl 0x1c(%eax),%eax;
            // shr $0x4,%al; and $0x1,%eax; shl $0x7,%eax; sar $0x7,%al; and $0x1,%eax`
            // = **bit4 单 bit**（末尾 and $1 归一）。旧译文 `((x >> 4) << 7) >> 7` 在 4.4
            // 下恒等 ⇒ 得到完整半字节（0..15），使 `| (bit*2)` 污染 dst 高位 ⇒ 语义不同。
            // [R21a13 2026-09-13 形态修复·逐槽证据] 旧文把 bit 落具名 uchar 槽
            //   （我方 .o `movzbl (%eax); movzbl %al,%eax; sar $0x4,%eax; and $0x1,%eax;
            //   mov %al,-0x11(%ebp)`），ORIG 0x08649f5c..0x08649f6f 全程寄存器
            //   （`shr $0x4,%al; and $0x1,%eax; shl $0x7,%eax; sar $0x7,%al; and $0x1,%eax;
            //   lea (%eax,%eax,1),%edx`，无槽）；且 ORIG 0x08649f40..0x08649f4a 为
            //   **独立第一段写** `movzbl -0xdd5(%ebp); or $0x1; mov %al,-0xdd5(%ebp)`
            //   （bit0=1），随后 0x08649f5c 段才 `and $0xfffffffd; or %edx` ⇒ 源形是
            //   两条独立语句，非旧文的单条合并式 ⇒ 本处同时去槽 + 拆两条（等价）。
            // [R21a13 2026-09-13 形态修复·逐槽证据] 旧文再用一条
            //   `unsigned char& dst = info.m_tagVisibleFlags.m_flag;` 引用局部，
            //   4.4 对引用恒落槽并缓存地址（我方 .o `lea -0x1c1d(%ebp),%eax;
            //   add $0xe40,%eax; mov %eax,-0xc(%ebp)` + 两次 `mov -0xc(%ebp),%eax`
            //   回读）；ORIG 0x08649f40/0x08649f72 为**单条 disp 直访**
            //   `movzbl -0xdd5(%ebp),%eax` / `mov %al,-0xdd5(%ebp)`
            //   （-0xdd5 = 对象基 -0x1c15 + 0xe40）⇒ 去引用、直接成员访问。
            info.m_tagVisibleFlags.m_flag = static_cast<unsigned char>(
                info.m_tagVisibleFlags.m_flag | 0x1);
            info.m_tagVisibleFlags.m_flag = static_cast<unsigned char>(
                (info.m_tagVisibleFlags.m_flag & 0xfd) |
                (((static_cast<unsigned char>(rec[0x1c]) >> 4) & 1) * 2));
        }
        // [R20a13 2026-09-13 ④ 修复] 删 `p[0x1c04] = expireFlag;`：ORIG
        //  0x08649c6a..0x0864a185 全函数**无**该写（ci+0xe66 的同值写唯一落在
        //  0x08649ec1 `mov %al,-0xdaf(%ebp)`，即上方 m_fieldE66 一行）；sizeof
        //  (_Charac_info)==0x14d1 ⇒ +0x1c04 越界。生成码 `lea 0x1c04(%eax),%edx;
        //  mov %al,(%edx)`（我方 .o 0x824fa02）落 ebp-0x21 = 本函数 `char* p`
        //  局部（-0x24）的**最高字节** ⇒ 破坏自身 ci 指针，随后两处
        //  CInventory/CCargo::SetInventoryMemory(&p->…) 收野指针（内存破坏级）。
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
            // [R21a13 2026-09-13 形态修复·逐槽证据] 旧文 `Stream* s = ...Acquire(...);
            //   CStreamGuard guard(s, true);` 占具名槽（我方 .o `call Acquire;
            //   mov %eax,-0xc(%ebp); …; mov -0xc(%ebp),%eax`），ORIG 0x0864a15x 为
            //   内联实参 `call Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp);
            //   lea -0x38(%ebp),%eax; mov %eax,(%esp); call CStreamGuard::C1`（无 s 槽）
            //   ⇒ 按 P0/P259 去槽：Acquire 结果作 guard ctor 内联实参。
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("user.cpp", 0xcb3), true);
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