bool GameWorld::InsertUserToIDMap(const char* name, unsigned int id)
{
    // b28d-lane11：P133 拆独立守卫（ORIG 0x086cd7a8 `jne body` + 0x086cd7c3
    // `jbe body2` 两条独立直跳，合并短路形生 setne 汇聚）。
    if (name == NULL)
    {
        return false;
    }
    if (strlen(name) > 0x1e)
    {
        return false;
    }
    std::string strName(name, std::allocator<char>());   // ORIG +0x038 外联 allocator ctor（试复现）
    // b28d-lane11 判词 N82：104=104 助记符全同，残余仅槽组方向——ORIG 0x086cd81a
    // find sret=-0x18 / end sret=-0xc（it 深 last 浅，4.1 声明序）vs 4.4 首次使用序
    // （it=-0x14/last=-0x18）；allocator 临时 -0xd vs -0x9 同族。声明序/两语句序
    // 均不可翻（make_unique_id 同判例）。
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);   // +0x098
    // [b28d-solo-GameWorld] _ZN9GameWorld17InsertUserToIDMapEPKcj verdict=AE
    // （内联 end() 恢复 find 先/end 后调用序 + end sret 浅槽，P220/P293）
    // b28d-solo-GameWorld：ORIG 0x086cd611 end sret 落浅槽 -0xc（匿名临时），
    // 具名 last 会被 4.4 排到深槽 -0x18 ⇒ 内联 end() 保留 find 先/end 后调用序。
    // b28d-lane11：P198② 谓词整体内联（ORIG +0x0c8/+0x0d7 `test %al,%al` 直测
    // 寄存器，具名 bool 局部反多 mov/cmp 落槽两步）。
    if ((it != m_idMap.end()) && (it->second.m_id != 0))
    {
        return true;
    }
    m_idMap[strName].m_id = id;
    return true;
}