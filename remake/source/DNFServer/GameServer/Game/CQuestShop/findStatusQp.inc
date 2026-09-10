int CQuestShop::findStatusQp(unsigned char status)
{
    // ORIG 0x85eeda8：m_map0 局部 map 拷贝（非 const）+ find/end/operator->；
    // value+0x18 单字节读取（movzbl 实证 = second.m_questPoint +0x14）。
    // 经镜像顶层 stQuestShopStatus map 拷贝（与 ORIG 顶层模板同名）。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0); // +0x6398
    // b28c-L10：ORIG 0x85eeda8 槽序 table=-0x28 / key临时=-0x10。4.4 对「具名 key 局部」
    // 把 key 排到 table 之前（-0x28/-0x24）→ 全帧 key/table 引用差 4。实内联 cast 作实参
    // 时 4.4 复现 ORIG 槽序（探针 /tmp/l10_probe3.cpp fC/fD 实测 table=-0x28、临时=-0x10）。
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(static_cast<int>(status));
    if (it != table.end())                        // ORIG ne 比较 + je 跳过命中块
        return static_cast<int>(it->second.m_questPoint); // movzbl 0x18(%eax)
    return -1;
