// ---- clear @ 0x88fd9c4 ----
// [b28d-35] 翻正（推翻原 N18a 判词）：m_field44 写 ORIG 为 `mov $0x0,%edx;
// mov %edx,0x44(%eax)` 两段（4.1 常数寄存器中转）——按 P238 用 float 视图
// `m_field44f = 0.0f;` 即复现该 edx 中转形（int 视图恒折 `movl $0x0,
// 0x44(%eax)` 单段）。字段经匿名 union 双视图，sizeof/偏移/mangled 不变。
void STCeraShopItem::clear()
{
    m_field0 = (char)0xff;
    m_field4 = 0;
    m_field8 = 0xfffffffe;
    m_fieldc = 0xffffffff;
    m_field10 = 0;
    m_field14 = (ENUM_CERA_SHOP_ITEM_NO)0xffffffff;
    m_field18 = (char)0xff;
    m_field1c = 1;
    m_field20 = 0;
    m_field22 = 0;
    m_field24 = 0;
    m_field2c = 0;
    m_field30 = 0;
    m_field38 = 0;
    m_field34 = 0;
    m_field3c = (char)0xff;
    m_name = "";
    m_field80 = 0;
    m_field44 = 0.0f;
    m_field48 = 0;
    m_field4c = 0;
    m_field50 = 0;
    m_field51 = 0;
    m_field52 = 0;
    m_field53 = 1;
    m_field54 = 0xfffffffe;
    m_field58 = 0xffffffff;
    m_field5c = 0;
    m_field60 = (char)0xc;
    m_field62 = 0;
    m_field64 = 0;
    m_field68 = 0;
    m_field6c = 0;
    m_field70 = 0;
    m_field74 = 0;
    m_field78 = 0;
    m_field28 = 0;
    m_field7c = 0xffffffff;
}
