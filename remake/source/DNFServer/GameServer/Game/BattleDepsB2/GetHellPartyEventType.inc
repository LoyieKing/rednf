// ORIG 0x082ffb06 GetHellPartyEventType(diff)：查 stHellParty_t.m_field8a[diff]。
// b28d-solo-BattleDepsB2 定案（N 封顶 13/14）：残差为 base/index 寄存器放置——ORIG 留 call
//   结果于 %eax、索引 `movsbl -0xc(%ebp),%edx`；4.4 ISel 恒 index→%eax、base 挪
//   %edx（多 1 条 `mov %eax,%edx`）。探针定案：唯一旋钮 = getter 返回类型
//   void*→stHellParty_t*（/tmp/b28d_bdb2/probe3.cpp h1 = 14 条、probe4.cpp a1
//   = 13 条 = ORIG 形；4.4.7 与 4.4.6 后端读数一致，非路由敏感）。该改动涉
//   CDataManager.h + 7 个消费 .cpp 的 static_cast<const float*>，Main 裁决本波
//   不落（收益 1 件 < 跨 TU 风险）⇒ 封顶。
int CBattle_Field::GetHellPartyEventType(char diff)
{
    return (signed char)static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_field8a[(int)diff];
}