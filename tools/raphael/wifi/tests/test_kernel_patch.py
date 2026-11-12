#!/usr/bin/env python3
"""Regression checks for the source-only, disabled kernel bridge patch."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[4]
PATCH = ROOT / 'xnu/patches-11215/0263-raphael-wifi-bridge.patch'
BSD_FILES = ('flame_sta.c', 'flame_wlan.c', 'redwood_apple80211.c',
             'redwood_net.c', 'redwood_sta.c', 'sweet_pmem.c')
IOKIT_FILES = ('MiniIONetworkStack.cpp', 'RedwoodWLAN.cpp')
HEADERS = ('flame_wlan.h', 'redwood_input.h', 'redwood_net.h', 'redwood_sta.h')


def sections():
    text = PATCH.read_text()
    pattern = re.compile(r'^diff --git a/(\S+) b/(\S+)\n', re.MULTILINE)
    matches = list(pattern.finditer(text))
    result = {}
    for i, match in enumerate(matches):
        end = matches[i + 1].start() if i + 1 < len(matches) else len(text)
        result[match.group(2)] = text[match.start():end]
    return result


def added(text):
    return [line[1:] for line in text.splitlines()
            if line.startswith('+') and not line.startswith('+++')]


class KernelPatchTests(unittest.TestCase):
    def test_exact_source_scope(self):
        expected = {'bsd/conf/files', 'bsd/dev/sweet_ufs.c', 'iokit/conf/files.arm64'}
        expected.update('bsd/dev/' + name for name in BSD_FILES)
        expected.update('bsd/sys/' + name for name in HEADERS)
        expected.update('iokit/Kernel/arm/' + name for name in IOKIT_FILES)
        self.assertEqual(set(sections()), expected)

    def test_build_registration_complete(self):
        parts = sections()
        bsd = added(parts['bsd/conf/files'])
        for name in BSD_FILES:
            self.assertTrue(any(line.split() == ['bsd/dev/' + name, 'standard'] for line in bsd), name)
        iokit = added(parts['iokit/conf/files.arm64'])
        for name in IOKIT_FILES:
            self.assertTrue(any(line.split() == ['iokit/Kernel/arm/' + name, 'optional', 'iokitcpp']
                                for line in iokit), name)

    def test_build_lines_have_type_on_same_line(self):
        for path in ('bsd/conf/files', 'iokit/conf/files.arm64'):
            for line in added(sections()[path]):
                tokens = line.split()
                if not tokens or line.lstrip().startswith('#'):
                    continue
                self.assertGreaterEqual(len(tokens), 2, line)
                self.assertIn(tokens[1], ('standard', 'optional'), line)

    def test_preserve_existing_platform_expert(self):
        text = sections()['iokit/conf/files.arm64']
        self.assertNotIn('-iokit/Kernel/arm/SweetPlatformExpert.cpp', text)
        self.assertRegex(text, r'(?m)^ iokit/Kernel/arm/SweetPlatformExpert\.cpp\s+optional iokitcpp$')

    def test_no_local_build_products(self):
        text = PATCH.read_text()
        for value in ('/Users/runner/', '/hdd/', 'diff --git a/-MJ', 'beryllium_wlan.o.json'):
            self.assertNotIn(value, text)

    def test_both_nodes_disabled_by_default(self):
        parts = sections()
        self.assertRegex(parts['bsd/dev/sweet_pmem.c'], r'uint32_t on = 0;')
        self.assertRegex(parts['bsd/dev/flame_wlan.c'], r'uint32_t on = 0;')

    def test_root_credentials_and_hardware_gate(self):
        parts = sections()
        self.assertIn('kauth_cred_issuser(kauth_cred_get())', parts['bsd/dev/sweet_pmem.c'])
        self.assertIn('kauth_cred_issuser(kauth_cred_get())', parts['bsd/dev/flame_wlan.c'])
        self.assertRegex(parts['bsd/dev/sweet_pmem.c'], r'pm_rw\([^\n]+\)\n\+\{\n\+\t/\*[^\n]+\*/\n\+\treturn EPERM;')
        self.assertRegex(parts['bsd/dev/sweet_pmem.c'], r'case PMEM_SMC:\n\+\t\t/\*[^\n]+\*/\n\+\t\terr = EPERM;')


if __name__ == '__main__':
    unittest.main()
