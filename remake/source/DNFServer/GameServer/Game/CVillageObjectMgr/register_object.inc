bool CVillageObjectMgr::register_object(Zone zone, IObject* object)
{
    // [b28d-39 判词 · N82/N93] ORIG 0x086b6f58：帧 0x50 vs 0x40 + ORIG 具名局部
    // `add $0x4,%eax; mov %eax,-0xc` 与迭代器槽阶梯（N86/N93）；插入/碰撞/返回
    // 分支逐条等价。
    // ORIG 0x86b6f58：无该 Zone 则新建列表插入，否则先 detect_collision 碰撞
    // 检测（命中返 0），通过才 push_back；成功返 1。
    std::map<Zone, std::list<IObject*> >::iterator it = m_objects.find(zone);
    if (it == m_objects.end())
    {
        std::list<IObject*> newList;
        newList.push_back(object);
        m_objects.insert(std::make_pair(zone, newList));
    }
    else
    {
        if (collision_detection::detect_collision(it->second, object))
            return 0;
        it->second.push_back(object);
    }
    return 1;
