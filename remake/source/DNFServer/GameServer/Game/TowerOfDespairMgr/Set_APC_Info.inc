void TowerOfDespair_APC_Mgr::Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info)
{
    // b28d-49 @0x85fed2e：P167 差集定位 —— ORIG 有 iterator::C1Ev（默认构造）+
    // operator!=（正极性），我方缺 C1Ev 且多发 operator==。⇒ P50 复活
    // 「先声明后赋值」+ P220 正极性 `it != end()`，命中分支（set_data）为 then。
    // 二次复核 @0x85ff1aa：ORIG 求值序 = C1Ev(it) → end()（具名槽 -0x2e804）→
    // find() → it=结果（-0x2e808）⇒ 必须有具名 end 局部（P28 双旋钮之「命名 it
    // 固定 find 先求值」的反向：此处 end 先于 find）。
    // b28d-49 判词：NEAR 封顶（N93 槽方向/求值序绑定）——4.4 把先声明的 it 放浅槽
    // -0x2e804、end 放深槽 -0x2e808（ORIG 相反）；交换声明序可对槽但求值序反
    //（end→C1Ev→find vs ORIG C1Ev→end→find），两旋钮互斥。P167 CLEAR。@0x85fed2e
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;
    std::map<TOD_Layer, SendingAPC_Info>::iterator mapEnd = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != mapEnd)
    {
        it->second.set_data(info);
    }
    else
    {
        SendingAPC_Info value;
        value.set_data(info);
        m_APCInfoMap.insert(std::make_pair(layer, value));
    }
