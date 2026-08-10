// Reconstructed from secagent binary (DWARF + 反汇编, 2026-08-10)
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/framework/comm_transaction_manager.cpp
// 语义与 secagent 二进制逐一对照（符号/偏移/字符串/返回码一致）。

#include "src/commlib/framework/comm_predefine.h"
#include "output/commlib/zenlib/release/include/zen_predefine.h"
#include "output/commlib/zenlib/release/include/zen_time_value.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_debug.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_msg.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_base.h"
#include "output/commlib/zenlib/release/include/zen_thread_msgque_template.h"
#include "output/commlib/zenlib/release/include/zen_boost_lord_rings.h"
#include "src/commlib/framework/comm_app_frame.h"
#include "src/commlib/framework/comm_enum_define.h"
#include "src/commlib/framework/comm_service_info.h"
#include "src/commlib/framework/comm_frame_command.h"
#include "src/commlib/framework/comm_error_code.h"
#include "src/commlib/framework/comm_security_check.h"
#include "src/commlib/framework/comm_stat_monitor.h"
#include "src/commlib/framework/comm_stat_define.h"
#include "src/commlib/framework/comm_svrd_config.h"
#include "src/commlib/framework/comm_transaction_base.h"
#include "src/commlib/framework/comm_transaction_manager.h"
#include "src/commlib/framework/comm_frame_malloc.h"
#include "src/commlib/framework/comm_zerg_mmappipe.h"
#include <ext/hash_map>
#include <ext/hash_set>
#include <string.h>
#include <utility>

CREATE_TRANS_RECORD::CREATE_TRANS_RECORD(unsigned int arg0)
    : trans_command_(arg0),
      crttrs_cmd_pool_(),
      if_auto_trans_lock_(false),
      trans_lock_cmd_(0),
      create_trans_num_(0),
      destroy_right_num_(0),
      destroy_timeout_num_(0),
      destroy_exception_num_(0),
      trans_consume_time_(0) {
}

CREATE_TRANS_RECORD::~CREATE_TRANS_RECORD() {
}

TRANS_LOCK_RECORD::TRANS_LOCK_RECORD(unsigned int arg0, unsigned int arg1)
    : lock_qq_uin_(arg0), lock_trans_cmd_(arg1) {
}

TRANS_LOCK_RECORD::TRANS_LOCK_RECORD()
    : lock_qq_uin_(0), lock_trans_cmd_(0) {
}

TRANS_LOCK_RECORD::~TRANS_LOCK_RECORD() {
}

Transaction_Manager::Transaction_Manager()
    : transc_map_(100),
      regtrans_pool_map_(100),
      trans_lock_pool_((size_t)-5),
      max_trans_(0),
      trans_id_builder_(0),
      self_services_id_(0, 0),
      timer_queue_(0),
      zerg_mmap_pipe_(0),
      statistics_clock_(0),
      trans_send_buffer_(0),
      trans_recv_buffer_(0),
      inner_frame_malloc_(0),
      inner_message_queue_(0),
      gen_trans_counter_(0),
      cycle_gentrans_counter_(0) {
}

Transaction_Manager::~Transaction_Manager() {
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] Transaction_Manager::~Transaction_Manager start.");
    size_t transc_num = transc_map_.size();
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] close all transaction, transc_map_ size =%u", transc_num);
    for (size_t i = 0; i < transc_num; ++i) {
        Transaction_Base *txbase = 0;
        if (transc_map_.begin() != transc_map_.end()) {
            txbase = transc_map_.begin()->second;
        }
        txbase->output_trans_info("[Application exit]:");
        txbase->handle_close();
    }
    for (regtrans_pool_map_type::iterator it = regtrans_pool_map_.begin();
         it != regtrans_pool_map_.end(); ++it) {
        CREATE_TRANS_RECORD &reg_ctr_trans = it->second;
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] Register command:%u size of pool:%u capacity of pool:%u.",
            reg_ctr_trans.trans_command_, reg_ctr_trans.crttrs_cmd_pool_.size(),
            reg_ctr_trans.crttrs_cmd_pool_.capacity());
        if (reg_ctr_trans.crttrs_cmd_pool_.size()
            != reg_ctr_trans.crttrs_cmd_pool_.capacity()) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Plase notice!! size[%u] != capacity[%u] may be exist memory leak.",
                reg_ctr_trans.crttrs_cmd_pool_.size(),
                reg_ctr_trans.crttrs_cmd_pool_.capacity());
        }
        while (reg_ctr_trans.crttrs_cmd_pool_.size() != 0) {
            Transaction_Base *recycle_txbase = 0;
            reg_ctr_trans.crttrs_cmd_pool_.pop_front(recycle_txbase);
            if (recycle_txbase != 0) {
                delete recycle_txbase;
            }
        }
    }
    if (inner_frame_malloc_ != 0) {
        delete inner_frame_malloc_;
        inner_frame_malloc_ = 0;
    }
    if (inner_message_queue_ != 0) {
        delete inner_message_queue_;
        inner_message_queue_ = 0;
    }
}

void Transaction_Manager::initialize(size_t szregtrans, size_t sztransmap,
                                     SERVICES_ID selfsvr,
                                     ZEN_Timer_Queue *timer_queue,
                                     Zerg_MMAP_BusPipe *zerg_mmap_pipe,
                                     unsigned int max_frame_len,
                                     bool init_inner_queue,
                                     bool init_lock_pool) {
    regtrans_pool_map_.resize(szregtrans + 0x80);
    transc_map_.resize(sztransmap + 0x400);
    max_trans_ = sztransmap;
    self_services_id_.services_id_ = selfsvr.services_id_;
    self_services_id_.services_type_ = selfsvr.services_type_;
    timer_queue_ = timer_queue;
    zerg_mmap_pipe_ = zerg_mmap_pipe;
    trans_send_buffer_ = new (max_frame_len + 0x20)
        Comm_App_Frame(0, max_frame_len + 0x20, 0x1000000);
    trans_recv_buffer_ = new (max_frame_len + 0x20)
        Comm_App_Frame(0, max_frame_len + 0x20, 0x1000000);
    if (init_inner_queue) {
        inner_frame_malloc_ = new AppFrame_Mallocor_Mgr<ZEN_Null_Mutex>();
        inner_frame_malloc_->initialize(INIT_FRAME_MALLOC_NUMBER, 0x10000);
        inner_message_queue_ =
            new ZEN_Message_Queue_Deque<ZEN_NULL_SYNCH, Comm_App_Frame *>(
                INNER_QUEUE_WATER_MARK);
    }
    if (init_lock_pool) {
        trans_lock_pool_.resize(sztransmap >> 1);
    }
}

int Transaction_Manager::register_trans_cmd(unsigned int cmd,
                                            Transaction_Base *ptxbase,
                                            bool if_auto_lock_trans,
                                            unsigned int lock_trans_cmd) {
    CREATE_TRANS_RECORD ctr_trans_rec(cmd);
    ctr_trans_rec.if_auto_trans_lock_ = if_auto_lock_trans;
    ctr_trans_rec.trans_lock_cmd_ = lock_trans_cmd;
    CREATE_TRANS_RECORD &reg_ctr_trans =
        hash_map_find_or_insert(regtrans_pool_map_, cmd, ctr_trans_rec).second;
    // 初始化命令池：容量 POOL_EXTEND_TRANSACTION_NUM，填入模型克隆
    if (reg_ctr_trans.crttrs_cmd_pool_.value_ptr_ != 0) {
        delete[] reg_ctr_trans.crttrs_cmd_pool_.value_ptr_;
        reg_ctr_trans.crttrs_cmd_pool_.value_ptr_ = 0;
    }
    reg_ctr_trans.crttrs_cmd_pool_.cycdeque_start_ = 0;
    reg_ctr_trans.crttrs_cmd_pool_.cycdeque_size_ = 0;
    reg_ctr_trans.crttrs_cmd_pool_.cycdeque_capacity_ =
        POOL_EXTEND_TRANSACTION_NUM;
    reg_ctr_trans.crttrs_cmd_pool_.value_ptr_ =
        new Transaction_Base *[POOL_EXTEND_TRANSACTION_NUM];
    for (size_t i = 0; i < POOL_EXTEND_TRANSACTION_NUM; ++i) {
        Transaction_Base *cloned_txbase =
            ptxbase->create_self(timer_queue_, this);
        reg_ctr_trans.crttrs_cmd_pool_.push_back(cloned_txbase);
    }
    delete ptxbase;
    return 0;
}

int Transaction_Manager::regiester_trans_id(unsigned int transid,
                                            unsigned int trans_cmd,
                                            Transaction_Base *ptxbase) {
    if (statistics_clock_ != 0) {
        ptxbase->trans_create_time_ = statistics_clock_->sec();
        CREATE_TRANS_RECORD ctr_trans_rec(0);
        CREATE_TRANS_RECORD &reg_ctr_trans =
            hash_map_find_or_insert(regtrans_pool_map_, trans_cmd,
                                    ctr_trans_rec).second;
        reg_ctr_trans.create_trans_num_++;
    }
    hash_map_find_or_insert(transc_map_, transid, (Transaction_Base *)0)
        .second = ptxbase;
    return 0;
}

int Transaction_Manager::unregiester_trans_id(unsigned int transid,
                                              unsigned int trans_cmd,
                                              int run_state,
                                              time_t trans_start) {
    Transaction_Base *rt_tsbase = 0;
    transc_map_type::iterator mapiter = transc_map_.find(transid);
    if (mapiter == transc_map_.end()) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] unregiester_trans_id,Can't Find Transaction ID:%u From MAP.",
            transid);
        return 0x139e;
    }
    rt_tsbase = mapiter->second;
    if (statistics_clock_ != 0) {
        CREATE_TRANS_RECORD ctr_trans_rec(0);
        CREATE_TRANS_RECORD &reg_ctr_trans =
            hash_map_find_or_insert(regtrans_pool_map_, trans_cmd,
                                    ctr_trans_rec).second;
        if (run_state >= 1 && run_state <= 2) {
            reg_ctr_trans.destroy_right_num_++;
        } else if (run_state == 3) {
            reg_ctr_trans.destroy_timeout_num_++;
        } else {
            reg_ctr_trans.destroy_exception_num_++;
        }
        reg_ctr_trans.trans_consume_time_ +=
            statistics_clock_->sec() - trans_start;
    }
    transc_map_.erase(transid);
    return_clone_to_pool(trans_cmd, rt_tsbase);
    return 0;
}

int Transaction_Manager::process_pipe_frame(size_t &proc_frame,
                                            size_t &create_trans) {
    create_trans = 0;
    proc_frame = 0;
    Comm_App_Frame *recv_frame = trans_recv_buffer_;
    while (!zerg_mmap_pipe_->bus_pipe_pointer_[0]->empty()
           && proc_frame <= MAX_ONCE_PROCESS_FRAME - 1) {
        if (zerg_mmap_pipe_->bus_pipe_pointer_[0]->empty()) {
            break;
        }
        zerg_mmap_pipe_->bus_pipe_pointer_[0]->pop_front(
            (ZEN_LIB::dequechunk_node *)recv_frame);
        Comm_App_Frame *tmp_frame = recv_frame;
        zerg_mmap_pipe_->monitor_->increase_by_statid(
            0x238d, tmp_frame->app_id_, tmp_frame->frame_command_, 1);
        zerg_mmap_pipe_->monitor_->increase_by_statid(
            0x239b, tmp_frame->app_id_, tmp_frame->frame_command_,
            tmp_frame->frame_length_);
        if (zerg_mmap_pipe_->config_->is_monitor_uin(tmp_frame->frame_uin_)) {
            tmp_frame->frame_option_ |= 0x10000;
        }
        if (tmp_frame->frame_option_ & 0x10000) {
            Comm_App_Frame::dumpoutput_frameInfo(
                tmp_frame, "[TRACK MONITOR][Recv]", RS_ERROR);
        }
        bool bcrtcx = false;
        int ret = process_appframe(tmp_frame, bcrtcx);
        if (ret == 0 && bcrtcx) {
            create_trans++;
        }
        proc_frame++;
    }
    return 0;
}

int Transaction_Manager::push_back_sendpipe(Comm_App_Frame *proc_frame) {
    if (proc_frame->frame_length_ - Comm_App_Frame::LEN_OF_APPFRAME_HEAD > 0xffce) {
        ZEN_Trace_LogMsg::debug_errorex(
            "[framework] Frame Len is error ,frame length :%u ,Please check your code.",
            proc_frame->frame_length_);
        return 0x13b1;
    }
    if (zerg_mmap_pipe_->config_->is_monitor_uin(proc_frame->frame_uin_)) {
        proc_frame->frame_option_ |= 0x10000;
    }
    if (proc_frame->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_frameInfo(proc_frame,
                                             "[TRACK MONITOR][Send]",
                                             RS_ERROR);
    }
    if (!zerg_mmap_pipe_->bus_pipe_pointer_[1]->push_end(
            (const ZEN_LIB::dequechunk_node *)proc_frame)) {
        ZEN_Trace_LogMsg::debug_alertex(
            "[zenlib] %u Pipe is full or data small?,Some data can't put to pipe. Please increase and check. nodesize=%u, freesize=%u",
            1, proc_frame->frame_length_,
            zerg_mmap_pipe_->bus_pipe_pointer_[1]->freesize());
        zerg_mmap_pipe_->monitor_->increase_by_statid(
            0x238f, proc_frame->app_id_, proc_frame->frame_command_, 1);
        return 0x1396;
    }
    zerg_mmap_pipe_->monitor_->increase_by_statid(
        0x238e, proc_frame->app_id_, proc_frame->frame_command_, 1);
    zerg_mmap_pipe_->monitor_->increase_by_statid(
        0x239c, proc_frame->app_id_, proc_frame->frame_command_,
        proc_frame->frame_length_);
    return 0;
}

int Transaction_Manager::get_clone_from_pool(unsigned int frame_cmd,
                                             unsigned int qq_uin,
                                             Transaction_Base *&crt_trans) {
    int ret;
    regtrans_pool_map_type::iterator mapiter = regtrans_pool_map_.find(frame_cmd);
    if (mapiter == regtrans_pool_map_.end()) {
        return 0x13dd;
    }
    CREATE_TRANS_RECORD ctr_trans_rec(0);
    CREATE_TRANS_RECORD &reg_ctr_trans =
        hash_map_find_or_insert(regtrans_pool_map_, frame_cmd,
                                ctr_trans_rec).second;
    if (reg_ctr_trans.if_auto_trans_lock_) {
        ret = lock_qquin_trnas_cmd(qq_uin, reg_ctr_trans.trans_lock_cmd_,
                                   frame_cmd);
        if (ret != 0) {
            return ret;
        }
    }
    ZEN_Trace_LogMsg::debug_debugEx(
        "Get clone frame command [%u],QQUin[%u],Pool size=[%u].", frame_cmd,
        qq_uin, reg_ctr_trans.crttrs_cmd_pool_.size());
    if (reg_ctr_trans.crttrs_cmd_pool_.size() == 1) {
        ZEN_Trace_LogMsg::debug_infoex("[framework] Before extend trans.");
        Transaction_Base *model_trans = 0;
        if (reg_ctr_trans.crttrs_cmd_pool_.size() != 0) {
            reg_ctr_trans.crttrs_cmd_pool_.pop_front(model_trans);
        }
        size_t pool_capacity = reg_ctr_trans.crttrs_cmd_pool_.capacity();
        size_t new_capacity =
            pool_capacity + POOL_EXTEND_TRANSACTION_NUM;
        if (new_capacity >= reg_ctr_trans.crttrs_cmd_pool_.size()) {
            ZEN_Trace_LogMsg::debug_infoex(
                "[framework] Pool Size=%u,  command %u, capacity = %u , resize =%u .",
                reg_ctr_trans.crttrs_cmd_pool_.size(), frame_cmd,
                pool_capacity, new_capacity);
            Transaction_Base **new_data =
                new Transaction_Base *[new_capacity];
            size_t old_size = reg_ctr_trans.crttrs_cmd_pool_.size();
            if (old_size != 0) {
                for (size_t i = 0; i < old_size; ++i) {
                    new_data[i] = reg_ctr_trans.crttrs_cmd_pool_[
                        (unsigned int)i];
                }
            }
            if (reg_ctr_trans.crttrs_cmd_pool_.value_ptr_ != 0) {
                delete[] reg_ctr_trans.crttrs_cmd_pool_.value_ptr_;
                reg_ctr_trans.crttrs_cmd_pool_.value_ptr_ = 0;
            }
            reg_ctr_trans.crttrs_cmd_pool_.cycdeque_start_ = 0;
            reg_ctr_trans.crttrs_cmd_pool_.cycdeque_capacity_ = new_capacity;
            reg_ctr_trans.crttrs_cmd_pool_.value_ptr_ = new_data;
        }
        for (size_t i = 0; i < POOL_EXTEND_TRANSACTION_NUM; ++i) {
            Transaction_Base *cloned_txbase =
                model_trans->create_self(timer_queue_, this);
            reg_ctr_trans.crttrs_cmd_pool_.push_back(cloned_txbase);
        }
        reg_ctr_trans.crttrs_cmd_pool_.push_back(model_trans);
        ZEN_Trace_LogMsg::debug_infoex("[framework] After Extend trans.");
    }
    if (reg_ctr_trans.crttrs_cmd_pool_.size() != 0) {
        reg_ctr_trans.crttrs_cmd_pool_.pop_front(crt_trans);
    }
    crt_trans->re_init();
    return 0;
}

int Transaction_Manager::return_clone_to_pool(unsigned int frame_cmd,
                                              Transaction_Base *&rt_txbase) {
    CREATE_TRANS_RECORD ctr_trans_rec(0);
    CREATE_TRANS_RECORD &reg_ctr_trans =
        hash_map_find_or_insert(regtrans_pool_map_, frame_cmd,
                                ctr_trans_rec).second;
    ZEN_Trace_LogMsg::debug_debugEx(
        "[framework] Return clone frame command %u,Pool size=%u .",
        frame_cmd, reg_ctr_trans.crttrs_cmd_pool_.size());
    if (reg_ctr_trans.if_auto_trans_lock_) {
        unlock_qquin_trans_cmd(rt_txbase->req_qq_uin_,
                               reg_ctr_trans.trans_lock_cmd_);
    }
    rt_txbase->finish();
    reg_ctr_trans.crttrs_cmd_pool_.push_back(rt_txbase);
    return 0;
}

void Transaction_Manager::recycle_clone_from_pool() {
    size_t i = 1;
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] Recycle trans,transaction manager are processing pool number [%d] . ",
        regtrans_pool_map_.size());
    for (regtrans_pool_map_type::iterator iter_tmp =
             regtrans_pool_map_.begin();
         iter_tmp != regtrans_pool_map_.end(); ++iter_tmp) {
        CREATE_TRANS_RECORD &pool_regtrans = iter_tmp->second;
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] %u.Pool porcess command:%u,capacity:%u,size:%u,use:%u.",
            i++, pool_regtrans.trans_command_,
            pool_regtrans.crttrs_cmd_pool_.capacity(),
            pool_regtrans.crttrs_cmd_pool_.size(),
            pool_regtrans.crttrs_cmd_pool_.capacity()
                - pool_regtrans.crttrs_cmd_pool_.size());
        size_t pool_capacity = pool_regtrans.crttrs_cmd_pool_.capacity();
        if (pool_capacity - pool_regtrans.crttrs_cmd_pool_.size() > 0xff
            || pool_regtrans.crttrs_cmd_pool_.size()
                   <= RECYCLE_POOL_THRESHOLD_VALUE) {
            continue;
        }
        for (size_t j = 0; j < POOL_EXTEND_TRANSACTION_NUM; ++j) {
            Transaction_Base *recycle_txbase = 0;
            if (pool_regtrans.crttrs_cmd_pool_.size() != 0) {
                pool_regtrans.crttrs_cmd_pool_.pop_front(recycle_txbase);
                if (recycle_txbase != 0) {
                    delete recycle_txbase;
                }
            }
        }
        size_t new_capacity =
            pool_capacity - POOL_EXTEND_TRANSACTION_NUM;
        if (new_capacity >= pool_regtrans.crttrs_cmd_pool_.size()) {
            Transaction_Base **new_data =
                new Transaction_Base *[new_capacity];
            size_t old_size = pool_regtrans.crttrs_cmd_pool_.size();
            if (old_size != 0) {
                for (size_t k = 0; k < old_size; ++k) {
                    new_data[k] =
                        pool_regtrans.crttrs_cmd_pool_[(unsigned int)k];
                }
            }
            if (pool_regtrans.crttrs_cmd_pool_.value_ptr_ != 0) {
                delete[] pool_regtrans.crttrs_cmd_pool_.value_ptr_;
                pool_regtrans.crttrs_cmd_pool_.value_ptr_ = 0;
            }
            pool_regtrans.crttrs_cmd_pool_.cycdeque_start_ = 0;
            pool_regtrans.crttrs_cmd_pool_.cycdeque_capacity_ = new_capacity;
            pool_regtrans.crttrs_cmd_pool_.value_ptr_ = new_data;
        }
    }
}

int Transaction_Manager::lock_qquin_trnas_cmd(unsigned int qq_uin,
                                              unsigned int trnas_lock_id,
                                              unsigned int frame_cmd) {
    TRANS_LOCK_RECORD lock_rec(qq_uin, trnas_lock_id);
    trans_lock_pool_.resize(trans_lock_pool_.size() + 1);
    std::pair<__gnu_cxx::hash_set<
                  TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK,
                  EQUAL_OF_TRANS_LOCK>::iterator, bool> iter_tmp =
        trans_lock_pool_.insert(lock_rec);
    if (iter_tmp.second) {
        return 0;
    }
    ZEN_Trace_LogMsg::debug_errorex(
        "[framework] [LOCK]Oh!Transaction lock fail.QQUin[%u] trans lock id[%u] trans cmd[%u].",
        qq_uin, trnas_lock_id, frame_cmd);
    return -1;
}

int Transaction_Manager::unlock_qquin_trans_cmd(unsigned int qq_uin,
                                                unsigned int lock_cmd) {
    TRANS_LOCK_RECORD lock_rec(qq_uin, lock_cmd);
    return trans_lock_pool_.erase(lock_rec);
}

int Transaction_Manager::process_appframe(Comm_App_Frame *ppetappframe,
                                          bool &bcrttx) {
    int ret;
    Transaction_Base *crt_trans = 0;
    Transaction_Base *run_tans = 0;
    bcrttx = false;
    if (ppetappframe->frame_option_ & 0x10000) {
        Comm_App_Frame::dumpoutput_framehead(
            ppetappframe, "[TRACK MONITOR][TRANS PROCESS]", RS_INFO);
    }
    ret = get_clone_from_pool(ppetappframe->frame_command_,
                              ppetappframe->frame_uin_, crt_trans);
    if (ret != 0) {
        ret = get_handler_by_transid(ppetappframe->backfill_trans_id_,
                                     ppetappframe->frame_command_, run_tans);
        if (ret != 0) {
            return ret;
        }
        ZEN_Trace_LogMsg::debug_debugEx(
            "Find raw Transaction ID: %u. ",
            ppetappframe->backfill_trans_id_);
        ret = run_tans->check_receive_frame(ppetappframe);
        if (ret != 0) {
            return ret;
        }
        ret = run_tans->receive_trans_msg(ppetappframe);
        if (ret != 0) {
            run_tans->handle_close();
        }
        return ret;
    }
    trans_id_builder_++;
    if (trans_id_builder_ == 0) {
        trans_id_builder_ = 1;
    }
    ret = regiester_trans_id(trans_id_builder_,
                             ppetappframe->frame_command_, crt_trans);
    if (ret != 0) {
        return_clone_to_pool(ppetappframe->frame_command_, crt_trans);
        return ret;
    }
    ret = crt_trans->initialize_trans(ppetappframe, trans_id_builder_);
    if (ret != 0) {
        crt_trans->handle_close();
        return ret;
    }
    bcrttx = true;
    gen_trans_counter_++;
    cycle_gentrans_counter_++;
    ZEN_Trace_LogMsg::debug_debugEx(
        "Create Trascation ,Command:%u Transaction ID:%u .",
        ppetappframe->frame_command_, trans_id_builder_);
    return 0;
}

int Transaction_Manager::get_handler_by_transid(unsigned int transid,
                                                unsigned int trans_cmd,
                                                Transaction_Base *&ptxbase) {
    transc_map_type::iterator mapiter = transc_map_.find(transid);
    if (mapiter == transc_map_.end()) {
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] get_handler_by_transid,Can't Find Transaction ID:%u,Command:%u From MAP.",
            trans_cmd, transid);
        return 0x139e;
    }
    ptxbase = mapiter->second;
    return 0;
}

void Transaction_Manager::dump_all_trans_info() const {
    size_t i = 1;
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] Transaction Manager are processing [%d] transactions. ",
        transc_map_.size());
    for (transc_map_const_iterator iter_tmp = transc_map_.begin();
         iter_tmp != transc_map_.end(); ++iter_tmp) {
        Transaction_Base *txbase = iter_tmp->second;
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] %u.Transaction ID:%u,Request UIN:%u,Command:%u,State:%u.",
            i++, txbase->req_qq_uin_, txbase->req_qq_uin_,
            txbase->trans_command_, txbase->trans_phase_,
            txbase->transaction_id_);
    }
}

void Transaction_Manager::dump_trans_pool_info() const {
    size_t i = 1;
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] Transaction Manager are processing pool number [%d] . ",
        regtrans_pool_map_.size());
    for (regtrans_pool_map_const_iterator iter_tmp =
             regtrans_pool_map_.begin();
         iter_tmp != regtrans_pool_map_.end(); ++iter_tmp) {
        const CREATE_TRANS_RECORD &reg_ctr_trans = iter_tmp->second;
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] %u.Pool porcess command:%u,capacity:%u,size:%u.",
            i++, reg_ctr_trans.trans_command_,
            reg_ctr_trans.crttrs_cmd_pool_.capacity(),
            reg_ctr_trans.crttrs_cmd_pool_.size());
    }
}

void Transaction_Manager::dump_statistics_info() const {
    size_t i = 1;
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] [TRANS INFO] All generate transaction counter [%llu] ,previous cycle generate transaction number[%llu].",
        gen_trans_counter_, cycle_gentrans_counter_);
    ZEN_Trace_LogMsg::debug_infoex(
        "[framework] Transaction Manager are processing [%d] transactions. ",
        transc_map_.size());
    for (regtrans_pool_map_const_iterator iter_tmp =
             regtrans_pool_map_.begin();
         iter_tmp != regtrans_pool_map_.end(); ++iter_tmp) {
        const CREATE_TRANS_RECORD &reg_ctr_trans = iter_tmp->second;
        ZEN_Trace_LogMsg::debug_infoex(
            "[framework] [TRANS INFO]%u.Transaction command ID [%u],create [%llu], destroy right[%llu], destroy timeout[%llu],destroy exception[%llu],consume seconds[%llu]",
            i++, reg_ctr_trans.trans_command_,
            reg_ctr_trans.create_trans_num_,
            reg_ctr_trans.destroy_right_num_,
            reg_ctr_trans.destroy_timeout_num_,
            reg_ctr_trans.destroy_exception_num_,
            reg_ctr_trans.trans_consume_time_);
    }
}

void Transaction_Manager::dump_all_debug_info() const {
    dump_trans_pool_info();
    dump_statistics_info();
    dump_all_trans_info();
}

int Transaction_Manager::mgr_sendmsghead_to_service(
    unsigned int cmd, unsigned int qquin, const SERVICES_ID &rcvsvc,
    const SERVICES_ID &proxysvc, unsigned int backfill_trans_id,
    unsigned int app_id, unsigned int option) {
    Comm_App_Frame *rsp_msg = trans_send_buffer_;
    rsp_msg->init_framehead(0, 0, 0x10000);
    rsp_msg->frame_length_ = Comm_App_Frame::LEN_OF_APPFRAME_HEAD;
    rsp_msg->frame_command_ = cmd;
    rsp_msg->transaction_id_ = 0;
    rsp_msg->frame_uin_ = qquin;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ = self_services_id_;
    rsp_msg->frame_option_ = option;
    rsp_msg->backfill_trans_id_ = backfill_trans_id;
    rsp_msg->app_id_ = app_id;
    return push_back_sendpipe(rsp_msg);
}

void Transaction_Manager::enable_trans_statistics(
    const ZEN_Time_Value *stat_clock) {
    statistics_clock_ = stat_clock;
}

int Transaction_Manager::mgr_postframe_to_msgqueue(Comm_App_Frame *post_frame) {
    int ret;
    Comm_App_Frame *tmp_frame = 0;
    inner_frame_malloc_->clone_appframe(post_frame, tmp_frame);
    ret = inner_message_queue_->enqueue(tmp_frame);
    if (ret != 0) {
        ZEN_Trace_LogMsg::debug_debugEx(
            "Post message to send queue fail.ret =%dSend queue message_count:%u message_bytes:%u. ",
            -1, inner_message_queue_->size(),
            inner_message_queue_->size() * 4);
        inner_frame_malloc_->free_appframe(tmp_frame);
        return 0x13e1;
    }
    return 0;
}

int Transaction_Manager::process_queue_frame(size_t &proc_frame,
                                             size_t &create_trans) {
    proc_frame = 0;
    create_trans = 0;
    if (inner_message_queue_ == 0) {
        return 0;
    }
    for (size_t i = 0;
         i < MAX_ONCE_PROCESS_FRAME && inner_message_queue_->size() > 0;
         ++i) {
        Comm_App_Frame *tmp_frame = 0;
        int ret = inner_message_queue_->dequeue(tmp_frame);
        if (ret != 0) {
            ZEN_Trace_LogMsg::debug_errorex(
                "[framework] Recv queue dequeue fail ,ret=%u,", ret);
            break;
        }
        bool bcrtcx = false;
        ret = process_appframe(tmp_frame, bcrtcx);
        inner_frame_malloc_->free_appframe(tmp_frame);
        if (ret == 0 && bcrtcx) {
            create_trans++;
        }
        proc_frame++;
    }
    return 0;
}

void Transaction_Manager::get_manager_load_foctor(unsigned int &load_max,
                                                  unsigned int &load_cur) {
    load_max = max_trans_;
    load_cur = transc_map_.size();
    if (load_cur == 0) {
        load_cur = 1;
    }
}

void Transaction_Manager::get_manager_load_foctor2(unsigned int &load_max,
                                                   unsigned int &load_cur) {
    const unsigned int ONE_CYCLE_GENERATE_TRANS = 0x7530;
    load_max = ONE_CYCLE_GENERATE_TRANS;
    if (cycle_gentrans_counter_ > ONE_CYCLE_GENERATE_TRANS) {
        load_cur = ONE_CYCLE_GENERATE_TRANS;
    } else {
        load_cur = cycle_gentrans_counter_;
    }
    cycle_gentrans_counter_ = 0;
    if (load_cur == 0) {
        load_cur = 1;
    }
}

Transaction_Manager *Transaction_Manager::instance() {
    if (instance_ == 0) {
        instance_ = new Transaction_Manager();
    }
    return instance_;
}

void Transaction_Manager::instance(Transaction_Manager *pinstatnce) {
    clean_instance();
    instance_ = pinstatnce;
}

void Transaction_Manager::clean_instance() {
    if (instance_ != 0) {
        delete instance_;
        instance_ = 0;
    }
}

int Transaction_Manager::mgr_sendbuf_to_service(
    unsigned int cmd, unsigned int qquin, unsigned int transaction_id,
    unsigned int backfill_trans_id, const SERVICES_ID &rcvsvc,
    const SERVICES_ID &proxysvc, const SERVICES_ID &sndsvc,
    const unsigned char *buf, size_t buf_len, unsigned int app_id,
    unsigned int option) {
    return zerg_mmap_pipe_->pipe_sendbuf_to_service(
        cmd, qquin, transaction_id, backfill_trans_id, rcvsvc, proxysvc,
        sndsvc, buf, buf_len, app_id, option);
}

Transaction_Manager *Transaction_Manager::instance_ = 0;

// 显式实例化（原二进制弱符号：AppFrame_Mallocor_Mgr<ZEN_Null_Mutex> 模板）
template class AppFrame_Mallocor_Mgr<ZEN_Null_Mutex>;
