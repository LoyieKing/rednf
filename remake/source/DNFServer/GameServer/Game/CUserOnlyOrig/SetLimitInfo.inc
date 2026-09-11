// ---------------------------------------------------------------------------
// CUser::SetLimitInfo(SIG_LIMIT_ITEM_INFO* info) T 0x086888f8 sz0x1ed
// 报告：docs/class_func_reports/CUser/SetLimitInfo.md
// 清空 m_limitItemUsage(+0x8cf68) 后遍历 CDataManager::GetLimitItemUsageInfo
// (m_info0.m_list)：sig 条目 itemId 匹配 LimitInfo.m_a → 填 usage{kind,m_b,m_a,
// entry.m_field0} + SetCreateTime(entry.m_createTime)；未匹配 → m_field19=0 +
// SetCurTime()。push_back 到 m_limitItemUsage。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x086888f8：p167=M0/E4:_ZN14LimitItemUsageD1Ev;_Unwind_Resume;_ZN9__gnu_cxx17__normal_iteratorIPN20stLimitItemUsageInfo9LimitInfoESt6vectorIS2_SaIS2_EEEppEv，gap=11；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x086888f8：P28 增量侧试形实测反劣（it++ 157 条 vs
// ++it 153，ORIG 142）⇒ 回退 ++it；p167 残余 ppEi×1 + LimitItemUsage D1×2/_Unwind_Resume
// （push_back EH landing，P216/N91 族）+ vector 模板 out-of-line（N89）；N82 封顶。
void CUser::SetLimitInfo(SIG_LIMIT_ITEM_INFO* info)
{
    if (info != 0) {
        // [H6 2026-09-11 语义轴·真字段错位修正] ORIG 0x086888f8 对 `info` 的读点（绝对偏移，
        // i*0x10 已含）：itemId @ +0x0c（0x08688985/0x08688995 `add $0xc,%eax; mov (%eax),%ebx`，
        // 与 it->m_a 比较）、写 usage.m_field19 的源 @ +0x10（0x086889e3 `add $0x10,%eax`）、
        // SetCreateTime 实参 @ +0x08（0x086889f6 `add $0x8,%eax`）。
        // SigTypes_E.h 的 `m_pad4[8]` + `m_entries@+0xc` + Entry{field0@0,field4@4,ct@8,itemId@0xc}
        // 使我们的对应读点落在 +0x18/+0x0c/+0x14 ⇒ 三处字段整体错位（运行时读错限购数据）。
        // 头修正属跨文件广播项，本轮只报告；此处按 ORIG 绝对偏移用局部视图读取（等价映射：
        // 元素基址 info+0x8、步长 0x10、createTime@元素+0 / itemId@+4 / field0@+8）。
        struct LimitInfoEntryView {
            long m_createTime;   // 元素+0x00 ⇒ 绝对 +0x08（ORIG 0x086889f6）
            int  m_itemId;       // 元素+0x04 ⇒ 绝对 +0x0c（ORIG 0x08688985）
            int  m_field0;       // 元素+0x08 ⇒ 绝对 +0x10（ORIG 0x086889e3）
            int  m_field4;       // 元素+0x0c（ORIG 未读）
        };
        struct LimitInfoView {
            int m_count;                     // +0x00（ORIG 0x08688a2c 循环条件读）
            int m_pad4;                      // +0x04
            LimitInfoEntryView m_entries[1]; // +0x08 起，步长 0x10
        };
        // [H6 2026-09-11 P183 修正] 视图指针**不具名缓存**：ORIG 每处重读形参
        // `mov 0xc(%ebp),%eax`；具名 `LimitInfoView* lim` 会占 -0x18 槽并每次回读
        // （实测 +16 条、多 push %esi）⇒ 用宏展开，零槽。
#define CUOO_LIM (reinterpret_cast<LimitInfoView*>(info))
        m_limitItemUsage.clear();
        stLimitItemUsageInfoEx* usageInfo = G_CDataManager()->GetLimitItemUsageInfo();
        std::vector<stLimitItemUsageInfo::LimitInfo>::iterator it =
            usageInfo->m_info0.m_list.begin();
        // [H6 2026-09-11 单旋钮·P13/P28 缓存 end] ORIG 0x0868894b 段 end() 仅一次落 -0x20 槽，
        // 循环条件只调 `operator!=`（0x08688ac4 `lea -0x20; lea -0x1c(=%it); call ne`）⇒ 源码
        // 有具名 end 迭代器。旧形每轮 `call end()` 落 -0x1c 临时槽 ⇒ it/end 槽位整体对调。
        std::vector<stLimitItemUsageInfo::LimitInfo>::iterator end =
            usageInfo->m_info0.m_list.end();
        // [FL5 2026-09-10 P28/P167] ORIG 增量段调 `__normal_iterator<...>::ppEi`（后置，
        // 带独立 sret 临时槽；p167 直证：ORIG 有 ppEi、我方旧 `++it` 产 ppEv 差集 1）。
        for (; it != end; it++) {
            LimitItemUsage usage;
            bool matched = false;
            // [H6B 2026-09-11 负结论·已回滚] 试「先声明 int i 后 `i = 0;` 再空 for-init」
            // 欲复现 ORIG 0x08688967/0x0868896e 的连续两条 `movl $0x0,-0xc(%ebp)`
            // （ORIG-only movl:1）：实测 **157→157、ndiff 35→35 中性**（4.4 把两处
            // 同一 zero store 合并为一条）⇒ 回滚；该 movl 属 4.1 冗余 store，不可复现。
            for (int i = 0; i < CUOO_LIM->m_count; ++i) {
                if (CUOO_LIM->m_entries[i].m_itemId == it->m_a) {
                    usage.m_field10 = it->m_kind;
                    usage.m_field11 = it->m_b;  // +0x11
                    usage.m_field15 = it->m_a;
                    usage.m_field19 = CUOO_LIM->m_entries[i].m_field0;
                    reinterpret_cast<CommonTime*>(&usage)
                        ->SetCreateTime(CUOO_LIM->m_entries[i].m_createTime);
                    m_limitItemUsage.push_back(usage);
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                usage.m_field10 = it->m_kind;
                usage.m_field11 = it->m_b;  // +0x11
                usage.m_field15 = it->m_a;
                usage.m_field19 = 0;
                reinterpret_cast<CommonTime*>(&usage)->SetCurTime();
                m_limitItemUsage.push_back(usage);
            }
        }
    }
}