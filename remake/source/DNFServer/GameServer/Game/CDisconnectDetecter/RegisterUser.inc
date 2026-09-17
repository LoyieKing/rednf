// [b28d-lane16 NEAR] ORIG 08472272：p167 CLEAR/跨版本寄存器·槽位残差（N 类），详见 TSV
bool CDisconnectDetecter::RegisterUser(CUser* user)
{
    int mapSize = m_map.size();
    long curSec = GlobalData::s_systemTime_.getCurSec();
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
#if decompile_verify
    std::pair<std::map<DetectedObjectKey, int>::iterator, bool> ret =
        m_map.insert(std::make_pair<DetectedObjectKey, long>(key, curSec));
#else
    // 宿主 make_pair 显式模板实参 + 左值实参会绑定失败 ⇒ 直接构造 pair。
    std::pair<std::map<DetectedObjectKey, int>::iterator, bool> ret =
        m_map.insert(std::pair<DetectedObjectKey, long>(key, curSec));
#endif
    if (ret.second == false)
    {
        // [b28e-N5 · P221 全表达式临时链] 构造即调用。4.4 对「具名 cMyTrace 对象」给深槽
        //   (-0x48)，对「表达式临时」给浅槽 ⇒ 临时链使 trace 归位 ORIG 的 -0x1c，并连带把
        //   insert 返回槽归位 -0x48、ret.second 读取归位 -0x44（ORIG 0x08472317 段与
        //   our .o 逐槽对照实证；探针 /tmp/r5/n5/mgr/CDisconnectDetecter）。
        //   残留：curSec/key/make_pair/pair 四槽与 ORIG 呈镜像（4.4 把表达式临时放浅区、
        //   具名对象放深区；4.1 相反），属 N82附 结构性残差，试过「先声明后赋值」形 ⇒ 多发
        //   2×iterator C1Ev + 2 次拷贝（帧 0x54→0x64）反劣，已弃。
        //   读数（n1_v 口径）：NEAR 67/68，reloc 归一后行级 diff 11→9（净 −2）。
        cMyTrace(__PRETTY_FUNCTION__, 0x25, 0)("RegisterUser() wrong");
        return 0;
    }
    if (mapSize == 0)
    {
        TimerDetectDisconnect::registNextTimer();
    }
    return 1;
}
