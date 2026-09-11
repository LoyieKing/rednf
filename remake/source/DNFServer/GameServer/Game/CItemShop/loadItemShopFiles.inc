// ---- loadItemShopFiles @ 0x8512aa0 ----
bool CItemShop::loadItemShopFiles()
{
    ItemShopScript script;
    // [b28d-38b] ORIG 0x08512ab9：path 槽清零发射在 ItemShopScript::C1 之后、
    // iterator 默认构造之前 ⇒ 声明序 script→path→it。
    const char* path = 0;
    std::map<int, std::string>::iterator it;
    it = g_itemShopScriptFileList.m_fileMap.begin();
    // [b28d-38] P167：ORIG 调 _Rb_tree_iterator::operator!=（我们原用 ==）⇒ 正极性 while。
    while (it != g_itemShopScriptFileList.m_fileMap.end())
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        if (!importItemShopScript(&script, path)) {
            LogManager::logFormat(1, "item.cpp", __PRETTY_FUNCTION__,
                                  0xc26, "ImportEquipmentScript() fail file - %s", path);
            return false;
        }
        script.m_shopId =
            g_itemShopScriptFileList.FindIndexByFullScan(path);
        if (script.m_isOneADay)
            setOneADayItemListNo(script);
        m_shopMap.insert(std::make_pair(script.m_shopId, script));
        ++it;
    }
    return true;
}