// [S1Charac 2026-09-11 复核] 100/104：真残差=4.4 把 LHS 地址（index+0x460）CSE 到
// %esi 跨 call 保活（+push/pop %esi），并按「disp/scale≥2 折、==1 不折」定则把 disp
// 0x10 折成 `lea 0x2(%esi),%edx`；ORIG 0x08677f97/0x08677fbc 每处独立
// `lea 0x460(%ebx),%ecx` + disp 0x10 ⇒ N82附/N4 封顶。§6.5 串核查：本函数无 rodata
// 串引用，且本 TU 全量 .rodata 串 24/24 均存在于 ORIG（无发明串）⇒ CLEAR。
// [b28d-solo-CUserCharacInfo] ①字段直赋中转已修（P211 档③）：SigTypes_C.h
// m_cooltimeCharacNo/m_effectCharacNo int→unsigned（与 m_characNo 同型）⇒ ORIG
// 0x08677f34 单段 `mov 0x4(%eax),%edx`，ndiff 54→48。②余差 = N82附（4.4 把
// `index + 0x460` CSE 到 %esi 跨 call 保活，ORIG 每处独立 `lea 0x460(%ebx),%ecx`
// 重算；语句表达式下标/下标交换两形实测不可阻）+ N4/P30（4.4 把 disp 0x10 折成
// 索引 +2：`lea 0x2(%esi),%edx`）⇒ 封顶，语义等价（104 vs 100 条）。
// [b28d-solo-CUserCharacInfo 复核] 再补 5 形（/tmp/b28d_solo/gcii.cpp、gcii2.cpp、
// gcii3.cpp、star.cpp：`*&` 双算、裸语句表达式 RHS、具名 const 引用、int↔unsigned
// cast 异型化、unsigned 下标）均产同一「LHS 地址（index+0x460）前置跨 call 保活」形；
// disp 折叠经探针确认为 4.4 定则（disp/scale≥2 折、==1 不折：gcii.cpp k1 vs k3）
// ⇒ N76/N82附/N4 封顶定论。
// [P8 2026-09-11 第二遍回扫] 新钥匙复核：prett 无缺口/无 size 差、串 CLEAR、
//   p167 CLEAR；n1_v=DIFF 100/104（gap=-4）残差 = 4.4 CSE `index+0x460`→%esi 跨
//   call 保活 + disp 0x10 折成索引 +2（ORIG 4.1 每处独立 lea、disp 不折）
//   ⇒ 维持 N82附/N4 封顶，未改动。
void CUserCharacInfo::GetContinuousItemInfo(SIG_LOAD_ETC* p) const
{
    p->m_contItemCooltime.m_characNo = p->m_characNo;
    p->m_contItemEffect.m_characNo = p->m_characNo;
    const std::vector<ContinuousItemInfo>* cool = GetCurCharacCoolTimeItemListR();
    p->m_contItemCooltime.m_info.m_count = (unsigned char)cool->size();
    int index = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        ((LoadEtcCoolItemWindow*)p)->m_entries[index + 0x460].m_itemIdx =
            (*cool)[i].m_itemIdx;
        ((LoadEtcCoolItemWindow*)p)->m_entries[index + 0x460].m_expireTime =
            (*cool)[i].m_expireTime;
        ++index;
    }
    index = 0;
    const std::vector<ContinuousItemInfo>* effect = GetCurCharacEffectItemListR();
    p->m_contItemEffect.m_info.m_count = (unsigned char)effect->size();
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        ((LoadEtcEffectItemWindow*)p)->m_entries[index + 0x494].m_itemIdx =
            (*effect)[i].m_itemIdx;
        ((LoadEtcEffectItemWindow*)p)->m_entries[index + 0x494].m_expireTime =
            (*effect)[i].m_expireTime;
        ++index;
    }
}