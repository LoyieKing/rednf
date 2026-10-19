#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Isolated regression tests for the acceptance-gate classification rules.

These tests do not read the ELF/report tree and do not build or deploy
anything.  They pin the two acceptance holes that the 2026-10-19 pass
corrected:

  * a static object name existing somewhere must not skip static-init
    verification;
  * one vendor-boundary diff must not exempt a whole type/signature row.

Run: python3 audit/accept_selftest.py
"""
from __future__ import print_function

import os
import sys
import unittest
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import accept  # noqa: E402
import diff_model  # noqa: E402


ORIG_CU = "/orig/a.cpp"
OUR_CU = "/our/a.cpp"


def model(funcs=None, variables=None):
    return {"funcs": funcs or {}, "vars": variables or {}, "objects": {},
            "types": {}}


def var(name, cu, size=4, type_name="X", lkey="agg:X"):
    return {"name": name, "pretty": name, "type": type_name, "size": size,
            "lkey": lkey, "cu": cu}


def func(mangled, cu, pretty=None):
    return {"mangled": mangled, "pretty": pretty or mangled, "cu": cu}


def init_row(calls=None, refs=None, imms=None):
    return {"calls": calls or [], "refs": refs or [], "imms": imms or [],
            "sym": "_GLOBAL__sub_I_a", "size": 1, "cu": ORIG_CU}


class BoundaryTests(unittest.TestCase):
    def test_vendor_only_row_is_boundary(self):
        diffs = [{"kind": "member_layout_key", "path": "cfg_",
                  "orig": "agg:std::map<int, int>",
                  "ours": "agg:std::map<int, int>"}]
        self.assertTrue(accept.diff_is_boundary(diffs))

    def test_one_vendor_diff_does_not_hide_business_diff(self):
        diffs = [
            {"kind": "member_layout_key", "path": "cfg_",
             "orig": "agg:std::map<int, int>",
             "ours": "agg:std::map<int, int>"},
            {"kind": "member_offset", "path": "business_counter_",
             "orig": 4, "ours": 8},
        ]
        self.assertFalse(accept.diff_is_boundary(diffs))

    def test_signature_std_layout_size_is_boundary(self):
        row = {"mangled": "_Z1fv"}
        orig = {"_Z1fv": {"param_lkeys": ["agg:std::string"],
                          "ret_lkey": "void"}}
        ours = {"_Z1fv": {"param_lkeys": ["anon12"], "ret_lkey": "void"}}
        types = {"std::string": {"size": 12}}
        self.assertFalse(accept.signature_has_business_diff(
            row, orig, ours, types, {}))

    def test_signature_business_parameter_is_not_hidden(self):
        row = {"mangled": "_Z1fv"}
        orig = {"_Z1fv": {"param_lkeys": ["agg:std::string", "int4"],
                          "ret_lkey": "void"}}
        ours = {"_Z1fv": {"param_lkeys": ["agg:std::string", "int8"],
                          "ret_lkey": "void"}}
        types = {"std::string": {"size": 12}}
        self.assertTrue(accept.signature_has_business_diff(
            row, orig, ours, types, {}))


class InitTests(unittest.TestCase):
    def test_name_existence_does_not_replace_missing_block(self):
        """The old gate skipped this CU because all variable names existed."""
        io = {"groups": {ORIG_CU: [init_row(
            calls=[accept.STATUS_CTOR] * 16 +
                  [accept.IOS_INIT, "__cxa_atexit"])]}}
        iu = {"groups": {}}
        orig = model(
            funcs={"_Z1fv": func("_Z1fv", ORIG_CU)},
            variables={"0x100": [var("g_tss_sdk", ORIG_CU)]})
        ours = model(
            funcs={"_Z1fv": func("_Z1fv", OUR_CU)},
            variables={"0x200": [var("g_tss_sdk", OUR_CU)]})
        hard, _soft, summary = accept.evaluate_init(io, iu, orig, ours)
        self.assertTrue(any(cu == "a.cpp" and "no same-TU init block" in why
                            for cu, why in hard), hard)
        self.assertEqual(summary["no_same_tu"], 1)

    def test_one_status_ctor_does_not_match_sixteen(self):
        io = {"groups": {ORIG_CU: [init_row(
            calls=[accept.STATUS_CTOR] * 16 + [accept.IOS_INIT])]}}
        iu = {"groups": {OUR_CU: [init_row(
            calls=[accept.STATUS_CTOR] + [accept.IOS_INIT])]}}
        orig = model(funcs={"_Z1fv": func("_Z1fv", ORIG_CU)})
        ours = model(funcs={"_Z1fv": func("_Z1fv", OUR_CU)})
        hard, _soft, _summary = accept.evaluate_init(io, iu, orig, ours)
        self.assertTrue(any("COMM_STAT_ITEM_WITH_NAME ctors 1/16" in why
                            for _cu, why in hard), hard)

    def test_shared_block_must_carry_one_array_per_orig_cu(self):
        orig_b = "/orig/b.cpp"
        io = {"groups": {
            "/orig/a.cpp": [init_row(calls=[accept.STATUS_CTOR] * 16)],
            orig_b: [init_row(calls=[accept.STATUS_CTOR] * 16)],
        }}
        iu = {"groups": {OUR_CU: [init_row(
            calls=[accept.STATUS_CTOR] * 16)]}}
        orig = model(funcs={
            "_Z1fv": func("_Z1fv", "/orig/a.cpp"),
            "_Z1gv": func("_Z1gv", orig_b),
        })
        ours = model(funcs={
            "_Z1fv": func("_Z1fv", OUR_CU),
            "_Z1gv": func("_Z1gv", OUR_CU),
        })
        hard, _soft, _summary = accept.evaluate_init(io, iu, orig, ours)
        self.assertTrue(any(cu == "block:a.cpp" and "16 < 32" in why
                            for cu, why in hard), hard)

    def test_singleton_precreate_code_shape_is_equivalent(self):
        left = Counter({
            "tss_sdk_impl::TssSdkCommSvc::TssSdkCommSvc": 1})
        right = Counter({
            "Singleton<tss_sdk_impl::TssSdkCommSvc>::instance": 1})
        self.assertEqual(accept.canonical_business(left),
                         accept.canonical_business(right))

    def test_status_counter_id_mismatch_is_hard(self):
        io = {"groups": {ORIG_CU: [init_row(
            calls=[accept.STATUS_CTOR] * 1,
            imms=[9101, 430009])]}}
        iu = {"groups": {OUR_CU: [init_row(
            calls=[accept.STATUS_CTOR] * 1,
            imms=[9101, 430008])]}}
        orig = model(funcs={"_Z1fv": func("_Z1fv", ORIG_CU)})
        ours = model(funcs={"_Z1fv": func("_Z1fv", OUR_CU)})
        hard, _soft, _summary = accept.evaluate_init(io, iu, orig, ours)
        self.assertTrue(any("430009" in why for _cu, why in hard), hard)

    def test_split_global_ctor_allows_one_atexit_deferral(self):
        """g_antibot_if is one global; its dtor registration stays in the
        split slice and must not require a second instance."""
        other_cu = "/our/b.cpp"
        io = {"groups": {ORIG_CU: [init_row(calls=(
            [accept.STATUS_CTOR] * 16 +
            [accept.STATUS_DTOR, accept.IOS_INIT,
             "__cxa_atexit", "__cxa_atexit", "__cxa_atexit",
             "AntibotInterface::AntibotInterface"]))]}}
        iu = {"groups": {
            OUR_CU: [init_row(calls=(
                [accept.STATUS_CTOR] * 16 +
                [accept.STATUS_DTOR, accept.IOS_INIT,
                 "__cxa_atexit", "__cxa_atexit"]))],
            other_cu: [init_row(
                calls=["AntibotInterface::AntibotInterface"])],
        }}
        orig = model(funcs={"_Z1fv": func("_Z1fv", ORIG_CU)})
        ours = model(funcs={"_Z1fv": func("_Z1fv", OUR_CU)})
        hard, soft, _summary = accept.evaluate_init(io, iu, orig, ours)
        self.assertEqual(hard, [], hard)
        self.assertTrue(any("split-global" in why for _cu, why in soft),
                        soft)


class AbiTests(unittest.TestCase):
    @staticmethod
    def stat(cu, size, member_offset=44):
        return {
            "name": "stat", "cu": cu, "size": size, "align": 4,
            "members": [{
                "path": "st_size", "offset": member_offset,
                "size": 8 if size == 96 else 4,
                "lkey": "int8" if size == 96 else "int4",
            }],
        }

    def test_size_mismatch_is_a_hard_abi_diff(self):
        diffs = diff_model.abi_layout_diff(
            self.stat("/orig/zen_bus_mmap_pipe.cpp", 96),
            self.stat("/our/zen_bus_mmap_pipe.cpp", 88))
        self.assertTrue(any(d["kind"] == "size" and d["orig"] == 96
                            and d["ours"] == 88 for d in diffs), diffs)

    def test_same_size_different_member_offset_is_hard(self):
        diffs = diff_model.abi_layout_diff(
            self.stat("/orig/x.cpp", 96, 44),
            self.stat("/our/x.cpp", 96, 48))
        self.assertTrue(any(d["kind"] == "members" for d in diffs), diffs)

    def test_compare_abi_reports_old_bus_88_fixture(self):
        orig = {"abi_types": {"stat": [
            self.stat("/orig/zen_bus_mmap_pipe.cpp", 96)]}}
        ours = {"abi_types": {"stat": [
            self.stat("/our/zen_bus_mmap_pipe.cpp", 88)]}}
        report = diff_model.compare_abi(
            orig, ours, {"/our/zen_bus_mmap_pipe.cpp":
                         "zen_bus_mmap_pipe.cpp"})
        self.assertTrue(report["available"])
        self.assertEqual(report["checked"], 1)
        self.assertEqual(len(report["mismatches"]), 1, report)
        self.assertEqual(report["mismatches"][0]["orig_size"], 96)
        self.assertEqual(report["mismatches"][0]["ours_size"], 88)

    def test_compare_abi_accepts_matching_96(self):
        orig = {"abi_types": {"stat": [
            self.stat("/orig/zen_bus_mmap_pipe.cpp", 96)]}}
        ours = {"abi_types": {"stat": [
            self.stat("/our/zen_bus_mmap_pipe.cpp", 96)]}}
        report = diff_model.compare_abi(
            orig, ours, {"/our/zen_bus_mmap_pipe.cpp":
                         "zen_bus_mmap_pipe.cpp"})
        self.assertEqual(report["checked"], 1)
        self.assertEqual(report["mismatches"], [])


if __name__ == "__main__":
    unittest.main()
