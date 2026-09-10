// df_game_r 还原 —— CQueryCounter（D4 批次）
// 参照 docs/class_func_reports/CQueryCounter.md；源文件 QueryCounter.cpp。
// [G5 2026-09-10 · L12 round2 组5 复核] 5 件全核（判据⑩/⑪ + p167 + 归一化 asm 对比）：
//  · **真缺口修复 1**：IncreaseQueryCount —— p167 显示我方多 1×
//    `_ZNKSt17_Rb_tree_iteratorISt4pairIKiN13CQueryCounter10QUERY_INFOEEEptEv`
//    （const operator->）；`(*m_current).second` 形反劣（换出 +3 deEv 且 ORIG 需 ptEv），
//    改 `m_current->second.count++`（后置自增，P38 反向）后 calls 30→29、
//    **CALLSITE CLEAN**（真缺口清零）。
//  · LoadQueryTable/SaveQuery/RegisterQuery 的 7 处具名 cMyTrace 改全表达式临时链（P227）：
//    LoadQueryTable 实测四数不变（中性，保留）。
//  · 残余 N 族：SaveQuery/LoadQueryTable 的 `memset(buf,0,N)` 常量经 %edx 中转
//    （ORIG 0x085ed3a2/0x085ed18a `mov $N,%edx; mov %edx,0x8(%esp)`）—— 本波探针
//    /tmp/probe_memset.cpp f1..f4（普通/sizeof/unsigned/cast）四形恒直写
//    `movl $N,0x8(%esp)` ⇒ N 封顶（P238 实参变体，无源形）；UpdateQueryCounter 的
//    nop/尾岛；RegisterQuery 槽分区（N95/N82，槽集合两侧全等）；IncreaseQueryCount
//    残余槽差 -0x1c↔-0x18（CStreamGuard 对象区）。
#include "CQueryCounter.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "CStreamGuard.h"
#include "GameBase.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- ORIG 全局工具函数（属 DB 工具 TU，0x083fbc66）----
int GetIdentityFromDB(MySQL* mysql);

// ---- ORIG 全局数据：query_table（D 0x093abc80 size 0x554 = 341×4）----
// 341 项指针表，指向 .rodata 中的 DB 查询 SQL 文本；CU=QueryCounter.cpp。
// ORIG 全二进制零指令、零数据引用（二进制扫描唯一命中在 .symtab 自身）⇒ 死数据；
// 本定义按 ORIG .data 指针表 + 目标 .rodata 逐项提取（内容照 ORIG，无推测），
// 仅补符号账（REAL_GAP_SYMBOL）。数组顶层无 const ⇒ external linkage。
const char* query_table[341] = {
	"upDate login_account set login_status=0 where m_channel_no=%d",
	"seLect item_grade, rate_type, money_rate, item_rate, free_rate from item_gen_ref order by item_grade",
	"seLect grade, bottom_grade, money, random_value from money_gen_ref order by grade limit 100",
	"seLect level, exp from monster_reward_ref",
	"seLect limit_pts from pvp_grade_ref where grade=%d",
	"seLect grade, sp from sp_reward",
	"seLect @@identity",
	"inSert into charac_event_items(charac_no,it_id,stack_count,event_code,reg_time) values(%d,%d,%d,%d,now())",
	"seLect id,it_id,stack_count,event_code from charac_event_items where charac_no=%d and delete_flag=0 limit %d",
	"seLect id,it_id,stack_count,event_code from charac_event_items where id=%d and charac_no=%d",
	"seLect avata_check1 from event_ati_info where id=%d",
	"seLect avata_check2 from event_ati_info where id=%d",
	"seLect item_check from event_ati_info where id=%d",
	"seLect cera_check from event_ati_info where id=%d",
	"upDate event_ati_info set avata_check1=unix_timestamp(now()) where id=%d",
	"upDate event_ati_info set avata_check2=unix_timestamp(now()) where id=%d",
	"upDate event_ati_info set item_check=unix_timestamp(now()) where id=%d",
	"upDate event_ati_info set cera_check=unix_timestamp(now()) where id=%d",
	"seLect id,cera,cera_check,item_no,item_server_id,item_charac_no,item_check,avata_no1,avata_no2,avata_server_id,avata_charac_no,avata_check1,avata_check2 from event_ati_info where m_id=%d and occ_check!=0 limit %d",
	"seLect item1_check from event_sigong_info where id=%d",
	"seLect item2_check from event_sigong_info where id=%d",
	"upDate event_sigong_info set item1_check=unix_timestamp(now()) where id=%d",
	"upDate event_sigong_info set item2_check=unix_timestamp(now()) where id=%d",
	"seLect id,item1_no,item1_server_id,item1_charac_no,item1_check,item2_no,item2_server_id,item2_charac_no,item2_check from event_sigong_info where m_id=%d and occ_check!=0 limit %d",
	"seLect coin_check from event_everland_info where id=%d",
	"upDate event_everland_info set coin_check=unix_timestamp(now()) where id=%d",
	"seLect id,server_id,charac_no,coin,coin_check from event_everland_info where m_id=%d and occ_check!=0 limit %d",
	"upDate user_items set expire_date = adddate(expire_date, interval %d day) where charac_no=%u and slot=%d",
	"inSert into user_items (charac_no,slot,it_id,obtain_from,reg_date,ability_no,ipg_agency_no) values(%u,%d,%u,%d,now(),%d,'%s')",
	"inSert into user_items (charac_no,slot,it_id,expire_date,obtain_from,reg_date,ability_no,ipg_agency_no) values(%u,%d,%u,adddate(now(),interval %d day),%d,now(),%d,'%s')",
	"seLect ui_id from user_items where charac_no=%u and slot=%d and (expire_date='0000-00-00 00:00:00' or expire_date>now())",
	"upDate user_items set slot=%d where ui_id=%d",
	"inSert into tester_point (charac_no, point) values(%u, %d)",
	"seLect count(*) from charac_info where m_id=%d and delete_flag=0",
	"seLect delete_flag from charac_info where charac_name='%s'",
	"inSert into charac_info (m_id,charac_name,job,HP,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,move_speed,attack_speed,cast_speed,hit_recovery,jump,charac_weight,max_fatigue,create_time) values(%u,'%s',%d,%d,%d,%u,%d,%d,%d,%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
	"inSert into charac_stat (charac_no,HP) values(%d,100)",
	"inSert into inventory (charac_no, coin, money) values(%u, %d, %d)",
	"inSert into inventory (charac_no, inventory, money,coin) values(%u, '%s', %d,%d)",
	"inSert into inventory (charac_no, coin) values(%u, %d)",
	"inSert into inventory (charac_no, inventory,coin) values(%u, '%s',%d)",
	"upDate inventory set equipslot='%s' where charac_no = %u",
	"inSert into skill (charac_no, skill_slot) values(%u, '%s')",
	"inSert into pvp_result (charac_no) values(%u)",
	"deLete from game_channel where gc_no=%d",
	"upDate charac_info set delete_time=now(),delete_flag=1 where charac_no=%u",
	"inSert into game_channel (gc_no,gc_now,gc_ip,gc_port,gc_max,gc_game,gc_channel,gc_ch_group,gc_channeltype,gc_up_time) values(%d,0,'%s',%d,%d,%d,'%s',%d,'N',now())",
	"upDate game_channel set gc_now=0,gc_ip='%s',gc_port=%d,gc_max=%d,gc_game=%d,gc_channel='%s',gc_ch_group=%d,gc_channeltype='N',gc_up_time=now() where gc_no=%d",
	"inSert village_ticket values(%u,%d)",
	"seLect dungeon_no,difficulty,style,technic,attacked,rank from charac_best_record where charac_no=%u limit %d",
	"seLect point from pvp_point_base where charac_no=%u",
	"seLect win,lose,pvp_point,pvp_grade,avg_kill_count,avg_buf_count,avg_debuf_count,avg_heal_count,avg_counter_count,avg_back_atk_count,avg_union_hit_count,avg_overkill_count,avg_combo_count,avg_aerial_count,avg_attacked_count,avg_deal_damage,avg_technic,avg_style,avg_hit_penalty,pvp_count,win_point from pvp_result where charac_no=%u",
	"seLect pay_coin,event_coin from member_coin where m_id=%d",
	"seLect money,coin,inventory,equipslot,cargo_capacity,cargo from inventory where charac_no=%u",
	"seLect money,coin,inventory,equipslot,pay_coin,event_coin,cargo_capacity,cargo from inventory where charac_no=%u",
	"seLect slot,it_id,unix_timestamp(expire_date),ability_no from user_items where charac_no=%u and (expire_date='0000-00-00 00:00:00' or expire_date>now()) limit %d",
	"seLect quest_no,trigger from quest where charac_no=%u ",
	"seLect remain_sp,skill_slot,request_sp,sp_garbage from skill where charac_no=%u",
	"seLect count(*) from user_ban where m_id=%d and status=1",
	"seLect count(*) from tester_info where m_id=%d and tester_flag='y'",
	"seLect event_id,pre_type,unix_timestamp(service_start),unix_timestamp(service_end) from member_premium where m_id=%d limit %d",
	"seLect count(*) from dnf_pcroom where left(start_ip, length(start_ip)-instr(reverse(start_ip),'.')) = '%s' and reverse(left(reverse(start_ip),instr(reverse(start_ip),'.')-1))+0 <= %d and reverse(left(reverse(end_ip),instr(reverse(end_ip),'.')-1))+0 >= %d;",
	"seLect m_id, passwd,first_ssn, second_ssn, mid(from_unixtime(reg_date)+0,1,8),user_name,email from member_info where user_id='%s'",
	"seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,member_flag from charac_info where m_id=%u and delete_flag!=1 order by charac_no asc limit %d",
	"seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue from charac_info where m_id=%u and delete_flag!=1 order by charac_no asc limit %d",
	"seLect village,exp,HP,fatigue,premium_fatigue,unix_timestamp(last_play_time) from charac_stat where charac_no=%d",
	"seLect charac_no,charac_name,village,job,lev,exp,grow_type,HP,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, fatigue, max_fatigue, create_time,unix_timestamp(last_play_time),guild_id,guild_right,premium_fatigue,max_premium_fatigue,member_flag from charac_info where m_id=%u and delete_flag!=1 order by charac_no asc limit %d",
	"seLect charac_no,charac_name,village,job,lev,exp,grow_type,HP,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, fatigue, max_fatigue, create_time,unix_timestamp(last_play_time),guild_id,guild_right,premium_fatigue,max_premium_fatigue from charac_info where m_id=%u and delete_flag!=1 order by charac_no asc limit %d",
	"seLect slot,it_id,unix_timestamp(expire_date),ability_no from user_items where charac_no=%u and (expire_date='0000-00-00 00:00:00' or expire_date>now()) and slot>=0 and slot<=%d",
	"seLect equipslot from inventory where charac_no=%u",
	"upDate charac_info set lev=%d,grow_type=%d,maxHP=%d,maxMP=%d,phy_attack=%d,phy_defense=%d,mag_attack=%d,mag_defense=%d,inven_weight=%d,hp_regen=%d,mp_regen=%d,move_speed=%d,attack_speed=%d,cast_speed=%d,hit_recovery=%d,jump=%d,charac_weight=%d,element_resist='%s',spec_property='%s',max_premium_fatigue=%d,guild_right=%d where charac_no = %u",
	"upDate charac_info set lev=%d,grow_type=%d,maxHP=%d,maxMP=%d,phy_attack=%d,phy_defense=%d,mag_attack=%d,mag_defense=%d,inven_weight=%d,hp_regen=%d,mp_regen=%d,move_speed=%d,attack_speed=%d,cast_speed=%d,hit_recovery=%d,jump=%d,charac_weight=%d,element_resist='%s',spec_property='%s' where charac_no = %u",
	"upDate charac_info set max_premium_fatigue=%d,guild_right=%d where charac_no = %u",
	"upDate charac_stat set village=%d,exp=%d,HP=%d,fatigue=%d,premium_fatigue=%d,last_play_time=now() where charac_no = %u",
	"upDate charac_info set village=%d,lev=%d,exp=%u,grow_type=%d,fatigue=%d,HP=%d,maxHP=%d,maxMP=%d,phy_attack=%d,phy_defense=%d,mag_attack=%d,mag_defense=%d,inven_weight=%d,hp_regen=%d,mp_regen=%d,move_speed=%d,attack_speed=%d,cast_speed=%d,hit_recovery=%d,jump=%d,charac_weight=%d,element_resist='%s',spec_property='%s',last_play_time=NOW(),premium_fatigue=%d,max_premium_fatigue=%d,guild_right=%d where charac_no = %u",
	"inSert into member_coin(m_id,pay_coin,event_coin) values(%d,%d,%d)",
	"upDate member_coin set pay_coin=%d,event_coin=%d where m_id=%d",
	"upDate inventory set money=%u,coin=%d,equipslot='%s',inventory='%s',cargo_capacity=%d,cargo='%s' where charac_no=%u",
	"upDate inventory set money=%u,coin=%d where charac_no=%u",
	"upDate inventory set money=%u,equipslot='%s',inventory='%s',cargo_capacity=%d,cargo='%s' where charac_no=%u",
	"upDate inventory set money=%u,coin=%d,pay_coin=%d,event_coin=%d,equipslot='%s',inventory='%s',cargo_capacity=%d,cargo='%s' where charac_no=%u",
	"upDate inventory set money=%u,coin=%d,pay_coin=%d,event_coin=%d where charac_no=%u",
	"upDate pvp_result set win=%d,lose=%d,pvp_point=%d,pvp_grade=%d,avg_kill_count=%d,avg_buf_count=%d,avg_debuf_count=%d,avg_heal_count=%d,avg_counter_count=%d,avg_back_atk_count=%d,avg_union_hit_count=%d,avg_overkill_count=%d,avg_combo_count=%d,avg_aerial_count=%d,avg_attacked_count=%d,avg_deal_damage=%d,avg_technic=%d,avg_style=%d,avg_hit_penalty=%d,pvp_count=%d,win_point=%d where charac_no=%u",
	"inSert into charac_best_record (charac_no,dungeon_no,difficulty,style,technic,attacked,rank) values(%d,%d,%d,%d,%d,%d,%d)",
	"upDate charac_best_record set style=%d, technic=%d ,attacked=%d, rank=%d where charac_no=%d and dungeon_no=%d and difficulty=%d",
	"upDate skill set remain_sp=%u,skill_slot='%s',request_sp='%s',sp_garbage=%d where charac_no=%u",
	"upDate game_channel set gc_now=%d,gc_up_time=now() where gc_no=%d",
	"upDate charac_info set grow_type=%d where charac_no=%u",
	"inSert into login_account (m_id,m_channel_no,login_status,last_login_date) values(%u,%d,1,now())",
	"upDate login_account set m_channel_no=%d,login_status=1,last_login_date=now() where m_id=%u",
	"upDate charac_stat set fatigue=%d where charac_no=%u",
	"upDate charac_info set fatigue=%d where charac_no=%u",
	"upDate login_account set m_channel_no=%d,login_status=%d,last_login_date=now() where m_id=%u",
	"inSert into quest (charac_no,quest_no,trigger) values(%u,%d,%d)",
	"upDate quest set trigger=%d where charac_no=%d and quest_no=%d",
	"deLete from quest where charac_no=%u and quest_no=%d",
	"upDate quest set trigger=%d where charac_no=%u and quest_no=%d",
	"seLect village from village_ticket where charac_no=%u",
	"seLect dungeon_no,dungeon_lev from dungeon_clear where charac_no=%u",
	"upDate tester_point set point = point + %d where charac_no = %u",
	"inSert into dungeon_clear (charac_no,dungeon_no,dungeon_lev) values (%u,%d,%d)",
	"upDate dungeon_clear set dungeon_lev=%d where charac_no=%d and dungeon_no=%d",
	"seLect charac_no, lev, job, grow_type from charac_info where charac_name = '%s' and delete_flag=0",
	"inSert into charac_friends values (%d, %d)",
	"deLete from charac_friends where charac_no = %d and friend_no = %d",
	"seLect b.charac_no, b.charac_name, b.lev, b.job, b.grow_type from charac_friends a, charac_info b where b.charac_no = a.friend_no and a.charac_no = %d and b.delete_flag=0 limit %d",
	"inSert into user_cera_ticket(m_id,ticket_id,ticketKey,expire_time) values(%d,'%s',%d,adddate(now(),interval %d hour))",
	"upDate user_cera_ticket set ticket_id='%s',ticketKey=%d,expire_time=adddate(now(),interval %d hour) where m_id=%d",
	"inSert into member_coin(m_id,pay_coin,event_coin) valuse(%d,0,%d)",
	"upDate member_coin set event_coin=event_coin+%u where m_id=%d",
	"seLect coin_check from event_yegam_info where id=%d",
	"seLect item_check from event_yegam_info where id=%d",
	"upDate event_yegam_info set coin_check=unix_timestamp(now()) where id=%d",
	"upDate event_yegam_info set item_check=unix_timestamp(now()) where id=%d and item!=0",
	"seLect id from charac_event_items where charac_no=%d and it_id=26015",
	"inSert into charac_event_items(charac_no,it_id,stack_count,event_code,reg_time,delete_time,delete_flag) values(%d,%d,%d,%d,now(),now(),1)",
	"upDate charac_event_items set stack_count=stack_count+1 where id=%d",
	"upDate charac_event_items set delete_flag=1,delete_time=now() where id=%u",
	"upDate client_down set occ_count = occ_count+%d where occ_date = now()",
	"inSert into crash_down_by_dungeon(occ_date,dungeon_index,down_count) values(now(),%d,%d)",
	"upDate crash_down_by_dungeon set down_count = down_count + %d where occ_date = now() and dungeon_index = %d",
	"inSert into crash_down_by_packet(occ_date,packet_index,down_count) values(now(),%d,%d)",
	"upDate crash_down_by_packet set down_count = down_count + %d where occ_date = now() and packet_index = %d",
	"inSert into crash_down_by_playstatus(occ_date,play_status,down_count) values(now(),%d,%d)",
	"upDate crash_down_by_playstatus set down_count = down_count + %d where occ_date = now() and play_status = %d",
	"seLect event_id from dnf_event_entry where m_id=%d",
	"inSert into dnf_event_entry(event_id,m_id,occ_date,server_id,charac_no,obtain_date) values(%d,%d,from_unixtime(%d),%d,%d,from_unixtime(%d))",
	"deLete from dnf_event_entry where m_id=%d and event_id=%d",
	"inSert into member_premium(pre_type,m_id,service_start,service_end,event_id) values(%d,%d,from_unixtime(%d),from_unixtime(%d),%d)",
	"upDate member_premium set service_start=from_unixtime(%d),service_end=from_unixtime(%d) where pre_type=%d and m_id=%d",
	"upDate guild_member set grow_type=%d,lev=%d where guild_id=%d and charac_no=%d",
	"inSert into dungeon_fail(occ_date,lev,occ_count) values(now(),%d,%d)",
	"upDate dungeon_fail set occ_count = occ_count + %d where occ_date = now() and lev = %d",
	"inSert into re_stamina_shusia(occ_date,lev,occ_count) values(now(),%d,%d)",
	"upDate re_stamina_shusia set occ_count = occ_count + %d where occ_date = now() and lev = %d",
	"inSert into pvp_status(occ_date,room_number,play_type,no_participants,occ_count) values(now(),%d,%d,%d,%d)",
	"upDate pvp_status set occ_count = occ_count + %d , no_participants = no_participants + %d where occ_date = now() and room_number = %d and play_type = %d",
	"inSert into concurrent_user_status(occ_time,player_status,dungeon_index,channel_no,occ_count) values(now(),%d,%d,%d,%d)",
	"inSert into channel_lev_status(occ_time,channel_no,lev,occ_count) values(from_unixtime(%d),%d,%d,%d)",
	"inSert into store(charac_no, store) values(%d, '%s')",
	"upDate store set store = '%s' where charac_no =%d",
	"seLect store from store where charac_no=%d",
	"seLect item_grade, top, bottom, weight from item_select_std order by item_grade",
	"inSert into log_game_channel(gc_up_time,gc_game,gc_no,gc_channel,gc_ch_group,gc_ip,gc_now,gc_new,gc_out) values(now(),%d,%d,'%s',%d,'%s',%d,%d,%d)",
	"seLect dungeon_index, party_level, clear_count, average from party_rank_avg",
	"seLect dungeon_index, level, job, clear_count, average from single_rank_avg",
	"seLect event_type, parameter from dnf_event_log where end_time = 0 and server_id = %d order by start_time",
	"seLect charac_no,lev,delete_flag from charac_info where charac_name='%s'",
	"upDate postal set delete_flag=%d where receive_charac_no=%d and occ_time<subdate(now(),interval %d day) and delete_flag=0",
	"seLect postal_id,unix_timestamp(occ_time),send_charac_no,item_id,add_info,endurance,upgrade,gold,send_charac_name from postal where receive_charac_no=%d and delete_flag=0",
	"seLect postal_id,unix_timestamp(occ_time),send_charac_no,item_id,add_info,endurance,upgrade,gold,send_charac_name from postal where receive_charac_no=%d and postal_id>%d and delete_flag=0",
	"upDate postal set delete_flag=%d,receive_time=now() where postal_id=%d",
	"upDate postal set delete_flag=%d,receive_time=now() where postal_id %s",
	"inSert into postal(occ_time,send_charac_no,receive_charac_no,item_id,add_info,endurance,upgrade,gold,send_charac_name) values(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,'%s')",
	"seLect event_id,unix_timestamp(occ_date),server_id,charac_no,unix_timestamp(obtain_date) from dnf_event_entry where m_id=%d",
	"seLect pre_type,unix_timestamp(service_start),unix_timestamp(service_end) from member_premium where m_id=%d and event_id=%d and service_end>now() order by service_end desc limit 1",
	"seLect pre_type,unix_timestamp(service_start),unix_timestamp(service_end) from member_premium where m_id=%d and event_id=%d and (service_start<=now() and service_end>now()) order by service_end desc limit 1",
	"seLect charac_no,village,exp,HP,fatigue,premium_fatigue,unix_timestamp(last_play_time) from charac_stat where charac_no %s order by charac_no asc",
	"seLect charac_no,slot,it_id,unix_timestamp(expire_date),ability_no from user_items where charac_no %s and (expire_date='0000-00-00 00:00:00' or expire_date>now()) and slot>=0 and slot<=%d order by charac_no asc",
	"seLect charac_no,equipslot from inventory where charac_no %s",
	"seLect black_point,offset_point from charac_black_info where charac_no %s",
	"seLect m_id, level from gm_manifest",
	"seLect level from gm_manifest where m_id = %d",
	"seLect item_check from event_muz_entry where id=%d",
	"upDate event_muz_entry set item_check=unix_timestamp(now()) where id=%d",
	"seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt from event_muz_entry where m_id=%d and occ_check!=0 limit %d",
	"seLect item_check from event_alttle_info where m_id=%d",
	"upDate event_alttle_info set item_check=unix_timestamp(now()) where m_id=%d",
	"seLect item_server_id,item_charac_no,item_no,item_check,item_cnt from event_alttle_info where m_id=%d and occ_check!=0 limit %d",
	"seLect item_check from event_combo_entry where id=%d",
	"upDate event_combo_entry set item_check=unix_timestamp(now()) where id=%d",
	"seLect id,item_server_id,item_charac_no,item_no,item_check,item_cnt from event_combo_entry where m_id=%d and occ_check!=0 limit %d",
	"deLete from user_items where charac_no=%u and slot=%d",
	"inSert into user_items (charac_no,slot,it_id,expire_date,obtain_from,reg_date,ability_no,ipg_agency_no) values(%u,%d,%u,from_unixtime(%d),%d,now(),%d,'%s')",
	"upDate user_items set charac_no=%u,slot=%d where ui_id=%d && charac_no=%u",
	"upDate user_items set stat=%d,slot=%d where ui_id=%d and charac_no=%d",
	"seLect slot,ui_id,it_id,unix_timestamp(reg_date),unix_timestamp(expire_date),ability_no,ipg_agency_no from user_items where charac_no=%u and ui_id=%d",
	"inSert into charac_property(charac_no,element_resist,spec_property) values(%d,'%s','%s')",
	"seLect delete_flag from %s.charac_info where charac_name='%s'",
	"inSert into charac_info (m_id,charac_name,job,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,move_speed,attack_speed,cast_speed,hit_recovery,jump,charac_weight,max_fatigue,create_time) values(%u,'%s',%d,%d,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
	"inSert into charac_info (m_id,charac_name,job,sex,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,move_speed,attack_speed,cast_speed,hit_recovery,jump,charac_weight,max_fatigue,create_time) values(%u,'%s',%d,%d,%d,%u,%d,%d,%d,%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
	"inSert into charac_stat (charac_no,HP, forbidden_to_play, forbidden_due_to) values(%d,100,0,0)",
	"upDate charac_info set delete_time=now(),delete_flag=1 where charac_no=%u and m_id=%d",
	"upDate charac_view set info='%s' where m_id=%u",
	"inSert into charac_view(m_id,info) values(%d,'%s')",
	"seLect password(info) from charac_view where m_id=%u",
	"upDate charac_view set hash_key='%s' where m_id=%u",
	"seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,member_flag,m_id from charac_info where charac_no=%d and delete_flag!=1",
	"seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,member_flag from charac_info where charac_no=%d and delete_flag!=1",
	"seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,member_flag,element_resist,spec_property,m_id from charac_info where charac_no=%d and delete_flag!=1",
	"seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,member_flag,element_resist,spec_property from charac_info where charac_no=%d and delete_flag!=1",
	"seLect village,exp,HP,fatigue,premium_fatigue,unix_timestamp(last_play_time),used_fatigue from charac_stat where charac_no=%d",
	"seLect element_resist,spec_property from charac_property where charac_no=%d",
	"seLect money,coin,inventory,equipslot,cargo_capacity,cargo,creature,creature_flag from inventory where charac_no=%u",
	"seLect money,coin,inventory,equipslot,cargo_capacity,cargo,creature from inventory where charac_no=%u",
	"seLect money,coin,inventory,equipslot,pay_coin,event_coin,cargo_capacity,cargo,creature,creature_flag from inventory where charac_no=%u",
	"seLect money,coin,inventory,equipslot,pay_coin,event_coin,cargo_capacity,cargo,creature from inventory where charac_no=%u",
	"seLect slot,it_id,unix_timestamp(expire_date),ability_no,ipg_agency_no,unix_timestamp(reg_date),ui_id from user_items where charac_no=%u and (expire_date='0000-00-00 00:00:00' or expire_date>now()) and stat=0 limit %d",
	"seLect slot,it_id,name,stomach,exp,endurance,creature_type,ui_id,no_charge,stat from creature_items where charac_no=%u and stat = 0",
	"seLect apply_flag from member_reliable where m_id=%d",
	"seLect unix_timestamp(occ_date) from %s.active_user where m_id=%d",
	"seLect count(*) from %s.charac_info where m_id=%d and delete_flag=0",
	"deLete from charac_view where m_id=%d",
	"deLete from pvp_result where charac_no %s",
	"deLete from charac_property where charac_no %s",
	"deLete from charac_event_items where charac_no %s",
	"deLete from charac_best_record where charac_no %s",
	"deLete from store where charac_no %s",
	"deLete from dungeon_clear where charac_no %s",
	"deLete from village_ticket where charac_no %s",
	"deLete from quest where charac_no %s",
	"deLete from skill where charac_no %s",
	"deLete from inventory where charac_no %s",
	"deLete from charac_stat where charac_no %s",
	"deLete from charac_info where charac_no %s",
	"deLete from active_user where m_id=%d",
	"inSert into %s.active_user(m_id) values(%d)",
	"upDate %s.active_user set occ_date='' where m_id=%d",
	"upDate %s.active_user set occ_date=now() where m_id=%d",
	"seLect charac_no from %s.charac_info where m_id=%u and delete_flag=0 order by charac_no asc limit %d",
	"inSert into %s.charac_view (seLect * from %s.charac_view where m_id=%u)",
	"inSert into %s.charac_info (seLect * from %s.charac_info where charac_no %s)",
	"inSert into %s.charac_stat (seLect * from %s.charac_stat where charac_no %s)",
	"seLect * from %s.inventory where charac_no %s)",
	"inSert into %s.skill (seLect * from %s.skill where charac_no %s)",
	"inSert into %s.quest (seLect * from %s.quest where charac_no %s)",
	"inSert into %s.village_ticket (seLect * from %s.village_ticket where charac_no %s)",
	"inSert into %s.dungeon_clear (seLect * from %s.dungeon_clear where charac_no %s)",
	"inSert into %s.store (seLect * from %s.store where charac_no %s)",
	"inSert into %s.charac_best_record (seLect * from %s.charac_best_record where charac_no %s)",
	"inSert into %s.charac_event_items (seLect * from %s.charac_event_items where charac_no %s)",
	"inSert into %s.charac_property (seLect * from %s.charac_property where charac_no %s)",
	"inSert into %s.pvp_result (seLect * from %s.pvp_result where charac_no %s)",
	"seLect occ_time,period,now_flag from member_punish_hack where m_id=%d",
	"seLect count(*) from dnf_pcroom where ip='%s' and start_ip <= %d and end_ip >= %d",
	"seLect m_id, passwd,first_ssn, second_ssn, mid(from_unixtime(reg_date)+0,1,8),user_name,email,hangame_flag,password('%s') from member_info where user_id='%s' and hangame_flag=%d",
	"seLect m_id, passwd,first_ssn, second_ssn, mid(from_unixtime(reg_date)+0,1,8),user_name,email,hangame_flag from member_info where user_id='%s' and hangame_flag=%d",
	"seLect m_id, passwd,first_ssn, second_ssn, mid(from_unixtime(reg_date)+0,1,8),user_name,email, slot from member_info where user_id='%s'",
	"seLect info,hash_key,password(info) from charac_view where m_id=%d",
	"seLect info from charac_view where m_id=%d",
	"seLect charac_no,element_resist,spec_property from charac_property where charac_no %s order by charac_no asc",
	"seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight, max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,member_flag from charac_info where m_id=%u and delete_flag!=1 order by charac_no asc limit %d",
	"select charac_no,village,exp,HP,fatigue,premium_fatigue,unix_timestamp(last_play_time), forbidden_to_play, unix_timestamp(forbidden_due_to)-unix_timestamp(now()) from charac_stat where charac_no %s order by charac_no asc",
	"seLect charac_no,slot,it_id,unix_timestamp(expire_date),ability_no from user_items where charac_no %s and (expire_date='0000-00-00 00:00:00' or expire_date>now()) and slot>=0 and slot<=%d and stat=0 order by charac_no asc",
	"upDate charac_property set element_resist='%s',spec_property='%s' where charac_no = %u",
	"upDate charac_info set lev=%d,grow_type=%d,maxHP=%d,maxMP=%d,phy_attack=%d,phy_defense=%d,mag_attack=%d,mag_defense=%d,inven_weight=%d,hp_regen=%d,mp_regen=%d,move_speed=%d,attack_speed=%d,cast_speed=%d,hit_recovery=%d,jump=%d,charac_weight=%d,max_premium_fatigue=%d,guild_right=%d where charac_no = %u",
	"upDate charac_info set lev=%d,grow_type=%d,maxHP=%d,maxMP=%d,phy_attack=%d,phy_defense=%d,mag_attack=%d,mag_defense=%d,inven_weight=%d,hp_regen=%d,mp_regen=%d,move_speed=%d,attack_speed=%d,cast_speed=%d,hit_recovery=%d,jump=%d,charac_weight=%d where charac_no = %u",
	"upDate charac_stat set village=%d,exp=%d,HP=%d,fatigue=%d,premium_fatigue=%d,last_play_time=from_unixtime(%d),used_fatigue=%d where charac_no = %u",
	"upDate charac_stat set village=%d,exp=%d,HP=%d,fatigue=%d,premium_fatigue=%d,last_play_time=from_unixtime(%d) where charac_no = %u",
	"upDate inventory set money=%u,coin=%d,equipslot='%s',inventory='%s',cargo_capacity=%d,cargo='%s',creature='%s' where charac_no=%u",
	"upDate inventory set money=%u,equipslot='%s',inventory='%s',cargo_capacity=%d,cargo='%s',creature='%s' where charac_no=%u",
	"upDate inventory set money=%u,coin=%d,pay_coin=%d,event_coin=%d,equipslot='%s',inventory='%s',cargo_capacity=%d,cargo='%s',creature='%s' where charac_no=%u",
	"seLect equipslot,inventory,cargo from inventory where charac_no=%u",
	"upDate skill set remain_sp=%u,skill_slot='%s',request_sp='%s' where charac_no=%u",
	"upDate login_account set m_channel_no=%d,login_status=1,last_login_date=now(),login_ip='%s' where m_id=%u",
	"inSert into login_account (m_id,m_channel_no,login_status,last_login_date,login_ip) values(%u,%d,1,now(),'%s')",
	"seLect charac_no, lev, job, grow_type, sex from charac_info where charac_name = '%s' and delete_flag = 0",
	"seLect b.charac_no, b.charac_name, b.lev, b.job, b.grow_type, b.sex from charac_friends a, charac_info b where b.charac_no = a.friend_no and a.charac_no = %d and b.delete_flag=0 limit %d",
	"seLect id from charac_event_items where charac_no=%d and it_id=%d",
	"inSert into dungeon_lev_log(occ_date, dungeon_id, sum_lev, used_coin, try_cnt, clear_cnt) values(now(), %d, %d, %d, %d, %d)",
	"upDate member_premium set service_end=from_unixtime(%d) where event_id=%d and pre_type=%d and m_id=%d and service_end>from_unixtime(%d)",
	"upDate dnf_master_charac set job=%d,lev=%d where m_id=%d and charac_no=%d and server_id=%d",
	"inSert into concurrent_user_status(occ_time,player_status,dungeon_index,channel_no,occ_count) values(from_unixtime(%d),%d,%d,%d,%d)",
	"inSert into log_query_stat(occ_time,q_id,total,response_time,gc_no) values(from_unixtime(%d),%d,%d,%d,%d)",
	"inSert into log_nat_daily(occ_time,nat_type,nat_type_other,success_cnt,fail_cnt) values(from_unixtime(%d),%d,%d,%d,%d)",
	"inSert into log_nat_stat(occ_time,nat_type,nat_cnt) values(from_unixtime(%d),%d,%d)",
	"upDate postal set delete_flag=%d where receive_charac_no=%d and unlimit_flag=0 and occ_time<subdate(now(),interval %d minute)",
	"upDate postal set delete_flag=%d where receive_charac_no=%d and unlimit_flag=0 and occ_time<subdate(now(),interval %d day) and delete_flag=0",
	"seLect postal_id,unix_timestamp(occ_time),send_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name,unlimit_flag,avata_flag,creature_flag from postal where receive_charac_no=%d and delete_flag=0",
	"seLect postal_id,unix_timestamp(occ_time),send_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name,unlimit_flag,avata_flag,creature_flag from postal where receive_charac_no=%d and postal_id>%d and delete_flag=0",
	"seLect postal_id,unix_timestamp(occ_time),send_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name,unlimit_flag,avata_flag from postal where receive_charac_no=%d and delete_flag=0",
	"seLect postal_id,unix_timestamp(occ_time),send_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name,unlimit_flag,avata_flag from postal where receive_charac_no=%d and postal_id>%d and delete_flag=0",
	"seLect ui_id,unix_timestamp(reg_date),unix_timestamp(expire_date),ipg_agency_no from user_items where charac_no=%d and ui_id %s",
	"inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name,creature_flag) values(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d)",
	"inSert into postal(occ_time,send_charac_no,receive_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name) values(from_unixtime(%d),%d,%d,%d,%d,%d,%d,%d,%d,'%s')",
	"inSert into creature_items(charac_no,slot,it_id,reg_date,name,stomach,exp,endurance,no_charge,stat) values(%d,%d,%d,NOW(),'%s',%d,%d,%d,0,0)",
	"upDate inventory set creature_flag=%d where charac_no=%d",
	"upDate creature_items set exp=%d,stomach=%d where ui_id=%d",
	"upDate creature_items set slot=%d where ui_id=%d",
	"upDate creature_items set %s where ui_id=%d",
	"seLect slot,it_id,name,stomach,exp,endurance,creature_type,no_charge from creature_items where ui_id=%d",
	"inSert into dnf_party_result_history_pvp (str_ip,nat_type,m_id,party_result,occ_time,map_id,dungeon_id,peer_nat_type,peer_ip, established_time, IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, relay_tcp_port ) values('%s',%d,%d,%d,now(),%d,%d,%d,'%s',%d, %d, %d, '%s', %d, %d)",
	"inSert into dnf_party_result_history_pvp (str_ip,nat_type,m_id,party_result,occ_time,map_id,dungeon_id, peer_nat_type,peer_ip, established_time ) values('%s',%d,%d,%d,now(),%d,%d,%d,'%s',%d)",
	"inSert into dnf_party_ping_history_pvp(str_ip,m_id,nat_type_1,ping1,mtu_1,nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,nat_type_5,ping5,mtu_5,nat_type_6,ping6,mtu_6,nat_type_7,ping7,mtu_7,nat_type_8,ping8,mtu_8,occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4,ip_5,ip_6,ip_7,ip_8, end_reason) values('%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s',%d)",
	"inSert into dnf_party_ping_history_pvp(str_ip,m_id,nat_type_1,ping1,mtu_1,nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,nat_type_5,ping5,mtu_5,nat_type_6,ping6,mtu_6,nat_type_7,ping7,mtu_7,nat_type_8,ping8,mtu_8,occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4,ip_5,ip_6,ip_7,ip_8) values('%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s')",
	"inSert into dnf_party_result_history (str_ip,nat_type,m_id,party_result,occ_time,map_id,dungeon_id,peer_nat_type,peer_ip,established_time, IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, relay_tcp_port) values('%s',%d,%d,%d,now(),%d,%d,%d,'%s',%d, %d, %d, '%s', %d, %d)",
	"inSert into dnf_party_result_history (str_ip,nat_type,m_id,party_result,occ_time,map_id,dungeon_id,peer_nat_type,peer_ip,established_time) values('%s',%d,%d,%d,now(),%d,%d,%d,'%s',%d)",
	"inSert into dnf_party_ping_history(str_ip,m_id,nat_type_1,ping1,mtu_1,nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4, end_reason) values('%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s','%s',%d)",
	"inSert into dnf_party_ping_history(str_ip,m_id,nat_type_1,ping1,mtu_1,nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4) values('%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s','%s')",
	"upDate world_cup_event_log set cnt=%d,up_time=NOW()  where m_id=%d",
	"inSert into world_cup_event_log (m_id,up_time,cnt) values(%d,NOW(),%d)",
	"seLect cnt from world_cup_event_log where m_id=%d",
	"upDate charac_stat set forbidden_to_play=0, forbidden_due_to= 0 where charac_no = %d",
	"upDate charac_stat set forbidden_to_play=1, forbidden_due_to = date_add(now(), interval %d day) where charac_no = %d",
	"upDate dnf_hack_log set charac_no=%d,occ_date=now(),cnt=cnt+1,etc=%d where m_id=%d and hack_type=%d",
	"inSert into dnf_hack_log(m_id,charac_no,hack_type,occ_date,etc,cnt) values(%d,%d,%d,now(),%d,1)",
	"inSert into dnf_dbq_log(occ_time,channel_no,dbq) values(NOW(),%d,%d)",
	"inSert into login_view_log values(now(),%d,%d,%d,'%s')",
	"inSert into dnf_proxy_log values(now(),%d,'%s','%s',%d)",
	"inSert into dnf_zeromoney_log(to_m_id,to_nat_ip,to_peer_ip,to_nat_type,from_m_id,from_nat_ip,from_peer_ip,from_nat_type,occ_time,injury_money,to_mac_addr,from_mac_addr) values(%d,'%s','%s',%d,%d,'%s','%s',%d,now(),%d,'%s','%s')",
	"inSert into dnf_zeromoney_log(to_m_id,to_nat_ip,to_peer_ip,to_nat_type,from_m_id,from_nat_ip,from_peer_ip,from_nat_type,occ_time,injury_money) values(%d,'%s','%s',%d,%d,'%s','%s',%d,now(),%d)",
	"seLect count(*) from dnf_hack_mac where mac_addr='%s'",
	"seLect mousepass,fail_cnt from member_mousepass where m_id=%d",
	"upDate member_mousepass set fail_cnt=%d where m_id=%d",
	"inSert into member_mousepass(m_id,mousepass,occ_time) values(%d,'%s',now())",
	"upDate member_mousepass set mousepass='%s',occ_time=now() where m_id=%d",
	"deLete from member_mousepass where m_id=%d",
	"insert into member_mousepass_history(select now(),m_id,mousepass,%d from member_mousepass where m_id=%d)",
	"seLect gold from trade_gold_daily where m_id=%d and occ_time > from_unixtime(%d)",
	"upDate trade_gold_daily set gold=%d,occ_time=from_unixtime(%d) where m_id=%d",
	"inSert into trade_gold_daily(m_id,gold,occ_time) values(%d, %d, from_unixtime(%d))",
	"inSert into reliable_member_log(m_id,ip,occ_date) values(%d, '%s', now())",
	"upDate prod_buy_user set user_id = '%s', sex = %d, birthday = '%s', last_buy_time = now() where m_id = %d ",
	"insert into prod_buy_user (m_id, user_id, sex, birthday, first_buy_time, last_buy_time ) values(%d, '%s', %d, '%s', now(), now() )",
	"insert into prod_sale_entry_064 (agency_no, seal_flag, sale_time, price, slot_part_type, slot_part_code, ipg_no, bonus_ipg_no, m_id, user_id, server_id, charac_no, job, prize_flag, new_flag, result_code, result_message) values('%s', %d, now(), %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, 1, %d, '%s')",
	"insert into prod_sale_entry_064 (agency_no, seal_flag, sale_time, price, slot_part_type, slot_part_code, ipg_no, bonus_ipg_no, m_id, user_id, server_id, charac_no, job, prize_flag, new_flag, result_code, result_message) values('%s', %d, now(), %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, 2, %d, '%s')",
	"upDate member_environment set mean_fps = %d max_fps = %d, min_fps =%d, system_memory = %d, graphic_memory = %d, os_type = '%s', occ_time = now() where m_id = %d",
	"insert into member_environment(m_id, mean_fps, max_fps, min_fps, system_memory, graphic_memory, os_type, occ_time) values(%d, %d, %d, %d, %d, %d, '%s', now())",
	"seLect black_point,offset_point from charac_black_info where charac_no=%d",
	"inSert into log_vacation_event_0612(m_id,charac_no,occ_time,occ_cnt) values(%d,%d,now(),1)",
	"upDate log_vacation_event_0612 set occ_time=now(),occ_cnt=occ_cnt+1 where m_id=%d and charac_no=%d",
	"seLect occ_time from member_punish_hack where m_id=%d",
	"inSert into member_punish_hack_history(select * from member_punish_hack where m_id=%d)",
	"upDate member_punish_hack set occ_time=unix_timestamp(now()),period=0,now_flag=1,auto_flag=0 where m_id=%d",
	"inSert into member_punish_hack(m_id,occ_time,period,now_flag,auto_flag) values(%d,unix_timestamp(now()),%d,1,0)",
	"upDate log_dungeon_monster set damage_total=damage_total+%d, damage_count=damage_count+%d, last_time=NOW() where channel_no=%d and dungeon_index=%d and dungeon_diff=%d and monster_index=%d",
	"inSert into log_dungeon_monster(channel_no,dungeon_index,dungeon_diff,monster_index,damage_total,damage_count,last_time) values(%d,%d,%d,%d,%d,%d,NOW())",
	"upDate log_dungeon_skill set skill_cool_time=skill_cool_time+%d,skill_use=skill_use+%d,skill_count=skill_count+%d,last_time=NOW() where channel_no=%d and dungeon_index=%d and dungeon_diff=%d and charac_job=%d and charac_grow=%d and skill_index=%d",
	"inSert into log_dungeon_skill(skill_cool_time,skill_use,skill_count,channel_no,dungeon_index,dungeon_diff,charac_job,charac_grow,skill_index,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
	"upDate log_dungeon_party set clear_time=(clear_time*update_count+%d*%d)/(update_count+%d), die_count=(die_count*update_count+%d*%d)/(update_count+%d), hp_consume=(hp_consume*update_count+%d*%d)/(update_count+%d), mp_consume=(mp_consume*update_count+%d*%d)/(update_count+%d), hit_count=(hit_count*update_count+%d*%d)/(update_count+%d), hit_per_avg_damage=(hit_per_avg_damage*update_count+%d*%d)/(update_count+%d), hp_recovery=(hp_recovery*update_count+%d*%d)/(update_count+%d), mp_recovery=(mp_recovery*update_count+%d*%d)/(update_count+%d), update_count=update_count+%d ,last_time=NOW() where channel_no=%d and dungeon_index=%d and dungeon_diff=%d and party_user_count=%d and success=%d",
	"inSert into log_dungeon_party(clear_time,die_count,hp_consume,mp_consume,hit_count,hit_per_avg_damage,hp_recovery,mp_recovery,update_count,channel_no,dungeon_index,dungeon_diff,success,party_user_count,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
	"upDate log_dungeon_charac set clear_time=(clear_time*update_count+%d*%d)/(update_count+%d), die_count=(die_count*update_count+%d*%d)/(update_count+%d), hp_consume=(hp_consume*update_count+%d*%d)/(update_count+%d), mp_consume=(mp_consume*update_count+%d*%d)/(update_count+%d), hit_count=(hit_count*update_count+%d*%d)/(update_count+%d), hit_per_avg_damage=(hit_per_avg_damage*update_count+%d*%d)/(update_count+%d), hp_recovery=(hp_recovery*update_count+%d*%d)/(update_count+%d), mp_recovery=(mp_recovery*update_count+%d*%d)/(update_count+%d), update_count=update_count+%d ,last_time=NOW() where channel_no=%d and dungeon_index=%d and dungeon_diff=%d and charac_job=%d and charac_grow=%d and success=%d",
	"inSert into log_dungeon_charac(clear_time,die_count,hp_consume,mp_consume,hit_count,hit_per_avg_damage,hp_recovery,mp_recovery,update_count,channel_no,dungeon_index,dungeon_diff,success,charac_job,charac_grow,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
	"upDate log_dungeon_party_job set job_count=job_count+%d, last_time=NOW() where channel_no=%d and dungeon_index=%d and dungeon_diff=%d and charac_job=%d and charac_grow=%d and party_user_count=%d and success =%d",
	"inSert into log_dungeon_party_job(channel_no,dungeon_index,dungeon_diff,charac_job,charac_grow,party_user_count,success,job_count,last_time) values(%d,%d,%d,%d,%d,%d,%d,%d,NOW())",
	"seLect event_type, parameter1, parameter2 from dnf_event_log where end_time = 0 and server_id in (0,%d) order by start_time",
	"upDate member_login set expire_time=unix_timestamp(now())+%d where m_id=%d",
	"seLect login_time,expire_time from member_login where m_id=%d",
	"upDate member_login set login_time=%d where m_id=%d",
	"inSert into member_login(m_id, login_time) values(%d, %d)",
	"",
};

CQueryCounter::CQueryCounter()
{
    m_current = m_queryInfoMap.end();
    m_totalQueryCount = 0;
}

CQueryCounter::~CQueryCounter()
{
}

void CQueryCounter::Reset()
{
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); ++it)
    {
        it->second.count = 0;
        it->second.responseTime = 0.0;
    }
    m_totalQueryCount = 0;
}

// [b28d-62] 修：P220 极性翻转（`it == end` 直落未命中分支）+ m_bLogDB 正极性。
//   ORIG 0x085ecc5e
void CQueryCounter::IncreaseQueryCount(const std::string& query)
{
    if (query.find("log_query_ref", 0) != std::string::npos)
    {
        return;
    }
    if (query.find("from db_connect", 0) != std::string::npos)
    {
        return;
    }
    std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator it =
        m_queryIDMap.find(query);
    // [b28d-62] P220 极性翻转：ORIG 0x085eccfc 调 operator==（`it == end` 为真直落
    //   未命中分支），我方 `!=` 生相反块序；m_bLogDB 正极性（`test %al; je else`，
    //   ORIG 0x085ecd0f 实证）⇒ 未命中分支内先 stream 后 SaveQuery。
    if (it == m_queryIDMap.end())
    {
        if (m_bLogDB)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1d2), true);
            **guard << 0xbe;
            **guard << -1;
            **guard << (int)query.size();
            guard->put_binary((void*)query.c_str(), (int)query.size());
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
        else
        {
            int qid = 0;
            if (SaveQuery(query.c_str(), qid))
            {
                RegisterQuery(query.c_str(), qid, 0);
            }
        }
    }
    else
    {
        m_current = it->second;
        // [G5 2026-09-10 P167] ORIG 0x085ecc5e 调用差集：我方多 1×
        // `_ZNKSt17_Rb_tree_iteratorISt4pairIKiN13CQueryCounter10QUERY_INFOEEEptEv`。
        // 实测 `(*m_current).second` 形反劣（换出 +3 deEv 且 ORIG 需 ptEv×2）⇒ 维持
        // `->` 形；试后置自增（P38 反向：`= x+1` 单语句 SAVE_EXPR 合并成单次 ptEv，
        // 拆/`++` 形可出第二处访问）。
        if (m_current->second.count == 0)
        {
            m_totalQueryCount = m_totalQueryCount + 1;
        }
        m_current->second.count++;
    }
}

// [b28d-62] 修：迭代器增量改后置 `it++`（P28 增量侧：ORIG 调 operator++(int)）。
//   ORIG 0x085ecee6
void CQueryCounter::UpdateQueryCounter()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1f1), true);
    **guard << 0x7d;
    **guard << -1;
    int curTime = time(0);
    **guard << curTime;
    **guard << m_totalQueryCount;
    // [b28d-62] P28 增量侧：ORIG 增量段调 operator++(int)（后置，带独立 sret
    //   临时槽）⇒ 源形必写 `it++`（`++it` 走 ppEv 免临时槽）。
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); it++)
    {
        int qid = it->first;
        int count = it->second.count;
        int response = (int)(it->second.responseTime * 1000.0);
        if (count > 0)
        {
            **guard << qid;
            **guard << count;
            **guard << response;
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    Reset();
}

void CQueryCounter::SetResponseTime(double responseTime)
{
    if (m_current != m_queryInfoMap.end())
    {
        m_current->second.responseTime += responseTime;
        m_current = m_queryInfoMap.end();
    }
}

// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed18a
bool CQueryCounter::LoadQueryTable()
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    int ret = 0;
    char query[0x6001];
    mysql->set_query("seLect q_id,query from log_query_ref");
    if (!mysql->exec(true))
    {
        return false;
    }
    int nRows = mysql->get_n_rows();
    int i = 0;    // P191 ORIG -0xc 双 init 形
    for (i = 0; i < nRows; i = i + 1)
    {
        int qid = 0;
        memset(query, 0, 0x6001);
        if (!mysql->fetch())
        {
            // [G5 2026-09-10 P227 试验] ORIG 0x085ed18a 段为 cMyTrace ctor 后直接
            // operator()（varargs 在 ctor 前右起求值并保活 callee-saved）⇒ 改全表达式
            // 临时链；具名两语句形使 4.4 把 ctor 提前、帧 0x…。
            cMyTrace(__PRETTY_FUNCTION__, 0x22f, 5)("LoadQueryTable, fatch ERROR");
            return false;
        }
        if (!mysql->get_int(0, qid))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x235, 5)("LoadQueryTable, get_int(0) ERROR");
            return false;
        }
        if (!mysql->get_str(1, query, 0x6000))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x23b, 5)("LoadQueryTable, get_str(1) ERROR");
            return false;
        }
        if (!RegisterQuery(query, qid, 0))
        {
            return false;
        }
    }
    return true;
}

// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed3a2
bool CQueryCounter::SaveQuery(const char* query, int& qid)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    char escaped[0xc001];
    memset(escaped, 0, 0xc001);
    mysql->escape_string(escaped, query);
    mysql->set_query("inSert into log_query_ref(query, query_hash) values('%s',password('%s'))",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        qid = GetIdentityFromDB(mysql);
        return true;
    }
    mysql->set_query("seLect q_id from log_query_ref where query_hash=password('%s')",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        if (mysql->get_n_rows() == 0)
        {
            return false;
        }
        if (mysql->fetch())
        {
            if (mysql->get_int(0, qid))
            {
                return true;
            }
            cMyTrace trace(__PRETTY_FUNCTION__, 0x277, 5);
            trace("SaveQuery, get_int(0) ERROR");
            return false;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x271, 5);
        trace("SaveQuery, fetch() ERROR");
        return false;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x265, 5);
    trace("SaveQuery, exec() ERROR");
    return false;
}

// [b28d-62] N95/N82：槽基整体差 0x10（4.4 槽基自 ebp-8 起，N95）；p167 CLEAR。
//   ORIG 0x085ed5ae
bool CQueryCounter::RegisterQuery(const char* query, int qid, int type)
{
    QUERY_INFO info;
    info.count = type;
    std::pair<std::map<int, QUERY_INFO>::iterator, bool> ins =
        m_queryInfoMap.insert(std::make_pair(qid, info));
    if (!ins.second)
    {
        return false;
    }
    std::pair<std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator, bool> ins2 =
        m_queryIDMap.insert(std::make_pair(query, ins.first));
    if (!ins2.second)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x291, 0);
        trace("LoadQueryTable,  query_id_map_.insert ERROR");
        return false;
    }
    return true;
}
