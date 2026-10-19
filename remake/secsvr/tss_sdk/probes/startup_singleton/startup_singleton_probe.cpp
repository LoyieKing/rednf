// Isolated dlopen probe for the startup singleton/lifecycle P1s.
//
// It loads exactly one library per process (the frozen ORIG or a rebuilt
// libtss_sdk.so.1.0.394), lets its ELF constructors run, then inspects:
//
//   1. Singleton<tss_sdk_impl::TssSdkPluginMgr>::instance()
//      - symbol exists, is a function, returns the address of the exported
//        function-local-storage symbol, guard is non-zero, storage size is
//        0x38 (56) as recorded in the ELF symbol table;
//      - registered_plugin_count_ == 6;
//      - total_plugin_table_ has 8 slots, ids 1..6 hold non-null plugin
//        pointers whose +0x0c id equals the slot index, slot 7 (DataTrans) is
//        null, every activation flag is false;
//      - invalid/repeated/active id vectors are empty.
//
//   2. Singleton<tss_sdk_impl::TssSdkCommSvc>::instance()
//      - same root/storage checks with size 0x28 (40);
//      - channel_id_ == 0, plugin_mgr_ == the manager singleton, dp_if_ != 0
//        (DPInterface::instance() was obtained), error_info_ != 0 (the TLS
//        error block was allocated), all six channel counters are zero.
//
//   3. object_creator pre-creation roots for both types exist, their guard
//      bytes are non-zero after dlopen (their constructors call instance()),
//      and the object_creator storage is one byte.
//
// Mutation sensitivity is explicit: --expect=no-roots,
// --expect=one-byte-manager and --expect=no-object-creator make the probe exit
// 0 only when it actually observes the corresponding defect.  In normal
// (--expect=none) mode any of those defects is a failure.
//
// The probe never includes a recon header and never links the library.  It
// resolves the few real symbols it calls (the two instance() functions, the
// manager count getter and ITssSdkPlugin::get_id) with dlsym; all class-field
// offsets are read from the documented 56/40-byte layouts.  If the manager
// storage symbol is not 0x38 bytes the probe stops before dereferencing any
// manager field, so a one-byte mutant cannot cause an out-of-bounds read.
//
// Build/run commands: see build_and_run.sh in this directory.

#include <dlfcn.h>
#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

namespace {

const char *kManagerInstance =
    "_ZN9SingletonIN12tss_sdk_impl15TssSdkPluginMgrEE8instanceEv";
const char *kManagerObj =
    "_ZZN9SingletonIN12tss_sdk_impl15TssSdkPluginMgrEE8instanceEvE3obj";
const char *kManagerObjGuard =
    "_ZGVZN9SingletonIN12tss_sdk_impl15TssSdkPluginMgrEE8instanceEvE3obj";
const char *kManagerCreator =
    "_ZN9SingletonIN12tss_sdk_impl15TssSdkPluginMgrEE14object_creatorE";
const char *kManagerCreatorGuard =
    "_ZGVN9SingletonIN12tss_sdk_impl15TssSdkPluginMgrEE14object_creatorE";

const char *kCommSvcInstance =
    "_ZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEv";
const char *kCommSvcObj =
    "_ZZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEvE3obj";
const char *kCommSvcObjGuard =
    "_ZGVZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE8instanceEvE3obj";
const char *kCommSvcCreator =
    "_ZN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE14object_creatorE";
const char *kCommSvcCreatorGuard =
    "_ZGVN9SingletonIN12tss_sdk_impl13TssSdkCommSvcEE14object_creatorE";

const char *kManagerCount =
    "_ZN12tss_sdk_impl15TssSdkPluginMgr27get_registered_plugin_countEv";
const char *kPluginGetId = "_ZN12tss_sdk_impl13ITssSdkPlugin6get_idEv";

const uint32_t kManagerSize = 0x38;
const uint32_t kCommSvcSize = 0x28;


// i386 cdecl member-function ABI: `this` is the first stack argument.  Both
// getters are non-virtual direct symbols (ORIG 0x00097990 and 0x00097120).
typedef unsigned int (*GetRegisteredCountFn)(void *self);
typedef int (*GetIdFn)(void *self);
typedef void *(*InstanceFn)();

enum Expect {
    EXPECT_NONE = 0,
    EXPECT_NO_ROOTS,
    EXPECT_ONE_BYTE_MANAGER,
    EXPECT_NO_OBJECT_CREATOR
};

struct SymbolInfo {
    bool found;
    uint32_t value;
    uint32_t size;
    unsigned char bind;
    unsigned char type;
    unsigned char visibility;
};

struct Defect {
    std::string name;
    std::string detail;
};

class Reporter {
public:
    void add(const char *name, const char *detail) {
        Defect d;
        d.name = name;
        d.detail = detail ? detail : "";
        defects_.push_back(d);
    }

    bool has(const char *name) const {
        for (size_t i = 0; i < defects_.size(); ++i) {
            if (defects_[i].name == name) return true;
        }
        return false;
    }

    size_t count() const { return defects_.size(); }

    void print_json(FILE *out) const {
        fputs("  \"defects\": [", out);
        for (size_t i = 0; i < defects_.size(); ++i) {
            if (i) fputs(", ", out);
            fprintf(out, "{\"name\":\"%s\",\"detail\":\"%s\"}",
                    json_escape(defects_[i].name).c_str(),
                    json_escape(defects_[i].detail).c_str());
        }
        fputs("]", out);
    }

    static std::string json_escape(const std::string &in) {
        std::string out;
        for (size_t i = 0; i < in.size(); ++i) {
            char c = in[i];
            if (c == '"' || c == '\\') {
                out.push_back('\\');
                out.push_back(c);
            } else if ((unsigned char)c < 0x20) {
                char buf[8];
                snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                out += buf;
            } else {
                out.push_back(c);
            }
        }
        return out;
    }

private:
    std::vector<Defect> defects_;
};

// ---------------------------------------------------------------------------
// Minimal ELF32 symbol lookup (little-endian, SHT_SYMTAB and SHT_DYNSYM).
// ---------------------------------------------------------------------------

class ElfSymbols {
public:
    explicit ElfSymbols(const char *path) : data_(0), size_(0) {
        FILE *f = fopen(path, "rb");
        if (!f) return;
        if (fseek(f, 0, SEEK_END) != 0) {
            fclose(f);
            return;
        }
        long end = ftell(f);
        if (end <= 0) {
            fclose(f);
            return;
        }
        size_ = (size_t)end;
        data_ = (unsigned char *)malloc(size_);
        if (!data_) {
            fclose(f);
            return;
        }
        if (fseek(f, 0, SEEK_SET) != 0 ||
            fread(data_, 1, size_, f) != size_) {
            free(data_);
            data_ = 0;
            size_ = 0;
        }
        fclose(f);
    }

    ~ElfSymbols() { free(data_); }

    bool ok() const {
        if (!data_ || size_ < sizeof(Elf32_Ehdr)) return false;
        if (memcmp(data_, ELFMAG, SELFMAG) != 0) return false;
        if (data_[EI_CLASS] != ELFCLASS32) return false;
        if (data_[EI_DATA] != ELFDATA2LSB) return false;
        return true;
    }

    SymbolInfo find(const char *name) const {
        SymbolInfo result;
        memset(&result, 0, sizeof(result));
        if (!ok()) return result;

        const Elf32_Ehdr *eh = (const Elf32_Ehdr *)data_;
        if (eh->e_shentsize != sizeof(Elf32_Shdr)) return result;
        if (eh->e_shoff == 0 || eh->e_shoff > size_) return result;

        uint32_t shnum = eh->e_shnum;
        if (shnum == 0) {
            const Elf32_Shdr *sh0 =
                (const Elf32_Shdr *)(data_ + eh->e_shoff);
            shnum = sh0->sh_size;
        }
        if ((size_t)eh->e_shoff +
                (size_t)shnum * sizeof(Elf32_Shdr) > size_)
            return result;
        if (shnum > 0x10000) return result;

        // Prefer .symtab (it also carries local symbols); fall back to
        // .dynsym for stripped inputs.
        static const uint32_t kKinds[] = {SHT_SYMTAB, SHT_DYNSYM};
        for (size_t k = 0; k < sizeof(kKinds) / sizeof(kKinds[0]); ++k) {
            for (uint32_t i = 0; i < shnum; ++i) {
                const Elf32_Shdr *sh =
                    (const Elf32_Shdr *)(data_ + eh->e_shoff +
                                        (size_t)i * sizeof(Elf32_Shdr));
                if (sh->sh_type != kKinds[k]) continue;
                if (sh->sh_offset > size_ || sh->sh_size > size_ - sh->sh_offset)
                    continue;
                if (sh->sh_entsize != sizeof(Elf32_Sym)) continue;
                if (sh->sh_link >= shnum) continue;

                const Elf32_Shdr *strsh =
                    (const Elf32_Shdr *)(data_ + eh->e_shoff +
                                        (size_t)sh->sh_link * sizeof(Elf32_Shdr));
                if (strsh->sh_offset > size_ ||
                    strsh->sh_size > size_ - strsh->sh_offset)
                    continue;
                const char *strtab = (const char *)(data_ + strsh->sh_offset);
                uint32_t count = sh->sh_size / sizeof(Elf32_Sym);
                const Elf32_Sym *syms =
                    (const Elf32_Sym *)(data_ + sh->sh_offset);
                for (uint32_t j = 0; j < count; ++j) {
                    if (syms[j].st_name >= strsh->sh_size) continue;
                    if (strcmp(strtab + syms[j].st_name, name) != 0) continue;
                    result.found = true;
                    result.value = syms[j].st_value;
                    result.size = syms[j].st_size;
                    result.bind = ELF32_ST_BIND(syms[j].st_info);
                    result.type = ELF32_ST_TYPE(syms[j].st_info);
                    result.visibility = ELF32_ST_VISIBILITY(syms[j].st_other);
                    return result;
                }
            }
        }
        return result;
    }

private:
    unsigned char *data_;
    size_t size_;
};

// ---------------------------------------------------------------------------
// Layout readers.
// ---------------------------------------------------------------------------

uint32_t load_u32(const void *base, uint32_t offset) {
    uint32_t value;
    memcpy(&value, (const unsigned char *)base + offset, sizeof(value));
    return value;
}

bool load_bool_byte(const void *base, uint32_t offset) {
    return *((const unsigned char *)base + offset) != 0;
}

struct VectorView {
    uint32_t begin;
    uint32_t end;
    uint32_t cap;
};

VectorView load_vector(const void *base, uint32_t offset) {
    VectorView v;
    v.begin = load_u32(base, offset + 0);
    v.end = load_u32(base, offset + 4);
    v.cap = load_u32(base, offset + 8);
    return v;
}

std::string hex32(uint32_t value) {
    char buf[16];
    snprintf(buf, sizeof(buf), "0x%08x", value);
    return buf;
}

std::string hex_ptr(const void *value) {
    return hex32((uint32_t)(uintptr_t)value);
}

void print_vector_json(FILE *out, const char *name, const VectorView &v) {
    fprintf(out, "{\"name\":\"%s\",\"begin\":\"%s\",\"end\":\"%s\","
                 "\"cap\":\"%s\",\"bytes\":%u}",
            name, hex32(v.begin).c_str(), hex32(v.end).c_str(),
            hex32(v.cap).c_str(),
            (unsigned)(v.end >= v.begin ? v.end - v.begin : 0));
}

// ---------------------------------------------------------------------------
// Probe.
// ---------------------------------------------------------------------------


const char *expect_name(Expect expect);

int run(const char *library, Expect expect) {
    Reporter rep;
    ElfSymbols elf(library);
    if (!elf.ok()) {
        fprintf(stderr, "not a 32-bit little-endian ELF: %s\n", library);
        return 2;
    }

    SymbolInfo mgr_fn = elf.find(kManagerInstance);
    SymbolInfo mgr_obj = elf.find(kManagerObj);
    SymbolInfo mgr_guard = elf.find(kManagerObjGuard);
    SymbolInfo mgr_creator = elf.find(kManagerCreator);
    SymbolInfo mgr_creator_guard = elf.find(kManagerCreatorGuard);
    SymbolInfo svc_fn = elf.find(kCommSvcInstance);
    SymbolInfo svc_obj = elf.find(kCommSvcObj);
    SymbolInfo svc_guard = elf.find(kCommSvcObjGuard);
    SymbolInfo svc_creator = elf.find(kCommSvcCreator);
    SymbolInfo svc_creator_guard = elf.find(kCommSvcCreatorGuard);

    void *handle = dlopen(library, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return 2;
    }

    void *mgr_fn_addr = dlsym(handle, kManagerInstance);
    void *mgr_obj_addr = dlsym(handle, kManagerObj);
    void *mgr_guard_addr = dlsym(handle, kManagerObjGuard);
    void *mgr_creator_addr = dlsym(handle, kManagerCreator);
    void *mgr_creator_guard_addr = dlsym(handle, kManagerCreatorGuard);
    void *svc_fn_addr = dlsym(handle, kCommSvcInstance);
    void *svc_obj_addr = dlsym(handle, kCommSvcObj);
    void *svc_guard_addr = dlsym(handle, kCommSvcObjGuard);
    void *svc_creator_addr = dlsym(handle, kCommSvcCreator);
    void *svc_creator_guard_addr = dlsym(handle, kCommSvcCreatorGuard);
    GetRegisteredCountFn count_fn =
        (GetRegisteredCountFn)dlsym(handle, kManagerCount);
    GetIdFn get_id_fn = (GetIdFn)dlsym(handle, kPluginGetId);

    if (!mgr_fn_addr) rep.add("dlopen-missing-manager-instance", dlerror());
    if (!svc_fn_addr) rep.add("dlopen-missing-commsvc-instance", dlerror());
    if (!mgr_obj_addr) rep.add("dlopen-missing-manager-storage", "");
    if (!svc_obj_addr) rep.add("dlopen-missing-commsvc-storage", "");
    if (!mgr_creator_addr) rep.add("dlopen-missing-manager-object-creator", "");
    if (!svc_creator_addr) rep.add("dlopen-missing-commsvc-object-creator", "");
    if (!mgr_guard_addr) rep.add("dlopen-missing-manager-guard", "");
    if (!svc_guard_addr) rep.add("dlopen-missing-commsvc-guard", "");
    if (!mgr_creator_guard_addr)
        rep.add("dlopen-missing-manager-object-creator-guard", "");
    if (!svc_creator_guard_addr)
        rep.add("dlopen-missing-commsvc-object-creator-guard", "");
    if (!count_fn) rep.add("dlopen-missing-manager-count-getter", "");
    if (!get_id_fn) rep.add("dlopen-missing-plugin-get-id-getter", "");

    void *manager = 0;
    void *comm_svc = 0;
    if (mgr_fn_addr) manager = ((InstanceFn)mgr_fn_addr)();
    if (svc_fn_addr) comm_svc = ((InstanceFn)svc_fn_addr)();

    bool manager_safe = mgr_obj.found && mgr_obj.size == kManagerSize && manager;
    bool commsvc_safe = svc_obj.found && svc_obj.size == kCommSvcSize && comm_svc;

    // Root/storage checks.
    if (mgr_fn_addr || mgr_obj_addr) {
        if (!mgr_fn.found) rep.add("missing-instance-symbol", "manager");
        if (!mgr_obj.found) rep.add("missing-instance-storage", "manager");
        if (!mgr_guard.found) rep.add("missing-instance-guard", "manager");
        if (!mgr_creator.found)
            rep.add("missing-object-creator", "manager");
        if (!mgr_creator_guard.found)
            rep.add("missing-object-creator-guard", "manager");
        if (mgr_obj.found && mgr_obj.size != kManagerSize)
            rep.add("wrong-instance-storage-size", "manager");
        if (mgr_creator.found && mgr_creator.size != 1)
            rep.add("wrong-object-creator-size", "manager");
        if (manager && mgr_obj_addr && manager != mgr_obj_addr)
            rep.add("instance-does-not-return-declared-storage", "manager");
        if (mgr_guard_addr && *(const unsigned char *)mgr_guard_addr == 0)
            rep.add("instance-guard-not-set", "manager");
        if (mgr_creator_guard_addr &&
            *(const unsigned char *)mgr_creator_guard_addr == 0)
            rep.add("object-creator-guard-not-set", "manager");
    }
    if (svc_fn_addr || svc_obj_addr) {
        if (!svc_fn.found) rep.add("missing-instance-symbol", "comm_svc");
        if (!svc_obj.found) rep.add("missing-instance-storage", "comm_svc");
        if (!svc_guard.found) rep.add("missing-instance-guard", "comm_svc");
        if (!svc_creator.found) rep.add("missing-object-creator", "comm_svc");
        if (!svc_creator_guard.found)
            rep.add("missing-object-creator-guard", "comm_svc");
        if (svc_obj.found && svc_obj.size != kCommSvcSize)
            rep.add("wrong-instance-storage-size", "comm_svc");
        if (svc_creator.found && svc_creator.size != 1)
            rep.add("wrong-object-creator-size", "comm_svc");
        if (comm_svc && svc_obj_addr && comm_svc != svc_obj_addr)
            rep.add("instance-does-not-return-declared-storage", "comm_svc");
        if (svc_guard_addr && *(const unsigned char *)svc_guard_addr == 0)
            rep.add("instance-guard-not-set", "comm_svc");
        if (svc_creator_guard_addr &&
            *(const unsigned char *)svc_creator_guard_addr == 0)
            rep.add("object-creator-guard-not-set", "comm_svc");
    }

    // Manager fields, only when the storage really is 56 bytes.
    uint32_t registered_count = 0;
    uint32_t getter_count = 0;
    uint32_t table_bytes = 0;
    uint32_t slot_ids[8];
    uint32_t slot_ptrs[8];
    int slot_getter_ids[8];
    unsigned char slot_active[8];
    VectorView table;
    VectorView invalid_ids;
    VectorView repeated_ids;
    VectorView active_ids;
    for (int i = 0; i < 8; ++i) {
        slot_ids[i] = 0;
        slot_ptrs[i] = 0;
        slot_active[i] = 0;
        slot_getter_ids[i] = -1;
    }
    memset(&table, 0, sizeof(table));
    memset(&invalid_ids, 0, sizeof(invalid_ids));
    memset(&repeated_ids, 0, sizeof(repeated_ids));
    memset(&active_ids, 0, sizeof(active_ids));

    if (manager_safe) {
        registered_count = load_u32(manager, 0x00);
        invalid_ids = load_vector(manager, 0x04);
        repeated_ids = load_vector(manager, 0x10);
        active_ids = load_vector(manager, 0x1c);
        table = load_vector(manager, 0x28);
        table_bytes = table.end >= table.begin ? table.end - table.begin : 0;
        if (count_fn) getter_count = count_fn(manager);

        if (registered_count != 6)
            rep.add("wrong-registered-plugin-count", "expected 6");
        if (count_fn && getter_count != registered_count)
            rep.add("count-getter-mismatch", "getter != field");
        if (table_bytes != 8 * sizeof(uint32_t) * 2)
            rep.add("wrong-total-plugin-table-size", "expected 8 entries");
        if (table_bytes == 8 * 8 && table.begin != 0) {
            for (uint32_t id = 0; id < 8; ++id) {
                const unsigned char *slot =
                    (const unsigned char *)(uintptr_t)table.begin + id * 8;
                slot_ptrs[id] = load_u32(slot, 0);
                slot_active[id] = load_bool_byte(slot, 4);
                if (slot_ptrs[id]) {
                    slot_ids[id] = load_u32((const void *)(uintptr_t)slot_ptrs[id],
                                            0x0c);
                    if (get_id_fn)
                        slot_getter_ids[id] =
                            get_id_fn((void *)(uintptr_t)slot_ptrs[id]);
                }
            }
            for (uint32_t id = 1; id <= 6; ++id) {
                if (!slot_ptrs[id]) {
                    rep.add("missing-plugin-root", hex32(id).c_str());
                } else if (slot_ids[id] != id) {
                    rep.add("plugin-id-mismatch", hex32(id).c_str());
                }
                if (get_id_fn && slot_getter_ids[id] != (int)id)
                    rep.add("plugin-get-id-mismatch", hex32(id).c_str());
                if (slot_active[id]) {
                    rep.add("plugin-activated-during-static-init",
                            hex32(id).c_str());
                }
            }
            if (slot_ptrs[0] != 0) rep.add("plugin-slot-0-not-null", "");
            if (slot_ptrs[7] != 0) rep.add("datatrans-slot-not-null", "");
            if (slot_active[7]) rep.add("datatrans-slot-active", "");
        }
        if (invalid_ids.end != invalid_ids.begin)
            rep.add("invalid-id-vector-not-empty", "");
        if (repeated_ids.end != repeated_ids.begin)
            rep.add("repeated-id-vector-not-empty", "");
        if (active_ids.end != active_ids.begin)
            rep.add("active-id-vector-not-empty", "");
    }

    // CommSvc fields, only when the storage really is 40 bytes.
    uint32_t channel_id = 0;
    uint32_t plugin_mgr_field = 0;
    uint32_t dp_if = 0;
    uint32_t error_info = 0;
    uint32_t channel_status[6];
    for (int i = 0; i < 6; ++i) channel_status[i] = 0;
    if (commsvc_safe) {
        channel_id = load_u32(comm_svc, 0x00);
        plugin_mgr_field = load_u32(comm_svc, 0x04);
        dp_if = load_u32(comm_svc, 0x08);
        for (int i = 0; i < 6; ++i)
            channel_status[i] = load_u32(comm_svc, 0x0c + i * 4);
        error_info = load_u32(comm_svc, 0x24);

        if (channel_id != 0) rep.add("commsvc-channel-id-not-zero", "");
        if (!manager || plugin_mgr_field != (uint32_t)(uintptr_t)manager)
            rep.add("commsvc-manager-not-bound", "");
        if (dp_if == 0) rep.add("commsvc-dpinterface-null", "");
        if (error_info == 0) rep.add("commsvc-error-info-null", "");
        for (int i = 0; i < 6; ++i) {
            if (channel_status[i] != 0) {
                rep.add("commsvc-counter-not-zero", "");
                break;
            }
        }
    }

    // Expected-defect modes: success means the named defect was observed.
    const char *result = "FAIL";
    bool observed = false;
    switch (expect) {
        case EXPECT_NONE:
            observed = true;
            break;
        case EXPECT_NO_ROOTS:
            observed = manager_safe && registered_count == 0;
            break;
        case EXPECT_ONE_BYTE_MANAGER:
            observed = !mgr_obj.found || mgr_obj.size != kManagerSize ||
                       (manager && mgr_obj_addr && manager != mgr_obj_addr);
            break;

        case EXPECT_NO_OBJECT_CREATOR:
            observed = rep.has("missing-object-creator") ||
                       rep.has("object-creator-guard-not-set");
            break;
    }

    bool normal_pass = rep.count() == 0;
    bool pass = (expect == EXPECT_NONE) ? normal_pass : observed;
    result = pass ? "PASS" : "FAIL";

    // JSON report.
    printf("{\n");
    printf("  \"library\": \"%s\",\n",
           Reporter::json_escape(library).c_str());
    printf("  \"expect\": \"%s\",\n", expect_name(expect));
    printf("  \"manager\": {\"instance\": \"%s\", \"obj\": \"%s\", "
           "\"instance_equals_obj\": %s, \"obj_symbol_size\": %u, "
           "\"obj_symbol_found\": %s, \"instance_symbol_found\": %s, "
           "\"guard_set\": %s, \"object_creator\": \"%s\", "
           "\"object_creator_found\": %s, \"object_creator_size\": %u, "
           "\"object_creator_guard_set\": %s, "
           "\"registered_plugin_count\": %u, \"getter_count\": %u, "
           "\"table_bytes\": %u, \"slots\": [",
           hex_ptr(manager).c_str(), hex_ptr(mgr_obj_addr).c_str(),
           (manager && mgr_obj_addr && manager == mgr_obj_addr) ? "true"
                                                               : "false",
           (unsigned)mgr_obj.size, mgr_obj.found ? "true" : "false",
           mgr_fn.found ? "true" : "false",
           (mgr_guard_addr && *(const unsigned char *)mgr_guard_addr) ? "true"
                                                                     : "false",
           hex_ptr(mgr_creator_addr).c_str(),
           mgr_creator.found ? "true" : "false",
           (unsigned)mgr_creator.size,
           (mgr_creator_guard_addr &&
            *(const unsigned char *)mgr_creator_guard_addr)
               ? "true"
               : "false",
           registered_count, getter_count, table_bytes);
    for (uint32_t id = 0; id < 8; ++id) {
        if (id) printf(", ");
        printf("{\"id\":%u,\"plugin\":\"%s\",\"plugin_id\":%u,"
               "\"getter_id\":%d,\"active\":%s}",
               id, hex32(slot_ptrs[id]).c_str(), slot_ids[id],
               slot_getter_ids[id], slot_active[id] ? "true" : "false");
    }
    printf("], \"invalid_ids\": ");
    print_vector_json(stdout, "invalid", invalid_ids);
    printf(", \"repeated_ids\": ");
    print_vector_json(stdout, "repeated", repeated_ids);
    printf(", \"active_ids\": ");
    print_vector_json(stdout, "active", active_ids);
    printf("},\n");

    printf("  \"comm_svc\": {\"instance\": \"%s\", \"obj\": \"%s\", "
           "\"instance_equals_obj\": %s, \"obj_symbol_size\": %u, "
           "\"obj_symbol_found\": %s, \"instance_symbol_found\": %s, "
           "\"guard_set\": %s, \"object_creator\": \"%s\", "
           "\"object_creator_found\": %s, \"object_creator_size\": %u, "
           "\"object_creator_guard_set\": %s, \"channel_id\": %u, "
           "\"plugin_mgr\": \"%s\", \"plugin_mgr_equals_manager\": %s, "
           "\"dp_if\": \"%s\", \"error_info\": \"%s\", "
           "\"channel_status\": [%u,%u,%u,%u,%u,%u]},\n",
           hex_ptr(comm_svc).c_str(), hex_ptr(svc_obj_addr).c_str(),
           (comm_svc && svc_obj_addr && comm_svc == svc_obj_addr) ? "true"
                                                                 : "false",
           (unsigned)svc_obj.size, svc_obj.found ? "true" : "false",
           svc_fn.found ? "true" : "false",
           (svc_guard_addr && *(const unsigned char *)svc_guard_addr) ? "true"
                                                                      : "false",
           hex_ptr(svc_creator_addr).c_str(),
           svc_creator.found ? "true" : "false",
           (unsigned)svc_creator.size,
           (svc_creator_guard_addr &&
            *(const unsigned char *)svc_creator_guard_addr)
               ? "true"
               : "false",
           channel_id, hex32(plugin_mgr_field).c_str(),
           (manager &&
            plugin_mgr_field == (uint32_t)(uintptr_t)manager)
               ? "true"
               : "false",
           hex32(dp_if).c_str(), hex32(error_info).c_str(),
           channel_status[0], channel_status[1], channel_status[2],
           channel_status[3], channel_status[4], channel_status[5]);

    rep.print_json(stdout);
    printf(",\n  \"defect_mode_observed\": %s,\n",
           observed ? "true" : "false");
    printf("  \"result\": \"%s\"\n}\n", result);
    fflush(stdout);

    dlerror();
    dlclose(handle);
    return pass ? 0 : 1;
}

const char *expect_name(Expect expect) {
    switch (expect) {
        case EXPECT_NO_ROOTS: return "no-roots";
        case EXPECT_ONE_BYTE_MANAGER: return "one-byte-manager";
        case EXPECT_NO_OBJECT_CREATOR: return "no-object-creator";
        case EXPECT_NONE:
        default: return "none";
    }
}

Expect parse_expect(const char *value) {
    if (strcmp(value, "none") == 0) return EXPECT_NONE;
    if (strcmp(value, "no-roots") == 0) return EXPECT_NO_ROOTS;
    if (strcmp(value, "one-byte-manager") == 0) return EXPECT_ONE_BYTE_MANAGER;
    if (strcmp(value, "no-object-creator") == 0)
        return EXPECT_NO_OBJECT_CREATOR;
    return EXPECT_NONE;
}

}  // namespace

int main(int argc, char **argv) {
    const char *library = 0;
    Expect expect = EXPECT_NONE;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--lib") == 0 && i + 1 < argc) {
            library = argv[++i];
        } else if (strcmp(argv[i], "--expect") == 0 && i + 1 < argc) {
            expect = parse_expect(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("usage: %s --lib PATH "
                   "[--expect none|no-roots|one-byte-manager|"
                   "no-object-creator]\n",
                   argv[0]);
            return 0;
        } else {
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
            return 2;
        }
    }

    if (!library) {
        fprintf(stderr, "--lib PATH is required\n");
        return 2;
    }
    return run(library, expect);
}
