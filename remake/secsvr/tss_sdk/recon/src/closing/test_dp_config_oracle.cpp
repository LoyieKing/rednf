// Independent differential oracle for the reconstructed dp_config.cpp.
//
// This file is intentionally named test_* so build_server.py does not compile
// it into libtss_sdk.  oracle_dp_config.sh builds it separately and runs it
// twice: once against reference/libtss_sdk.so.1.0.394.orig and once against
// build-server/libtss_sdk.so.1.0.394.  Both runs print only "RESULT ..." lines;
// the script diffs those lines.
//
// Modes
//   pure : comparators, get_cmd_id, sort_dp_config and get_comm_mem with a
//          unique /tmp path.  No live shared memory and no
//          /tmp/dp_comm_config* access.  The file-local
//          init_dp_config(DPCmdLists&) helper is exercised indirectly through
//          the cur_crc fields of full mode (the ORIG does not export it).
//   full : init_dp_config()/reload_dp_config() over the ORIG's fixed paths
//          (/tmp/dp_comm_config_head, /tmp/dp_comm_config).  It refuses to run
//          unless DPCFG_ORACLE_ISOLATED=1, which oracle_dp_config.sh sets only
//          inside a private user+mount namespace whose /tmp is a fresh tmpfs.
//
// The library is dlopen()ed; nothing from the library is linked into this
// executable.

#include "src/closing/dp_config.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// ---------------------------------------------------------------------------
// Minimal definitions for the ABI model declared in dp_config.h.  Only the
// constructor/destructor are needed by this harness; the library keeps its own
// open()/addr() and static objects.
// ---------------------------------------------------------------------------
ZEN_ShareMem_Mmap::ZEN_ShareMem_Mmap()
    : mmap_addr_(NULL), mmap_handle_(-1), shm_size_(0) {}

ZEN_ShareMem_Mmap::~ZEN_ShareMem_Mmap() {
    // Deliberately no munmap/close: the oracle owns no live mapping policy.
}

namespace {

void fail(const char *what) {
    fprintf(stderr, "ORACLE_FAIL %s\n", what);
    exit(2);
}

void require_symbol(bool ok, const char *name) {
    if (!ok) {
        fprintf(stderr, "ORACLE_MISSING %s\n", name);
        exit(2);
    }
}

unsigned int fnv1a(const void *data, size_t size) {
    const unsigned char *p = static_cast<const unsigned char *>(data);
    unsigned int h = 2166136261u;
    for (size_t i = 0; i < size; ++i) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

void print_u32_array(const char *tag, const unsigned int *values,
                     unsigned int count) {
    printf("RESULT %s.n=%u", tag, count);
    for (unsigned int i = 0; i < count; ++i) {
        printf(" %u", values[i]);
    }
    printf("\n");
}

// ---------------------------------------------------------------------------
// Library surface
// ---------------------------------------------------------------------------
struct Api {
    void *handle;
    int (*cmp_uint32)(const void *, const void *);
    int (*cmp_dpsubcmdlist)(const void *, const void *);
    int (*cmp_dphackfragment)(const void *, const void *);
    void (*sort_dp_config)(tss_dp_comm::DPConfig &);
    void *(*get_comm_mem)(ZEN_ShareMem_Mmap &, const char *, unsigned int);
    int (*get_cmd_id)(const tss_dp_comm::DPGameCmdStruct &, unsigned char *,
                      unsigned short, unsigned int &);
    int (*do_reload_dp_config)();
    int (*reload_dp_config)();
    int (*init_dp_config)();
    tss_dp_comm::DPConfig **glb_dp_config;
    tss_dp_comm::DP_CONFIG **glb_dp_config_head;
    unsigned int *glb_config_update_time;
};

template <typename T>
T lookup(void *handle, const char *name) {
    void *p = dlsym(handle, name);
    require_symbol(p != NULL, name);
    return reinterpret_cast<T>(p);
}

void load_api(const char *path, Api *api) {
    memset(api, 0, sizeof(*api));
    api->handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (api->handle == NULL) {
        fprintf(stderr, "ORACLE_DLOPEN_FAIL %s\n", dlerror());
        exit(2);
    }
    api->cmp_uint32 = lookup<int (*)(const void *, const void *)>(
        api->handle, "_Z10cmp_uint32PKvS0_");
    api->cmp_dpsubcmdlist = lookup<int (*)(const void *, const void *)>(
        api->handle, "_Z16cmp_dpsubcmdlistPKvS0_");
    api->cmp_dphackfragment = lookup<int (*)(const void *, const void *)>(
        api->handle, "_Z18cmp_dphackfragmentPKvS0_");
    api->sort_dp_config =
        lookup<void (*)(tss_dp_comm::DPConfig &)>(
            api->handle, "_Z14sort_dp_configRN11tss_dp_comm8DPConfigE");
    api->get_comm_mem =
        lookup<void *(*)(ZEN_ShareMem_Mmap &, const char *, unsigned int)>(
            api->handle,
            "_ZN11tss_dp_comm12get_comm_memER17ZEN_ShareMem_MmapPKcj");
    api->get_cmd_id = lookup<int (*)(
        const tss_dp_comm::DPGameCmdStruct &, unsigned char *,
        unsigned short, unsigned int &)>(
        api->handle,
        "_ZN11tss_dp_comm10get_cmd_idERKNS_15DPGameCmdStructEPhtRj");
    api->do_reload_dp_config =
        lookup<int (*)()>(api->handle,
                          "_ZN11tss_dp_comm19do_reload_dp_configEv");
    api->reload_dp_config =
        lookup<int (*)()>(api->handle,
                          "_ZN11tss_dp_comm16reload_dp_configEv");
    api->init_dp_config =
        lookup<int (*)()>(api->handle, "_ZN11tss_dp_comm14init_dp_configEv");
    api->glb_dp_config = lookup<tss_dp_comm::DPConfig **>(
        api->handle, "_ZN11tss_dp_comm9DP_CONFIG14glb_dp_config_E");
    api->glb_dp_config_head = lookup<tss_dp_comm::DP_CONFIG **>(
        api->handle, "_ZN11tss_dp_comm9DP_CONFIG19glb_dp_config_head_E");
    api->glb_config_update_time = lookup<unsigned int *>(
        api->handle, "_ZN11tss_dp_comm9DP_CONFIG23glb_config_update_time_E");
}

// ---------------------------------------------------------------------------
// pure mode
// ---------------------------------------------------------------------------
void test_cmp(const Api &api) {
    unsigned int values[] = {0u, 1u, 2u, 0x7fffffffu, 0x80000000u,
                             0xffffffffu, 7u, 7u};
    const unsigned int n = sizeof(values) / sizeof(values[0]);
    for (unsigned int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < n; ++j) {
            const int r = api.cmp_uint32(&values[i], &values[j]);
            printf("RESULT cmp_uint32.%u.%u=%d\n", i, j, r);
        }
    }

    static tss_dp_comm::DPSubCmdList list[6];
    memset(list, 0, sizeof(list));
    const unsigned int ids[6] = {3u, 1u, 2u, 2u, 0u, 0xffffffffu};
    for (unsigned int i = 0; i < 6; ++i) {
        list[i].cmd_id = ids[i];
    }
    for (unsigned int i = 0; i < 6; ++i) {
        for (unsigned int j = 0; j < 6; ++j) {
            const int r = api.cmp_dpsubcmdlist(&list[i], &list[j]);
            printf("RESULT cmp_dpsubcmdlist.%u.%u=%d\n", i, j, r);
        }
    }

    static tss_dp_comm::DPHackFragment frag[4];
    memset(frag, 0, sizeof(frag));
    frag[0].cmd_start = 5;
    frag[1].cmd_start = 1;
    frag[2].cmd_start = 5;
    frag[3].cmd_start = 9;
    for (unsigned int i = 0; i < 4; ++i) {
        for (unsigned int j = 0; j < 4; ++j) {
            const int r = api.cmp_dphackfragment(&frag[i], &frag[j]);
            printf("RESULT cmp_dphackfragment.%u.%u=%d\n", i, j, r);
        }
    }
}

void test_get_cmd_id(const Api &api) {
    unsigned char buffer[512];
    for (unsigned int i = 0; i < sizeof(buffer); ++i) {
        buffer[i] = static_cast<unsigned char>((i * 37u + 11u) & 0xffu);
    }
    const unsigned short lengths[] = {0, 1, 2, 3, 4, 5, 6, 7, 8,
                                      0x7fff, 0x8000, 0xffff};
    const unsigned int offsets[] = {0, 1, 3, 7, 255};
    for (unsigned int type = 0; type <= 8; ++type) {
        for (unsigned int oi = 0; oi < sizeof(offsets) / sizeof(offsets[0]);
             ++oi) {
            for (unsigned int li = 0;
                 li < sizeof(lengths) / sizeof(lengths[0]); ++li) {
                tss_dp_comm::DPGameCmdStruct cmd;
                cmd.cmd_id_type = static_cast<uint8_t>(type);
                cmd.cmd_id_offset = static_cast<uint8_t>(offsets[oi]);
                unsigned int cmd_id = 0xdeadbeefu;
                const int ret = api.get_cmd_id(cmd, buffer, lengths[li],
                                               cmd_id);
                printf("RESULT get_cmd_id.t%u.o%u.l%u.ret=%d.id=%u\n",
                       type, offsets[oi], lengths[li], ret, cmd_id);
            }
        }
    }
}

void fill_dp_config(tss_dp_comm::DPConfig *cfg) {
    memset(cfg, 0, sizeof(*cfg));

    static const unsigned int drop_ids[5] = {9, 3, 7, 3, 1};
    cfg->cmd_hack.drop_subcmd_count = 5;
    for (unsigned int i = 0; i < 5; ++i) {
        cfg->cmd_hack.drop_subcmd_list[i].cmd_id = drop_ids[i];
        cfg->cmd_hack.drop_subcmd_list[i].subcmd_list.length =
            static_cast<uint16_t>((i * 3u) % 9u);
        for (unsigned int j = 0; j < 8; ++j) {
            cfg->cmd_hack.drop_subcmd_list[i].subcmd_list.values[j] =
                ((i + 1u) * 13u + j * 7u) % 23u;
        }
    }

    static const unsigned int fwd_ids[4] = {4, 8, 4, 2};
    cfg->cmd_hack.forward_subcmd_count = 4;
    for (unsigned int i = 0; i < 4; ++i) {
        cfg->cmd_hack.forward_subcmd_list[i].cmd_id = fwd_ids[i];
        cfg->cmd_hack.forward_subcmd_list[i].subcmd_list.length =
            static_cast<uint16_t>((i + 2u) % 6u);
        for (unsigned int j = 0; j < 8; ++j) {
            cfg->cmd_hack.forward_subcmd_list[i].subcmd_list.values[j] =
                (i * 5u + j * 3u + 1u) % 17u;
        }
    }

    cfg->cmd_hack.up_cmd_hack.fragment_count = 5;
    for (unsigned int i = 0; i < 5; ++i) {
        cfg->cmd_hack.up_cmd_hack.hack_fragments[i].cmd_start =
            (i * 17u + 5u) % 61u;
        cfg->cmd_hack.up_cmd_hack.hack_fragments[i].cmd_end =
            cfg->cmd_hack.up_cmd_hack.hack_fragments[i].cmd_start + 4u;
    }
    cfg->cmd_hack.down_cmd_hack.fragment_count = 4;
    for (unsigned int i = 0; i < 4; ++i) {
        cfg->cmd_hack.down_cmd_hack.hack_fragments[i].cmd_start =
            (i * 11u + 2u) % 41u;
    }
    cfg->cmd_hack.raw_cmd_hack.fragment_count = 3;
    for (unsigned int i = 0; i < 3; ++i) {
        cfg->cmd_hack.raw_cmd_hack.hack_fragments[i].cmd_start =
            (i * 7u + 1u) % 19u;
    }

    cfg->cmd_hack.delay_cmd.safemode_drop_cmdlist.length = 7;
    for (unsigned int i = 0; i < 7; ++i) {
        cfg->cmd_hack.delay_cmd.safemode_drop_cmdlist.values[i] =
            (i * 9u + 3u) % 31u;
    }

    cfg->dp_code_info.up_cmd_list.cur_posi = 2;
    cfg->dp_code_info.up_cmd_list.lists_count = 4;
    for (unsigned int i = 0; i < 4; ++i) {
        cfg->dp_code_info.up_cmd_list.cmd_lists[i].length =
            static_cast<uint16_t>(5u + i);
        for (unsigned int j = 0; j < 10; ++j) {
            cfg->dp_code_info.up_cmd_list.cmd_lists[i].values[j] =
                (i * 100u + j * 7u + 3u) % 97u;
        }
    }
    cfg->dp_code_info.down_cmd_list.cur_posi = 1;
    cfg->dp_code_info.down_cmd_list.lists_count = 3;
    for (unsigned int i = 0; i < 3; ++i) {
        cfg->dp_code_info.down_cmd_list.cmd_lists[i].length =
            static_cast<uint16_t>(3u + i);
        for (unsigned int j = 0; j < 8; ++j) {
            cfg->dp_code_info.down_cmd_list.cmd_lists[i].values[j] =
                (i * 50u + j * 11u + 1u) % 83u;
        }
    }

    cfg->uin_monitor_list.length = 9;
    for (unsigned int i = 0; i < 9; ++i) {
        cfg->uin_monitor_list.values[i] = (i * 37u + 11u) % 101u;
    }
}

void test_sort(const Api &api) {
    static tss_dp_comm::DPConfig cfg;
    fill_dp_config(&cfg);
    api.sort_dp_config(cfg);
    printf("RESULT sort.hash=%u\n", fnv1a(&cfg, sizeof(cfg)));

    unsigned int tmp[8];
    for (unsigned int i = 0; i < 5; ++i) {
        tmp[i] = cfg.cmd_hack.drop_subcmd_list[i].cmd_id;
    }
    print_u32_array("sort.drop_ids", tmp, 5);
    for (unsigned int i = 0; i < 5; ++i) {
        print_u32_array("sort.drop_sub_values", 
                        cfg.cmd_hack.drop_subcmd_list[i].subcmd_list.values,
                        cfg.cmd_hack.drop_subcmd_list[i].subcmd_list.length);
    }
    for (unsigned int i = 0; i < 4; ++i) {
        tmp[i] = cfg.cmd_hack.forward_subcmd_list[i].cmd_id;
    }
    print_u32_array("sort.forward_ids", tmp, 4);
    for (unsigned int i = 0; i < 4; ++i) {
        print_u32_array(
            "sort.forward_sub_values",
            cfg.cmd_hack.forward_subcmd_list[i].subcmd_list.values,
            cfg.cmd_hack.forward_subcmd_list[i].subcmd_list.length);
    }
    for (unsigned int i = 0; i < 5; ++i) {
        tmp[i] = cfg.cmd_hack.up_cmd_hack.hack_fragments[i].cmd_start;
    }
    print_u32_array("sort.up_frag_starts", tmp, 5);
    for (unsigned int i = 0; i < 4; ++i) {
        tmp[i] = cfg.cmd_hack.down_cmd_hack.hack_fragments[i].cmd_start;
    }
    print_u32_array("sort.down_frag_starts", tmp, 4);
    for (unsigned int i = 0; i < 3; ++i) {
        tmp[i] = cfg.cmd_hack.raw_cmd_hack.hack_fragments[i].cmd_start;
    }
    print_u32_array("sort.raw_frag_starts", tmp, 3);
    print_u32_array("sort.safemode_values",
                    cfg.cmd_hack.delay_cmd.safemode_drop_cmdlist.values,
                    cfg.cmd_hack.delay_cmd.safemode_drop_cmdlist.length);
    for (unsigned int i = 0; i < 4; ++i) {
        printf("RESULT sort.up_list.%u.len=%u\n", i,
               cfg.dp_code_info.up_cmd_list.cmd_lists[i].length);
        print_u32_array("sort.up_list_values",
                        cfg.dp_code_info.up_cmd_list.cmd_lists[i].values,
                        cfg.dp_code_info.up_cmd_list.cmd_lists[i].length);
    }
    for (unsigned int i = 0; i < 3; ++i) {
        printf("RESULT sort.down_list.%u.len=%u\n", i,
               cfg.dp_code_info.down_cmd_list.cmd_lists[i].length);
        print_u32_array("sort.down_list_values",
                        cfg.dp_code_info.down_cmd_list.cmd_lists[i].values,
                        cfg.dp_code_info.down_cmd_list.cmd_lists[i].length);
    }
    print_u32_array("sort.uin_values", cfg.uin_monitor_list.values,
                    cfg.uin_monitor_list.length);
}

void test_get_comm_mem(const Api &api) {
    char path[128];
    snprintf(path, sizeof(path), "/tmp/dpcfg_oracle_fresh_%d", (int)getpid());
    unlink(path);
    {
        ZEN_ShareMem_Mmap mmap;
        void *p = api.get_comm_mem(mmap, path, 0x1000);
        printf("RESULT mem.fresh_nonnull=%d\n", p != NULL ? 1 : 0);
        if (p != NULL) {
            unsigned char *b = static_cast<unsigned char *>(p);
            b[0] = 0x5a;
            b[0xfff] = 0xa5;
            printf("RESULT mem.fresh_byte0=%u\n", (unsigned int)b[0]);
            printf("RESULT mem.fresh_bytelast=%u\n", (unsigned int)b[0xfff]);
        }
        struct stat st;
        if (stat(path, &st) == 0) {
            printf("RESULT mem.fresh_size=%u\n", (unsigned int)st.st_size);
        } else {
            printf("RESULT mem.fresh_size=-1\n");
        }
    }
    unlink(path);

    // Existing file smaller than the requested size: the restore=true attempt
    // must fail and get_comm_mem must retry with truncate/create.
    snprintf(path, sizeof(path), "/tmp/dpcfg_oracle_small_%d", (int)getpid());
    unlink(path);
    {
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd >= 0) {
            unsigned char small[16];
            memset(small, 0x33, sizeof(small));
            if (write(fd, small, sizeof(small)) != (ssize_t)sizeof(small)) {
                close(fd);
                fail("write small file");
            }
            close(fd);
        }
    }
    {
        ZEN_ShareMem_Mmap mmap;
        void *p = api.get_comm_mem(mmap, path, 0x1000);
        printf("RESULT mem.retry_nonnull=%d\n", p != NULL ? 1 : 0);
        if (p != NULL) {
            unsigned char *b = static_cast<unsigned char *>(p);
            b[0x100] = 0x77;
            printf("RESULT mem.retry_byte=%u\n", (unsigned int)b[0x100]);
        }
        struct stat st;
        if (stat(path, &st) == 0) {
            printf("RESULT mem.retry_size=%u\n", (unsigned int)st.st_size);
        } else {
            printf("RESULT mem.retry_size=-1\n");
        }
    }
    unlink(path);
}

void run_pure(const Api &api) {
    test_cmp(api);
    test_get_cmd_id(api);
    test_sort(api);
    test_get_comm_mem(api);
}

// ---------------------------------------------------------------------------
// full mode: ORIG fixed paths, only valid inside a private /tmp namespace
// ---------------------------------------------------------------------------
void write_file(const char *path, const void *data, size_t size) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        fail("open output file");
    }
    const unsigned char *p = static_cast<const unsigned char *>(data);
    size_t done = 0;
    while (done < size) {
        ssize_t n = write(fd, p + done, size - done);
        if (n <= 0) {
            close(fd);
            fail("write output file");
        }
        done += static_cast<size_t>(n);
    }
    close(fd);
}

const char *const kXml =
    "<?xml version=\"1.0\" encoding=\"GBK\"?>\n"
    "<DPConfig>\n"
    "<dp_log_conf>\n"
    "<bill_level>7</bill_level><game_id>99</game_id>\n"
    "<bill_path>bill.log</bill_path><monitor_address>mon</monitor_address>\n"
    "</dp_log_conf>\n"
    "<dp_code_info>\n"
    "<up_cmd_list>\n"
    "<cur_posi>2</cur_posi><lists_count>4</lists_count>\n"
    "<cmd_lists><length>6</length><values>60 10 50 20 40 30</values></cmd_lists>\n"
    "<cmd_lists><length>5</length><values>9 1 8 2 7</values></cmd_lists>\n"
    "<cmd_lists><length>7</length><values>100 50 100 25 75 0 50</values></cmd_lists>\n"
    "<cmd_lists><length>4</length><values>4 4 3 3</values></cmd_lists>\n"
    "</up_cmd_list>\n"
    "<down_cmd_list>\n"
    "<cur_posi>1</cur_posi><lists_count>3</lists_count>\n"
    "<cmd_lists><length>5</length><values>5 4 3 2 1</values></cmd_lists>\n"
    "<cmd_lists><length>6</length><values>11 12 13 11 12 13</values></cmd_lists>\n"
    "<cmd_lists><length>3</length><values>7 7 7</values></cmd_lists>\n"
    "</down_cmd_list>\n"
    "<code_dir_count>2</code_dir_count>\n"
    "<code_dir_path>codes0</code_dir_path>\n"
    "<code_dir_path>/abs/codes1</code_dir_path>\n"
    "</dp_code_info>\n"
    "<uin_monitor_list><length>5</length><values>900 100 500 300 700</values></uin_monitor_list>\n"
    "<cmd_hack>\n"
    "<delay_cmd><delay_cmd_time>9</delay_cmd_time>\n"
    "<safemode_drop_cmdlist><length>7</length>"
    "<values>70 60 50 40 30 20 10</values></safemode_drop_cmdlist>\n"
    "</delay_cmd>\n"
    "<safemode_to_client>1</safemode_to_client>\n"
    "<safemode_hint_client>0</safemode_hint_client>\n"
    "<cf_room_qtalk_enable>1</cf_room_qtalk_enable>\n"
    "<up_cmd_hack>\n"
    "<fragment_count>5</fragment_count>\n"
    "<hack_fragments><cmd_start>50</cmd_start><cmd_end>59</cmd_end><hack_list>1 2 3</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>10</cmd_start><cmd_end>19</cmd_end><hack_list>4 5</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>30</cmd_start><cmd_end>39</cmd_end><hack_list>6</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>30</cmd_start><cmd_end>35</cmd_end><hack_list>7</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>20</cmd_start><cmd_end>29</cmd_end><hack_list>8</hack_list></hack_fragments>\n"
    "</up_cmd_hack>\n"
    "<down_cmd_hack>\n"
    "<fragment_count>4</fragment_count>\n"
    "<hack_fragments><cmd_start>44</cmd_start><cmd_end>45</cmd_end><hack_list>1</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>22</cmd_start><cmd_end>23</cmd_end><hack_list>2</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>33</cmd_start><cmd_end>34</cmd_end><hack_list>3</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>11</cmd_start><cmd_end>12</cmd_end><hack_list>4</hack_list></hack_fragments>\n"
    "</down_cmd_hack>\n"
    "<raw_cmd_hack>\n"
    "<fragment_count>3</fragment_count>\n"
    "<hack_fragments><cmd_start>9</cmd_start><cmd_end>10</cmd_end><hack_list>9</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>1</cmd_start><cmd_end>2</cmd_end><hack_list>8</hack_list></hack_fragments>\n"
    "<hack_fragments><cmd_start>5</cmd_start><cmd_end>6</cmd_end><hack_list>7</hack_list></hack_fragments>\n"
    "</raw_cmd_hack>\n"
    "<drop_subcmd_count>3</drop_subcmd_count>\n"
    "<drop_subcmd_list><is_accept>1</is_accept><cmd_id_type>2</cmd_id_type>"
    "<cmd_id>300</cmd_id><subcmd_list><length>4</length>"
    "<values>4 3 2 1</values></subcmd_list></drop_subcmd_list>\n"
    "<drop_subcmd_list><is_accept>1</is_accept><cmd_id_type>2</cmd_id_type>"
    "<cmd_id>100</cmd_id><subcmd_list><length>3</length>"
    "<values>9 8 7</values></subcmd_list></drop_subcmd_list>\n"
    "<drop_subcmd_list><is_accept>0</is_accept><cmd_id_type>2</cmd_id_type>"
    "<cmd_id>200</cmd_id><subcmd_list><length>5</length>"
    "<values>5 5 4 4 3</values></subcmd_list></drop_subcmd_list>\n"
    "<forward_subcmd_count>2</forward_subcmd_count>\n"
    "<forward_subcmd_list><is_accept>1</is_accept><cmd_id_type>1</cmd_id_type>"
    "<cmd_id>20</cmd_id><subcmd_list><length>3</length>"
    "<values>3 1 2</values></subcmd_list></forward_subcmd_list>\n"
    "<forward_subcmd_list><is_accept>1</is_accept><cmd_id_type>1</cmd_id_type>"
    "<cmd_id>10</cmd_id><subcmd_list><length>2</length>"
    "<values>8 6</values></subcmd_list></forward_subcmd_list>\n"
    "</cmd_hack>\n"
    "</DPConfig>\n";

void prepare_shared_files() {
    // 280-byte ORIG header, pre-seeded so init_dp_config's restore=true mapping
    // keeps our values instead of truncating.
    tss_dp_comm::DP_CONFIG head;
    memset(&head, 0, sizeof(head));
    head.update_time = 0x11111111u;
    head.revert = 0x22222222u;
    head.game_id = 8;
    head.world_id = 2;
    head.secagent_id = 3;
    strncpy(head.dp_config_path, "/tmp/dpcfg_oracle.xml", 255);
    head.now_time = 0x12345678u;
    write_file("/tmp/dp_comm_config_head", &head, sizeof(head));

    unsigned char body[sizeof(tss_dp_comm::DP_CONFIG_BODY)];
    memset(body, 0xa5, sizeof(body));
    write_file("/tmp/dp_comm_config", body, sizeof(body));

    write_file("/tmp/dpcfg_oracle.xml", kXml, strlen(kXml));
}

void dump_array(const char *tag, const unsigned int *values,
                unsigned int count) {
    printf("RESULT %s.n=%u", tag, count);
    for (unsigned int i = 0; i < count; ++i) {
        printf(" %u", values[i]);
    }
    printf("\n");
}

void dump_state(const Api &api, const char *tag) {
    tss_dp_comm::DP_CONFIG *head = *api.glb_dp_config_head;
    tss_dp_comm::DPConfig *cfg = *api.glb_dp_config;
    printf("RESULT %s.head=%d cfg=%d uptime_global=%u\n", tag,
           head != NULL ? 1 : 0, cfg != NULL ? 1 : 0,
           *api.glb_config_update_time);
    if (head == NULL || cfg == NULL) {
        return;
    }

    printf("RESULT %s.head.update_time=%u\n", tag, head->update_time);
    printf("RESULT %s.head.revert=%u\n", tag, head->revert);
    printf("RESULT %s.head.game_id=%u\n", tag, head->game_id);
    printf("RESULT %s.head.world_id=%u\n", tag, head->world_id);
    printf("RESULT %s.head.secagent_id=%u\n", tag, head->secagent_id);
    printf("RESULT %s.head.path=%s\n", tag, head->dp_config_path);
    printf("RESULT %s.head.now_time=%u\n", tag, head->now_time);

    const tss_dp_comm::DP_CONFIG_BODY *body =
        reinterpret_cast<const tss_dp_comm::DP_CONFIG_BODY *>(
            reinterpret_cast<const char *>(cfg) - 4);
    printf("RESULT %s.body.cur_update_time=%u\n", tag,
           body->cur_update_time_);

    printf("RESULT %s.cfg.hash=%u\n", tag, fnv1a(cfg, sizeof(*cfg)));
    printf("RESULT %s.cfg.log.game_id=%u\n", tag, cfg->dp_log_conf.game_id);
    printf("RESULT %s.cfg.log.bill_level=%u\n", tag,
           cfg->dp_log_conf.bill_level);
    printf("RESULT %s.cfg.log.bill_path=%s\n", tag,
           cfg->dp_log_conf.bill_path);
    printf("RESULT %s.cfg.code_dir_count=%u\n", tag,
           (unsigned int)cfg->dp_code_info.code_dir_count);
    printf("RESULT %s.cfg.code_dir0=%s\n", tag,
           cfg->dp_code_info.code_dir_path[0]);
    printf("RESULT %s.cfg.code_dir1=%s\n", tag,
           cfg->dp_code_info.code_dir_path[1]);
    printf("RESULT %s.cfg.cmd_list_buf_size=%u\n", tag,
           (unsigned int)cfg->dp_code_info.cmd_list_buf_size);
    printf("RESULT %s.cfg.cmd_list_buf=", tag);
    for (unsigned int i = 0; i < 16; ++i) {
        printf("%02x", (unsigned int)cfg->dp_code_info.cmd_list_buf[i]);
    }
    printf("\n");
    printf("RESULT %s.cfg.pkt.size=%u\n", tag,
           (unsigned int)(cfg->dp_code_info.cmd_list_buf[0] |
                          (cfg->dp_code_info.cmd_list_buf[1] << 8)));
    printf("RESULT %s.cfg.pkt.protocol=%u\n", tag,
           (unsigned int)(cfg->dp_code_info.cmd_list_buf[2] |
                          (cfg->dp_code_info.cmd_list_buf[3] << 8)));
    printf("RESULT %s.cfg.pkt.datatype=%u\n", tag,
           (unsigned int)cfg->dp_code_info.cmd_list_buf[4]);

    printf("RESULT %s.cfg.hack.up.count=%u\n", tag,
           (unsigned int)cfg->cmd_hack.up_cmd_hack.fragment_count);
    for (unsigned int i = 0; i < cfg->cmd_hack.up_cmd_hack.fragment_count;
         ++i) {
        printf("RESULT %s.cfg.hack.up.%u.start=%u end=%u\n", tag, i,
               cfg->cmd_hack.up_cmd_hack.hack_fragments[i].cmd_start,
               cfg->cmd_hack.up_cmd_hack.hack_fragments[i].cmd_end);
    }
    printf("RESULT %s.cfg.hack.down.count=%u\n", tag,
           (unsigned int)cfg->cmd_hack.down_cmd_hack.fragment_count);
    for (unsigned int i = 0; i < cfg->cmd_hack.down_cmd_hack.fragment_count;
         ++i) {
        printf("RESULT %s.cfg.hack.down.%u.start=%u\n", tag, i,
               cfg->cmd_hack.down_cmd_hack.hack_fragments[i].cmd_start);
    }
    printf("RESULT %s.cfg.hack.raw.count=%u\n", tag,
           (unsigned int)cfg->cmd_hack.raw_cmd_hack.fragment_count);
    for (unsigned int i = 0; i < cfg->cmd_hack.raw_cmd_hack.fragment_count;
         ++i) {
        printf("RESULT %s.cfg.hack.raw.%u.start=%u\n", tag, i,
               cfg->cmd_hack.raw_cmd_hack.hack_fragments[i].cmd_start);
    }
    dump_array("cfg.hack.safemode",
               cfg->cmd_hack.delay_cmd.safemode_drop_cmdlist.values,
               cfg->cmd_hack.delay_cmd.safemode_drop_cmdlist.length);

    printf("RESULT %s.cfg.drop.count=%u\n", tag,
           (unsigned int)cfg->cmd_hack.drop_subcmd_count);
    for (unsigned int i = 0; i < cfg->cmd_hack.drop_subcmd_count; ++i) {
        printf("RESULT %s.cfg.drop.%u.id=%u\n", tag, i,
               cfg->cmd_hack.drop_subcmd_list[i].cmd_id);
        dump_array("cfg.drop.values",
                   cfg->cmd_hack.drop_subcmd_list[i].subcmd_list.values,
                   cfg->cmd_hack.drop_subcmd_list[i].subcmd_list.length);
    }
    printf("RESULT %s.cfg.fwd.count=%u\n", tag,
           (unsigned int)cfg->cmd_hack.forward_subcmd_count);
    for (unsigned int i = 0; i < cfg->cmd_hack.forward_subcmd_count; ++i) {
        printf("RESULT %s.cfg.fwd.%u.id=%u\n", tag, i,
               cfg->cmd_hack.forward_subcmd_list[i].cmd_id);
        dump_array("cfg.fwd.values",
                   cfg->cmd_hack.forward_subcmd_list[i].subcmd_list.values,
                   cfg->cmd_hack.forward_subcmd_list[i].subcmd_list.length);
    }

    printf("RESULT %s.cfg.up.cur=%u count=%u crc=%u\n", tag,
           (unsigned int)cfg->dp_code_info.up_cmd_list.cur_posi,
           (unsigned int)cfg->dp_code_info.up_cmd_list.lists_count,
           cfg->dp_code_info.up_cmd_list.cur_crc);
    for (unsigned int i = 0; i < cfg->dp_code_info.up_cmd_list.lists_count;
         ++i) {
        printf("RESULT %s.cfg.up.%u.len=%u\n", tag, i,
               (unsigned int)cfg->dp_code_info.up_cmd_list.cmd_lists[i].length);
        dump_array("cfg.up.values",
                   cfg->dp_code_info.up_cmd_list.cmd_lists[i].values,
                   cfg->dp_code_info.up_cmd_list.cmd_lists[i].length);
    }
    printf("RESULT %s.cfg.down.cur=%u count=%u crc=%u\n", tag,
           (unsigned int)cfg->dp_code_info.down_cmd_list.cur_posi,
           (unsigned int)cfg->dp_code_info.down_cmd_list.lists_count,
           cfg->dp_code_info.down_cmd_list.cur_crc);
    for (unsigned int i = 0; i < cfg->dp_code_info.down_cmd_list.lists_count;
         ++i) {
        printf("RESULT %s.cfg.down.%u.len=%u\n", tag, i,
               (unsigned int)cfg->dp_code_info.down_cmd_list.cmd_lists[i].length);
        dump_array("cfg.down.values",
                   cfg->dp_code_info.down_cmd_list.cmd_lists[i].values,
                   cfg->dp_code_info.down_cmd_list.cmd_lists[i].length);
    }
    dump_array("cfg.uin", cfg->uin_monitor_list.values,
               cfg->uin_monitor_list.length);
}

void run_full(const Api &api) {
    if (getenv("DPCFG_ORACLE_ISOLATED") == NULL) {
        fail("full mode requires DPCFG_ORACLE_ISOLATED=1 (private /tmp)");
    }

    prepare_shared_files();

    const int init_ret = api.init_dp_config();
    printf("RESULT init.ret=%d\n", init_ret);
    dump_state(api, "after_init");

    tss_dp_comm::DP_CONFIG *head = *api.glb_dp_config_head;
    if (head == NULL) {
        return;
    }

    head->update_time = 0x22222222u;
    const int reload_ret = api.reload_dp_config();
    printf("RESULT reload.ret=%d\n", reload_ret);
    dump_state(api, "after_reload");

    // Parse failure path: the mapped config must stay unchanged while both
    // timestamp fields still advance.
    unlink("/tmp/dpcfg_oracle_missing.xml");
    strncpy(head->dp_config_path, "/tmp/dpcfg_oracle_missing.xml", 255);
    head->dp_config_path[255] = '\0';
    head->update_time = 0x33333333u;
    const int fail_ret = api.reload_dp_config();
    printf("RESULT reload_fail.ret=%d\n", fail_ret);
    dump_state(api, "after_reload_fail");

    // Restore the path and reload again: the config must be rewritten.
    strncpy(head->dp_config_path, "/tmp/dpcfg_oracle.xml", 255);
    head->dp_config_path[255] = '\0';
    head->update_time = 0x44444444u;
    const int reload2_ret = api.reload_dp_config();
    printf("RESULT reload2.ret=%d\n", reload2_ret);
    dump_state(api, "after_reload2");

    const int noop_ret = api.reload_dp_config();
    printf("RESULT reload_noop.ret=%d\n", noop_ret);

    const int init2_ret = api.init_dp_config();
    printf("RESULT init2.ret=%d\n", init2_ret);
}

}  // namespace

int main(int argc, char **argv) {
    const char *lib = NULL;
    const char *mode = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--lib") == 0 && i + 1 < argc) {
            lib = argv[++i];
        } else if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) {
            mode = argv[++i];
        }
    }
    if (lib == NULL || mode == NULL) {
        fprintf(stderr, "usage: %s --lib <so> --mode pure|full\n", argv[0]);
        return 2;
    }

    Api api;
    load_api(lib, &api);

    if (strcmp(mode, "pure") == 0) {
        run_pure(api);
    } else if (strcmp(mode, "full") == 0) {
        run_full(api);
    } else {
        fprintf(stderr, "unknown mode %s\n", mode);
        return 2;
    }

    dlclose(api.handle);
    return 0;
}
