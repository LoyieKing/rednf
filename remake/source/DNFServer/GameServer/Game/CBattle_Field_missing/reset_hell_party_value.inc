// ================= CBattle_Field::reset_hell_party_value() — ORIG 0x0830a57c =================
// 从 CDataManager 的 hellparty 脚本值（float 数组）拷贝各组比例；分母 10000.0f。
void CBattle_Field::reset_hell_party_value()
{
    // [round5] 六处固定字段翻正为 disp 直读；三处循环源复现 ORIG 两步寻址。
    // 残余（NEAR）：目标侧 4.1 保留 base disp（lea 0x7c+disp 0x10）vs 4.4 折进索引
    // （lea 0x80+disp 0）⇒ P30/N4 族封顶。
    // [b28d-round5-CBattle_Field_missing 翻正 · 判据②+P30/P311] ORIG 0x0830a57c：
    // ①固定比例字段为**权威 stHellParty_t 具名字段 disp 直读**
    //   （`mov 0x4(%eax),%edx` / `flds 0x10(%eax)` vs 旧稿 add 形，N107 五处翻正）；
    // ②三个循环为「基址 + 常数索引」两步形（源 `add $0x4,%edx; flds 0x8(%eax,%edx,4)`、
    //   目标 `lea 0x7c(%ebx),%edx; fstps 0x10(%eax,%edx,4)`）⇒ 用 TU 局部数组视角
    //   表达（具名成员直访问只能折单 disp）。
    // ORIG（reset_hell_party_value.md 0x0830a583..0x0830a6f7）P48 no-CSE：
    // 每个字段/数组元素各自 fresh G_CDataManager()->get_hellparty_script_values()，
    // 共 9 对 G_CDataManager/get_hellparty_script_values 调用（6 固定 + 3 循环整组）。
    // 前三个字段为整数 dword 拷贝（script[0]/[+4]/[+0xc] = m_maxBaseValue/m_minBaseValue/
    // m_baseRisingValue），与 m_hellPartyRatioN 同为 int ⇒ 直赋即 mov。
    // [本轮收口] 六处固定字段已翻正为 disp 直读（identical 段）；三处循环源已复现
    // ORIG 形（`add $0x4/$0x8/$0xc,%edx; flds 0x8/0x8/0xc(%eax,%edx,4)`）✓。
    // 残余（NEAR，ndiff=10）：目标侧 4.1 保留 base disp（`lea 0x7c(%ebx),%edx;
    // fstps 0x10(%eax,%edx,4)`）而 4.4 恒把成员偏移折进索引（`lea 0x80(%ebx),%edx;
    // fstps (%eax,%edx,4)`）；具名直访问/视图+常数索引/头声明/块内声明四形实测，
    // 4.4 恒折 ⇒ P30/N4 族（4.1 与 4.4 成员常量偏移寻址拆分差异），封顶。

    // —— 固定比例字段 ——
    m_hellPartyRatio0 = static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_maxBaseValue;    // +0x1e8
    m_hellPartyRatio1 = static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_minBaseValue;    // +0x1ec
    m_hellPartyRatio3 = static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_baseRisingValue; // +0x1f4
    m_hellPartyRatio4 = static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_subBaseRates / 10000.0f;     // +0x1f8
    m_hellPartyRatio5 = static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_passSubBaseRates / 10000.0f; // +0x1fc
    m_hellPartyScriptRate = static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_ancientDungeonAddRate / 10000.0f; // +0x234

    // —— m_hellPartyDungeonRate[4]（+0x200）= m_levelDiffRates[i]/10000 ——
    for (int i = 0; i < 4; ++i)
        m_hellPartyDungeonRate[i] = reinterpret_cast<stHellPartyRateSrcView8*>(
            G_CDataManager()->get_hellparty_script_values())->m_rates[i + 4] / 10000.0f;

    // —— m_hellPartyMapRate[5]（+0x210）= m_difficultyRates[i]/10000 ——
    for (int i = 0; i < 5; ++i)
        m_hellPartyMapRate[i] = reinterpret_cast<stHellPartyRateSrcView8*>(
            G_CDataManager()->get_hellparty_script_values())->m_rates[i + 8] / 10000.0f;

    // —— m_hellPartyItemRate[4]（+0x224）= m_partyRates[i]/10000 ——
    for (int i = 0; i < 4; ++i)
        m_hellPartyItemRate[i] = reinterpret_cast<stHellPartyRateSrcViewC*>(
            G_CDataManager()->get_hellparty_script_values())->m_rates[i + 0xc] / 10000.0f;
}