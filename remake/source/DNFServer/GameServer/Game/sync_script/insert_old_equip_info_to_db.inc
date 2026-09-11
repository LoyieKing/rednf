bool CSyncScript::insert_old_equip_info_to_db()
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；P167 CLEAR
    //   9=9）。真 bug 修正：ORIG 0x08624d5d 遍历**全局**
    //   g_equipmentStatInfoMap（B 0x095000a0），而非成员 m_oldEquipMap
    //   （全仓无填充点 ⇒ 旧实现恒空、一列不写）。P167 同时给出迭代器形证据：
    //   ORIG 调 operator-> 两次（绑定 second 引用 + 取 first），旧 `(*it).second`
    //   形多 1×operator* 少 2×operator->（MISSING ptEv ×2 / EXTRA deEv ×1）。
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    for (std::map<unsigned int, STEquipmentStatInfo>::iterator it =
             g_equipmentStatInfoMap.begin();
         it != g_equipmentStatInfoMap.end(); ++it)
    {
        // ORIG 0x08624d86：`add $0x4,%eax; mov %eax,-0x1c` ⇒ 源码把 it->second
        // 绑定为具名引用（指针缓存在 -0x1c），随后 33 个字段逐次重读该槽。
        const STEquipmentStatInfo& st = it->second;
        if (!db->set_query(
                "inSert into dnf_old_equip_info(it_id, hp_max, mp_max, phy_att, phy_def, mag_att, mag_def, equip_phy_att, equip_phy_def, equip_mag_att, equip_mag_def, ref_fire, ref_water, ref_dark, ref_light, ref_all_elements, ref_slow, ref_freeze, ref_poison, ref_stun, ref_curse, ref_blind, ref_lightning, ref_stone, ref_sleep, ref_burn, ref_weapon_break, ref_bleeding, ref_pierce, ref_stuck, ref_confuse, ref_hold, ref_armor_break, ref_all_state) values(%d, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)",
                it->first, st.m_hpMax, st.m_mpMax,
                st.m_phyAtt, st.m_phyDef,
                st.m_magAtt, st.m_magDef,
                st.m_equipPhyAtt, st.m_equipPhyDef,
                st.m_equipMagAtt, st.m_equipMagDef,
                st.m_refFire, st.m_refWater,
                st.m_refDark, st.m_refLight,
                st.m_refAll, st.m_refSlow,
                st.m_refFreeze, st.m_refPoison,
                st.m_refStun, st.m_refCurse,
                st.m_refBlind, st.m_refLightning,
                st.m_refStone, st.m_refSleep,
                st.m_refBurn, st.m_refWeaponBreak,
                st.m_refBleeding, st.m_refPierce,
                st.m_refStuck, st.m_refConfuse,
                st.m_refHold, st.m_refArmorBreak,
                st.m_refAllState))
        {
            return false;
        }
        if (!db->exec(true))
        {
            return false;
        }
    }
    return true;
}