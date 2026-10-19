#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Acceptance gate for the ORIG-identical layout/static-init reconstruction.

Reads audit/report and prints PASS/FAIL per category:

  types      no hard size/member deviation outside the documented compiler
             boundary (std::*, __gnu_cxx::*: GCC 4.1 vs GCC 13 libstdc++)
  odr        no type with two different layouts inside the reconstruction
  funcs      no signature mismatch outside the compiler boundary
  objects    no OBJECT/STT data symbol size mismatch
  statics    every ORIG namespace-scope object exists somewhere in the build
             (mangled linkage names must match exactly; internal names must
             match name+size or name+type, never name alone)
  init       every ORIG static-init block has reconstructed constructor /
             destructor / atexit effects in its primary reconstructed block.
             A static object whose name exists but is never constructed is a
             failure.  A combined reconstruction TU must carry one TU-local
             COMM_STAT_ITEM_WITH_NAME[16] / __ioinit per ORIG CU; one array
             cannot cover several ORIG CUs.  Split shared singletons are
             reported as evidence, not used to skip the CU.
  funcs-cover  informational: ORIG functions still absent, classified

The previous gate skipped a whole ORIG CU when every namespace-scope object
name existed somewhere in the build.  That accepted an object whose symbol was
emitted but whose constructor was never called, and it skipped CU/effect
comparison entirely.  This version has no name-existence skip.

Vendor-token exemptions are per-diff: one std:: member change no longer hides
an unrelated business-member offset/size change in the same type.

Usage: accept.py [--strict-missing-funcs]
Exit code 0 only when every hard category passes.
"""
from __future__ import print_function

import json
import re
import sys
from collections import Counter, defaultdict

BOUNDARY = ("std::", "__gnu_cxx::", "rapidxml::")
# Documented compiler/libstdc++/libc boundary types: the ORIG layout embeds a
# GCC 4.1 libstdc++/libc object whose DWARF shape GCC 13 cannot reproduce
# (std::ofstream tail; pthread_mutex_t/rwlock_t internal union; glibc
# fpos_t/dirent).  These types are never serialized by value across the
# reconstruction boundary; their sizes are asserted where used.
COMPILER_BOUNDARY_TYPES = {
    "ZEN_Trace_LogMsg",
    "ZEN_LogTrace_Basic",
    "ZEN_Thread_RW_Mutex",
    "_G_fpos_t",
    "_G_fpos64_t",
    "dirent",
    "ipc_perm",
    "re_pattern_buffer",
}
# libstdc++ internal variables whose GCC 4.1/13 shape differs: the prime
# table has 28 entries in GCC 4.1 and 29 in GCC 13.  This is not a business
# object and cannot be reconstructed byte-for-byte.
STATICS_BOUNDARY_NAMES = {"__stl_prime_list"}
# Tokens that identify GCC/libstdc++/libc implementation shape.  They are used
# only to classify an individual diff, never to excuse a whole type row.
VENDOR_TOKENS = ("std::", "__gnu_cxx::", "rapidxml::", "re_pattern_buffer",
                 "ipc_perm", "des_ks_struct", "_Rb_tree", "_Hashtable",
                 "_M_", "basic_string", "allocator",
                 "__pthread", "pthread_rwlock")


# Static-init effects are split into three groups.  They must not be collapsed:
#   * business calls      -- constructors/functions that create or register a
#                            named object.  Presence must be proved by a call
#                            in a reconstructed init block.
#   * compiler control    -- guard/unwind/allocation; code shape only.
#   * boilerplate/atexit  -- per-TU status array, __ioinit and destructor
#                            registrations; compared by count and data.
STATUS_CTOR = "ZEN_STATUS_ITEM_WITHNAME::ZEN_STATUS_ITEM_WITHNAME"
STATUS_DTOR = "ZEN_STATUS_ITEM_WITHNAME::~ZEN_STATUS_ITEM_WITHNAME"
IOS_INIT = "std::ios_base::Init::Init"
BOILERPLATE_CALLS = {STATUS_CTOR, STATUS_DTOR, IOS_INIT}
COMPILER_CONTROL_CALLS = {
    "__cxa_guard_acquire", "__cxa_guard_release", "__cxa_guard_abort",
    "_Unwind_Resume", "operator new", "operator delete",
}
LITERAL_RE = re.compile(r"^[A-Z][A-Z0-9_]{3,}$")
ANON_RE = re.compile(r"anon\d+")


def _texts(value):
    """Yield every string that can identify a compared member/diff."""
    if isinstance(value, str):
        yield value
    elif isinstance(value, dict):
        for key in ("path", "lkey", "skey", "name", "pretty", "type"):
            item = value.get(key)
            if isinstance(item, str):
                yield item
    elif isinstance(value, (list, tuple)):
        for item in value:
            for text in _texts(item):
                yield text


def has_vendor_token(text):
    if not text:
        return False
    return text in COMPILER_BOUNDARY_TYPES or any(t in text
                                                  for t in VENDOR_TOKENS)


def diff_is_vendor_only(diff):
    """True only when every identifier in this one diff is vendor-boundary.

    A path named `cfg_` whose member type is std::map still carries the
    std::map lkey in the member record, so the lkey/type text is inspected as
    well as the path.  A bare business path with no vendor text is business.
    """
    if diff.get("kind") == "signature":
        # Signatures are compared position-by-position by
        # signature_has_business_diff(); a whole-diff token test would excuse a
        # changed business parameter merely because another parameter is std::.
        return False
    path_texts = [t for t in _texts(diff.get("path")) if t]
    value_texts = []
    for key in ("lkey", "skey"):
        value_texts.extend(_texts(diff.get(key)))
    value_texts.extend(_texts(diff.get("orig")))
    value_texts.extend(_texts(diff.get("ours")))
    value_texts = [t for t in value_texts if t]
    texts = path_texts + value_texts
    if not texts:
        return False
    if not any(has_vendor_token(t) for t in texts):
        return False
    # A vendor-typed member may have a plain business field name (e.g.
    # `cfg_` whose type is std::map).  The value/type side carries the vendor
    # identity, so the path alone must not turn the diff into business.
    if value_texts and all(has_vendor_token(t) for t in value_texts):
        return True
    return all(has_vendor_token(t) for t in texts)


def diff_is_boundary(diffs):
    """A row is boundary-only only if every diff is boundary-only.

    The old implementation returned True when *any* token occurred anywhere in
    the JSON blob, which let one std:: member hide a business-member change.
    """
    return bool(diffs) and all(diff_is_vendor_only(d) for d in diffs)


def boundary(name):
    return name.startswith(BOUNDARY) or name in COMPILER_BOUNDARY_TYPES


def const_name(call):
    return call.split("(")[0]




# ---------------------------------------------------------------------------
# signature comparison
# ---------------------------------------------------------------------------

def canon_lkey(lkey, types):
    if lkey.startswith("agg:"):
        rec = types.get(lkey[4:])
        return "anon%d" % (rec["size"] if rec else 0)
    return re.sub(r"(?:^|::)\._\d+(?=::)", "", lkey)


def vendor_equivalent_lkey(left, right):
    """True when two vendor-boundary layout keys differ only in GCC shape."""
    if left == right:
        return True
    if not (has_vendor_token(left) and has_vendor_token(right)):
        return False
    # by-value aggregate sizes (anonN) are the libstdc++ ABI boundary; the
    # vendor type identity itself must still match after stripping the size.
    return ANON_RE.sub("anon", left) == ANON_RE.sub("anon", right)


def signature_has_business_diff(row, orig_funcs, ours_funcs, otypes, utypes):
    """Position-by-position signature check with per-parameter exemptions."""
    a = orig_funcs.get(row.get("mangled"))
    b = ours_funcs.get(row.get("mangled"))
    if a is None or b is None:
        return True
    al = [canon_lkey(x, otypes) for x in a.get("param_lkeys") or []]
    bl = [canon_lkey(x, utypes) for x in b.get("param_lkeys") or []]
    if len(al) != len(bl):
        return True
    for x, y in zip(al, bl):
        if not vendor_equivalent_lkey(x, y):
            return True
    aret = canon_lkey(a.get("ret_lkey") or "", otypes)
    bret = canon_lkey(b.get("ret_lkey") or "", utypes)
    return not vendor_equivalent_lkey(aret, bret)


# ---------------------------------------------------------------------------
# static-init effect model
# ---------------------------------------------------------------------------

def basename(cu):
    return (cu or "").replace("\\", "/").rsplit("/", 1)[-1]


def _addr_set(model, refs):
    out = set()
    for addr in model.get("vars", {}):
        try:
            out.add(int(addr))
        except (TypeError, ValueError):
            pass
    for ref in refs:
        addr = ref.get("addr")
        if isinstance(addr, int):
            out.add(addr)
    for rec in model.get("objects", {}).values():
        addr = rec.get("addr")
        if isinstance(addr, int):
            out.add(addr)
    return out


def scalar_imms(imms, refs, model):
    """Non-address immediates (status counter ids, small enum args).

    Addresses appear as stack operands as well as values; filter every address
    that the ELF/DWARF model or this block resolves to a symbol/variable.
    Values below 100 are shared compiler constants (0/1/size) and are not
    useful for data-identity comparison.
    """
    addresses = _addr_set(model, refs)
    out = Counter()
    for value in imms:
        if isinstance(value, int) and 100 <= value < 0x100000 \
                and value not in addresses:
            out[value] += 1
    return out


def data_literals(refs):
    """Counter-name-like string literals referenced by an init block."""
    out = Counter()
    for ref in refs:
        text = ref.get("str")
        if not text or not LITERAL_RE.match(text):
            continue
        if "GCC" in text or "SUSE" in text or "Linux" in text:
            continue
        out[text] += 1
    return out


def canonical_business(counter):
    """Canonicalise code-shape variants of the same load-time pre-create.

    Evidence:
      * ORIG 0x000914f0 / 0x0008af00 and the reconstructed out-of-line
        Singleton<T>::instance() at build-server-dbg 0x000b27d0 / 0x000b2880
        perform guard -> T::T -> __cxa_atexit -> release.
      * reconstructed IDPHackerSafe::IDPHackerSafe() at 0x000d3be0 calls
        ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*) with 0, which is the
        member construction ORIG inlined into its block.
    These are entry-point/code-shape differences, not extra initialization.
    """
    out = Counter()
    for name, count in counter.items():
        if name.startswith("Singleton<") and name.endswith(">::instance"):
            key = "precreate:" + name[len("Singleton<"):-len(">::instance")]
        elif name.endswith("::TssSdkCommSvc"):
            key = "precreate:tss_sdk_impl::TssSdkCommSvc"
        elif name.endswith("::TssSdkPluginMgr"):
            key = "precreate:tss_sdk_impl::TssSdkPluginMgr"
        elif name.endswith("::IDPHackerSafe"):
            key = "precreate:ZEN_Thread_Mutex"
        elif name in ("ZEN_Thread_Mutex::ZEN_Thread_Mutex",
                      "ZEN_Thread_Mutex::ZEN_Thread_Mutex(char const*)"):
            key = "precreate:ZEN_Thread_Mutex"
        else:
            key = name
        out[key] += count
    return out


def profile_block(rows, model):
    calls = Counter()
    refs = []
    imms = []
    for row in rows:
        for call in row.get("calls", []):
            calls[const_name(call)] += 1
        refs.extend(row.get("refs", []))
        imms.extend(row.get("imms", []))
    business = Counter({k: v for k, v in calls.items()
                        if k not in BOILERPLATE_CALLS
                        and k not in COMPILER_CONTROL_CALLS
                        and k != "__cxa_atexit"})
    return {
        "calls": calls,
        "business": business,
        "canon_business": canonical_business(business),
        "status": calls[STATUS_CTOR],
        "dtor": calls[STATUS_DTOR],
        "ios": calls[IOS_INIT],
        "atexit": calls["__cxa_atexit"],
        "guard": calls["__cxa_guard_acquire"],
        "scalars": scalar_imms(imms, refs, model),
        "literals": data_literals(refs),
    }


def coverage_profile(profiles):
    """Coverage evidence without double-counting split slices.

    Counts are the maximum seen in one reconstructed block, not a sum: two
    slices that each construct their own 16-entry status array must not add up
    to 32 and hide a block that constructs none.  Counter keys are unioned so
    a business effect split across slices is still observable.
    """
    if not profiles:
        return profile_block([], {})
    merged = {
        "calls": Counter(), "business": Counter(),
        "canon_business": Counter(), "status": 0, "dtor": 0, "ios": 0,
        "atexit": 0, "guard": 0, "scalars": Counter(),
        "literals": Counter(),
    }
    for p in profiles:
        for key in ("calls", "business", "canon_business", "scalars",
                    "literals"):
            for name, count in p[key].items():
                merged[key][name] = max(merged[key].get(name, 0), count)
        for key in ("status", "dtor", "ios", "atexit", "guard"):
            merged[key] = max(merged[key], p[key])
    return merged


def _fmt_counter(counter, limit=6):
    return ", ".join("%s x%d" % (k, v) for k, v in
                     sorted(counter.items())[:limit])


def _missing_counter(have, need):
    return {k: v for k, v in need.items() if have.get(k, 0) < 1}


def evaluate_init(io, iu, orig, ours, cu_map=None):
    """Return (hard_gaps, soft_gaps, summary) for static initialization.

    Strict per-ORIG-CU criterion:
      1. every ORIG business call must run in a reconstructed init block;
      2. TU-local effects are compared only against the ORIG CU's primary
         reconstructed block.  A same-named object in another TU does not
         cover a `COMM_STAT_ITEM_WITH_NAME[16]` / `__ioinit` / atexit
         registration: those are independent per-CU objects, not shared
         singletons;
      3. a reconstructed file that combines several ORIG TUs must carry the
         sum of their independent arrays, not one shared array;
      4. if a TU emits an object but no same-TU init block constructs it, that
         is a hard failure (existence != initialization);
      5. a missing primary block is hard whenever the ORIG block has any
         dynamic-init effect.  It is reported, never skipped;
      6. the only accepted code-shape equivalences are the documented shared
         `Singleton<T>::instance()` / `IDPHackerSafe` member construction
         (see canonical_business()); those still require a real call in some
         reconstructed init block.
    """
    our_inits = defaultdict(list)
    for cu, rows in iu["groups"].items():
        our_inits[basename(cu)].extend(rows)

    # our CU -> ORIG CU(s), from shared function symbols.  A slice that is
    # mapped to several ORIG CUs can be the primary block for several of them;
    # choose one primary block per ORIG CU below and check the combined count.
    per_our = defaultdict(set)
    shared_counts = defaultdict(Counter)
    for mangled, rec in ours["funcs"].items():
        orec = orig["funcs"].get(mangled)
        if orec is not None:
            ocu = basename(orec["cu"])
            ucu = basename(rec["cu"])
            per_our[ucu].add(ocu)
            shared_counts[ocu][ucu] += 1
    strict = defaultdict(set)
    for ucu, ocus in per_our.items():
        for ocu in ocus:
            strict[ocu].add(ucu)
    if cu_map:
        for ucu, ocu in cu_map.items():
            strict[basename(ocu)].add(basename(ucu))
    orig_cu_names = {basename(cu) for cu in io["groups"]}
    for name in our_inits:
        if name in orig_cu_names:
            strict[name].add(name)

    orig_vars_by_cu = defaultdict(list)
    for lst in orig["vars"].values():
        for var in lst:
            orig_vars_by_cu[basename(var["cu"])].append(var)
    our_vars_by_cu = defaultdict(list)
    for lst in ours["vars"].values():
        for var in lst:
            our_vars_by_cu[basename(var["cu"])].append(var)

    block_profile_by_cu = {}
    for cu, rows in iu["groups"].items():
        cb = basename(cu)
        block_profile_by_cu[cb] = coverage_profile(
            [profile_block([row], ours) for row in rows])
    all_blocks = [(cb, None, prof)
                  for cb, prof in block_profile_by_cu.items()]

    orig_profile_by_cu = {basename(cu): profile_block(rows, orig)
                          for cu, rows in io["groups"].items()}

    def choose_primary(ob, orig_p):
        cands = [b for b in all_blocks if b[0] in strict.get(ob, ())]
        if not cands:
            return None

        def score(block):
            cb, _row, prof = block
            value = 10 * shared_counts.get(ob, {}).get(cb, 0)
            if cb == ob:
                value += 1000
            if set(prof["canon_business"]) & set(orig_p["canon_business"]):
                value += 100
            if orig_p["status"] and prof["status"]:
                value += 20
            if orig_p["ios"] and prof["ios"]:
                value += 20
            if orig_p["dtor"] and prof["dtor"]:
                value += 10
            return value

        return max(cands, key=score)

    primaries = {}
    for ocu, rows in io["groups"].items():
        ob = basename(ocu)
        primaries[ob] = choose_primary(ob, orig_profile_by_cu[ob])

    # A reconstructed file that is the primary block for several ORIG CUs must
    # contain one construction per CU.  Compare the sum, so one 16-entry
    # array cannot silently cover seven ORIG CUs.
    primary_ocus = defaultdict(set)
    for ob, block in primaries.items():
        if block is not None:
            primary_ocus[block[0]].add(ob)

    hard = []
    soft = []
    summary = {
        "orig_blocks": len(io["groups"]),
        "recon_blocks": sum(len(v) for v in our_inits.values()),
        "same_tu": 0,
        "no_same_tu": 0,
        "business_uncovered": 0,
        "status_gaps": 0,
        "ioinit_gaps": 0,
        "atexit_gaps": 0,
        "dtor_gaps": 0,
    }

    for cb, ocus in sorted(primary_ocus.items()):
        if len(ocus) <= 1:
            continue
        block_p = block_profile_by_cu[cb]
        req_status = sum(orig_profile_by_cu[o]["status"] for o in ocus)
        req_ios = sum(orig_profile_by_cu[o]["ios"] for o in ocus)
        req_dtor = sum(orig_profile_by_cu[o]["dtor"] for o in ocus)
        req_scalars = sum(sum(orig_profile_by_cu[o]["scalars"].values())
                          for o in ocus)
        names = ", ".join(sorted(ocus)[:8])
        if block_p["status"] < req_status:
            summary["status_gaps"] += 1
            hard.append(("block:" + cb,
                         "shared block COMM_STAT ctors %d < %d required by "
                         "%d ORIG CUs: %s" %
                         (block_p["status"], req_status, len(ocus), names)))
        if block_p["ios"] < req_ios:
            summary["ioinit_gaps"] += 1
            hard.append(("block:" + cb,
                         "shared block __ioinit ctors %d < %d required by "
                         "%d ORIG CUs: %s" %
                         (block_p["ios"], req_ios, len(ocus), names)))
        if block_p["dtor"] < req_dtor:
            summary["dtor_gaps"] += 1
            hard.append(("block:" + cb,
                         "shared block status dtor calls %d < %d required by "
                         "%d ORIG CUs: %s" %
                         (block_p["dtor"], req_dtor, len(ocus), names)))
        if req_scalars and sum(block_p["scalars"].values()) < req_scalars:
            hard.append(("block:" + cb,
                         "shared block status ids %d < %d required by "
                         "%d ORIG CUs: %s" %
                         (sum(block_p["scalars"].values()), req_scalars,
                          len(ocus), names)))
        req_atexit = sum(orig_profile_by_cu[o]["atexit"] for o in ocus)
        if block_p["atexit"] < req_atexit:
            soft.append(("block:" + cb,
                         "shared block __cxa_atexit %d < %d ORIG registrations "
                         "across %d CUs (guard/duplicate registrations may "
                         "collapse)" %
                         (block_p["atexit"], req_atexit, len(ocus))))

    for ocu, rows in sorted(io["groups"].items()):
        ob = basename(ocu)
        orig_p = orig_profile_by_cu[ob]
        block = primaries.get(ob)
        direct_cus = set(strict.get(ob, ()))
        effects = (sum(orig_p["business"].values()) + orig_p["status"] +
                   orig_p["ios"] + orig_p["atexit"] + orig_p["dtor"])
        if block is None:
            summary["no_same_tu"] += 1
            if effects:
                hard.append((ob, "no same-TU init block (mapped=%s; ORIG "
                                 "effects business=%d status=%d ios=%d "
                                 "atexit=%d dtor=%d)" %
                            (",".join(sorted(direct_cus)) or "<none>",
                             sum(orig_p["business"].values()),
                             orig_p["status"], orig_p["ios"],
                             orig_p["atexit"], orig_p["dtor"])))
            continue

        summary["same_tu"] += 1
        cb, _row, direct_p = block
        shared_only = len(primary_ocus.get(cb, ())) > 1

        split_business_found = set()
        # ---- business calls -------------------------------------------
        missing_business = _missing_counter(direct_p["canon_business"],
                                            orig_p["canon_business"])
        if missing_business:
            shared_missing = {k: v for k, v in missing_business.items()
                              if k.startswith("precreate:")}
            other_missing = {k: v for k, v in missing_business.items()
                             if not k.startswith("precreate:")}
            shared_found = {}
            other_found = {}
            for candidate in all_blocks:
                for key in missing_business:
                    if candidate[2]["canon_business"].get(key):
                        if key.startswith("precreate:"):
                            shared_found[key] = max(
                                shared_found.get(key, 0),
                                candidate[2]["canon_business"][key])
                        else:
                            other_found.setdefault(key, set()).add(
                                candidate[0])
            split_business_found = set(other_found)
            shared_uncovered = {k: v for k, v in shared_missing.items()
                                if k not in shared_found}
            other_uncovered = {k: v for k, v in other_missing.items()
                               if k not in other_found}
            if shared_found:
                soft.append((ob, "shared pre-create covered by split slice: "
                                 "%s" % _fmt_counter(Counter(shared_found))))
            if other_found:
                soft.append((ob, "business ctor/effect covered by split "
                                 "slice: %s (called in %s)" %
                            (_fmt_counter(Counter(
                                {k: 1 for k in other_found})),
                             ", ".join(sorted(set().union(
                                 *other_found.values()))[:6]))))
            if other_uncovered or shared_uncovered:
                summary["business_uncovered"] += 1
                combined = dict(other_uncovered)
                combined.update(shared_uncovered)
                hard.append((ob, "business call(s) not called in any "
                                 "reconstructed block: %s" %
                            _fmt_counter(Counter(combined))))

        # ---- TU-local constructors / registrations ---------------------
        if not shared_only:
            if orig_p["status"] and direct_p["status"] < orig_p["status"]:
                summary["status_gaps"] += 1
                hard.append((ob, "COMM_STAT_ITEM_WITH_NAME ctors %d/%d in "
                                 "primary block %s" %
                            (direct_p["status"], orig_p["status"], cb)))
            if orig_p["ios"] and direct_p["ios"] < orig_p["ios"]:
                summary["ioinit_gaps"] += 1
                hard.append((ob, "__ioinit ctors %d/%d in primary block %s" %
                            (direct_p["ios"], orig_p["ios"], cb)))
            if orig_p["dtor"] and direct_p["dtor"] < orig_p["dtor"]:
                summary["dtor_gaps"] += 1
                hard.append((ob, "status dtor calls %d/%d in primary block "
                                 "%s" %
                            (direct_p["dtor"], orig_p["dtor"], cb)))
            if orig_p["atexit"] and direct_p["atexit"] < orig_p["atexit"]:
                deferred = [k for k in orig_p["canon_business"]
                            if k.startswith("precreate:") and
                            direct_p["canon_business"].get(k)]
                missing_atexit = orig_p["atexit"] - direct_p["atexit"]
                split_allowed = min(missing_atexit,
                                    len(split_business_found))
                allowed = len(deferred) + split_allowed
                if direct_p["atexit"] + allowed >= orig_p["atexit"]:
                    detail = list(deferred)
                    if split_allowed:
                        detail.append("split-global:" + ",".join(
                            sorted(split_business_found)))
                    soft.append((ob, "__cxa_atexit %d/%d deferred into %s" %
                                (direct_p["atexit"], orig_p["atexit"],
                                 ", ".join(detail[:4]))))
                else:
                    summary["atexit_gaps"] += 1
                    hard.append((ob, "__cxa_atexit registrations %d/%d in "
                                     "primary block %s" %
                                (direct_p["atexit"], orig_p["atexit"], cb)))
            if orig_p["scalars"] and \
                    not set(orig_p["scalars"]) <= set(direct_p["scalars"]):
                missing_ids = set(orig_p["scalars"]) - \
                    set(direct_p["scalars"])
                hard.append((ob, "status counter ids missing from primary "
                                 "block %s: %s" %
                            (cb, ", ".join(str(x) for x in
                                           sorted(missing_ids)[:8]))))

        # ---- evidence notes -------------------------------------------
        missing_literals = set(orig_p["literals"]) - set(direct_p["literals"])
        if missing_literals:
            soft.append((ob, "counter-name literal(s) not seen in primary "
                             "block %s: %s" %
                        (cb, ", ".join(sorted(missing_literals)[:6]))))
        if orig_p["guard"] and not direct_p["guard"]:
            soft.append((ob, "guard acquire %d/%d in primary block %s" %
                        (direct_p["guard"], orig_p["guard"], cb)))
        if direct_p["guard"] and not orig_p["guard"]:
            soft.append((ob, "extra guard acquire in primary block %s: %d" %
                        (cb, direct_p["guard"])))

    hard.sort()
    soft.sort()
    return hard, soft, summary


def main(argv):
    root = "audit/report"
    diff = json.load(open(root + "/diff.json"))
    work = json.load(open(root + "/workitems.json"))
    orig = json.load(open("/tmp/orig_model.json"))
    ours = json.load(open("/tmp/our_model.json"))
    io = json.load(open(root + "/init_orig.json"))
    iu = json.load(open(root + "/init_ours.json"))
    strict_funcs = "--strict-missing-funcs" in argv

    results = []

    def report(cat, hard, detail):
        results.append((cat, hard, detail))

    # ---- types ---------------------------------------------------------
    bad_size = [r for r in diff["types"]["size_mismatch"]
                if not boundary(r["name"])]
    bad_member = [r for r in diff["types"]["member_mismatch"]
                  if not boundary(r["name"]) and
                  not diff_is_boundary(r.get("diffs", []))]
    report("types", bool(bad_size or bad_member),
           "size=%d member=%d outside compiler boundary; "
           "boundary-only: size=%d member=%d" %
           (len(bad_size), len(bad_member),
            len(diff["types"]["size_mismatch"]) - len(bad_size),
            len(diff["types"]["member_mismatch"]) - len(bad_member)))

    # ---- ODR -----------------------------------------------------------
    odr_bad = []
    for row in diff["types"]["our_internal_conflicts"]:
        if boundary(row["name"]):
            continue
        # diff_model compares the variants with the same nested/byte-span
        # canonicalisation as the ORIG comparison; a raw member-count delta
        # alone is a DWARF-shape artifact (GCC 4.1 anonymous definitions vs
        # GCC 13 named nested types), not a layout conflict.
        if row.get("layout_conflict", True):
            odr_bad.append(row)
    report("odr", bool(odr_bad),
           "%d business types with diverging layouts in the build" %
           len(odr_bad))

    # ---- functions -----------------------------------------------------
    sig_bad = []
    for row in diff["funcs"]["signature_mismatch"]:
        if boundary(row["pretty"]):
            continue
        if signature_has_business_diff(row, orig["funcs"], ours["funcs"],
                                       orig["types"], ours["types"]):
            sig_bad.append(row)
    report("funcs", bool(sig_bad),
           "%d signature mismatches outside compiler boundary "
           "(per-parameter vendor check)" % len(sig_bad))

    # ---- objects -------------------------------------------------------
    obj = diff["objects"]["size_mismatch"]
    report("objects", bool(obj), "%d data/object size mismatches" % len(obj))

    # ---- per-CU ABI layouts (stat/stat64) -------------------------------
    abi = diff.get("abi")
    abi_bad = []
    if not abi or not abi.get("available"):
        abi_bad.append(("abi-report", "per-CU stat/stat64 layouts missing "
                                     "from diff.json; regenerate with "
                                     "audit/dwarf_model.py + diff_model.py"))
        report("abi", True, "per-CU stat/stat64 layouts unavailable; "
                            "regenerate audit report")
    else:
        for row in abi.get("mismatches", []):
            member = next((d for d in row.get("diffs", [])
                           if d.get("kind") == "members"), None)
            detail = ("%s %s: ORIG %s -> recon %s, size ORIG %s vs recon %s"
                      % (row["orig_cu"], row["name"], row["orig_cu"],
                         row["our_cu"], row["orig_size"], row["ours_size"]))
            if member:
                detail += ", members first_index=%s (%s vs %s)" % (
                    member.get("first_index"), member.get("orig_count"),
                    member.get("ours_count"))
            abi_bad.append((row["our_cu"], detail))
        report("abi", bool(abi_bad),
               "%d per-CU stat/stat64 layout mismatches (checked=%d, "
               "unmapped=%d)" %
               (len(abi_bad), abi.get("checked", 0),
                len(abi.get("unmapped", []))))

    # ---- statics (global presence) -------------------------------------
    our_links = set()
    our_pretty = defaultdict(list)
    our_obj_sizes = defaultdict(set)
    for lst in ours["vars"].values():
        for var in lst:
            our_links.add(var["name"])
            our_pretty[var["pretty"] or var["name"]].append(var)
    for name, rec in ours["objects"].items():
        base_name = re.sub(r"^_ZL\d+", "", name)
        our_obj_sizes[base_name].add(rec.get("size"))
        our_obj_sizes[name].add(rec.get("size"))

    missing_vars = []
    statics_soft = []
    for lst in orig["vars"].values():
        for var in lst:
            link = var["name"]
            pretty = var["pretty"] or link
            cu = basename(var.get("cu"))
            if pretty in STATICS_BOUNDARY_NAMES:
                continue
            if link.startswith("_Z"):
                if link not in our_links:
                    missing_vars.append("%s:%s" % (cu, link))
                continue
            cands = our_pretty.get(pretty, [])
            if not cands:
                # local/internal linkage may be emitted under _ZL<len>name
                if var.get("size") in our_obj_sizes.get(pretty, set()):
                    continue
                missing_vars.append("%s:%s" % (cu, pretty))
                continue
            exact = [v for v in cands
                     if v.get("size") == var.get("size") and
                     v.get("lkey") == var.get("lkey")]
            if exact:
                continue
            same_size = [v for v in cands
                         if v.get("size") == var.get("size")]
            same_type = [v for v in cands
                         if v.get("type") == var.get("type")]
            if same_size or same_type:
                statics_soft.append(
                    "%s:%s type/size %s/%s vs %s/%s" %
                    (cu, pretty, var.get("type"), var.get("size"),
                     cands[0].get("type"), cands[0].get("size")))
                continue
            # The model records declaration-only DWARF variables with size 0;
            # a symbol with the matching object size is still evidence.
            if var.get("size") in our_obj_sizes.get(pretty, set()):
                continue
            missing_vars.append("%s:%s(type/size)" % (cu, pretty))
    missing_vars = sorted(set(missing_vars))
    report("statics", bool(missing_vars),
           "%d ORIG namespace-scope objects absent by linkage/typed match"
           " (soft type/size deltas=%d)" %
           (len(missing_vars), len(statics_soft)))

    # ---- static init blocks -------------------------------------------
    init_hard, init_soft, init_summary = evaluate_init(
        io, iu, orig, ours, diff["types"].get("cu_map"))
    report("init", bool(init_hard),
           "%d hard gaps; %d/%d same-TU blocks; status=%d ioinit=%d "
           "atexit=%d dtor=%d; business uncovered=%d" %
           (len(init_hard), init_summary["same_tu"],
            init_summary["orig_blocks"], init_summary["status_gaps"],
            init_summary["ioinit_gaps"], init_summary["atexit_gaps"],
            init_summary["dtor_gaps"],
            init_summary["business_uncovered"]))

    # ---- missing function coverage (informational or strict) -----------
    fm = work["funcs_missing"]
    biz = [(cu, n) for cu, names in fm.items() for n in names
           if not boundary(n)]
    report("funcs-cover", strict_funcs and bool(biz),
           "%d non-libstdc++ ORIG functions absent (informational) len=%d" %
           (len(biz), len(biz)))

    # ---- print ---------------------------------------------------------
    print("category        result  detail")
    hard_fail = False
    for cat, hard, detail in results:
        hard_fail |= hard
        print("%-15s %-6s  %s" % (cat, "FAIL" if hard else "PASS", detail))
    print("")
    if init_hard:
        print("init hard gaps:")
        for ocu, why in init_hard[:60]:
            print("  %-45s %s" % (ocu, why))
    if init_soft:
        print("init shape/coverage notes (first 60):")
        for ocu, why in init_soft[:60]:
            print("  %-45s %s" % (ocu, why))
    print("init summary: orig_blocks=%d recon_blocks=%d same_tu=%d "
          "no_same_tu=%d business_uncovered=%d status_gaps=%d "
          "ioinit_gaps=%d atexit_gaps=%d dtor_gaps=%d" %
          (init_summary["orig_blocks"], init_summary["recon_blocks"],
           init_summary["same_tu"], init_summary["no_same_tu"],
           init_summary["business_uncovered"], init_summary["status_gaps"],
           init_summary["ioinit_gaps"], init_summary["atexit_gaps"],
           init_summary["dtor_gaps"]))
    if abi_bad:
        print("per-CU ABI layout mismatches:")
        for ocu, why in abi_bad[:20]:
            print("  %-45s %s" % (ocu, why))
    if bad_size:
        print("type size deviations:")
        for row in bad_size[:20]:
            print("  %-50s orig=%s ours=%s" %
                  (row["name"], row["orig"]["size"], row["ours"]["size"]))
    if odr_bad:
        print("ODR conflicts:")
        for row in odr_bad[:20]:
            print("  %-50s %s" %
                  (row["name"], [(v["size"], v["cu"]) for v in row["variants"]]))
    if missing_vars:
        print("absent statics: " + ", ".join(missing_vars[:20]))
    if statics_soft:
        print("statics type/size notes (first 10):")
        for row in statics_soft[:10]:
            print("  " + row)
    print("")
    print("RESULT:", "FAIL" if hard_fail else "PASS")
    return 1 if hard_fail else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
