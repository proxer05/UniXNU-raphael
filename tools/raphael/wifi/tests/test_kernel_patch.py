#!/usr/bin/env python3
"""Regression checks for the source-only, disabled kernel bridge patch."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[4]
PATCH = ROOT / 'xnu/patches-11215/0263-raphael-wifi-bridge.patch'
BSD_FILES = ('flame_sta.c', 'flame_wlan.c', 'redwood_apple80211.c',
             'redwood_net.c', 'redwood_sta.c', 'sweet_pmem.c', 'sweet_rpmh.c')
IOKIT_FILES = ('RedwoodWLAN.cpp', 'SweetNetwork.cpp', 'SweetNetwork.h')
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
        expected = {'bsd/conf/files', 'bsd/dev/sweet_ufs.c', 'bsd/dev/sweet_smmu.c',
                    'iokit/conf/files.arm64'}
        expected.update('bsd/dev/' + name for name in BSD_FILES)
        expected.update('bsd/sys/' + name for name in HEADERS)
        expected.update('iokit/Kernel/arm/' + name for name in IOKIT_FILES)
        pass  # all files already listed above
        self.assertEqual(set(sections()), expected)

    def test_build_registration_complete(self):
        parts = sections()
        iokit = added(parts['iokit/conf/files.arm64'])
        # The baseline already registers SweetNetwork.cpp (patch 0110) and headers are
        # found via includes; this patch must add only RedwoodWLAN.cpp.
        self.assertTrue(any(line.split() == ['iokit/Kernel/arm/RedwoodWLAN.cpp', 'optional', 'iokitcpp']
                            for line in iokit))
        self.assertNotIn('+iokit/Kernel/arm/SweetNetwork.cpp', parts['iokit/conf/files.arm64'])

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

    def test_shared_network_classes(self):
        parts = sections()
        wlan = '\n'.join(added(parts['iokit/Kernel/arm/RedwoodWLAN.cpp']))
        self.assertIn('#include "SweetNetwork.h"', wlan)
        self.assertNotRegex(wlan, r'class\s+IONetworkInterface\s*:')
        self.assertNotIn('MiniIONetworkStack.cpp', parts)
        stack = '\n'.join(added(parts['iokit/Kernel/arm/SweetNetwork.cpp']))
        self.assertIn('MiniIONetworkStackStart(void)', stack)
        self.assertIn('if (stack_published || platform == NULL)', stack)

    def test_root_credentials_enforced(self):
        parts = sections()
        self.assertIn('kauth_cred_issuser(kauth_cred_get())', parts['bsd/dev/sweet_pmem.c'])
        self.assertIn('kauth_cred_issuser(kauth_cred_get())', parts['bsd/dev/flame_wlan.c'])
        # pm_rw and PMEM_SMC are intentionally NOT blocked: wland needs them for radio bring-up

    def test_wlan_dma_is_kernel_owned(self):
        parts = sections()
        smmu = '\n'.join(added(parts['bsd/dev/sweet_smmu.c']))
        self.assertIn('sweet_smmu_wlan_setup', smmu)
        self.assertIn('sweet_smmu_wlan_info', smmu)
        self.assertIn('WLAN_IOVA_BASE  0xa0000000ull', smmu)      # raphael DT smmu_iova_base
        self.assertIn('S1 translate', smmu)
        wlan = '\n'.join(added(parts['bsd/dev/flame_wlan.c']))
        self.assertIn('FW_IOCTL_DMA_ARENA', wlan)
        self.assertIn('kmem_alloc_contig', wlan)
        self.assertIn('sweet_smmu_wlan_setup', wlan)
        header = '\n'.join(added(parts['bsd/sys/flame_wlan.h']))
        self.assertIn('struct fw_dma_arena', header)
        self.assertIn('FW_IOCTL_DMA_ARENA', header)
        # the radio must not assume any physical pool any more
        pmem = '\n'.join(added(parts['bsd/dev/sweet_pmem.c']))
        self.assertIn('sweet_dma_sync_va', pmem)

    def test_no_reserved_memory_pool(self):
        text = PATCH.read_text()
        for value in ('0x8a000000', '0x8A000000', '0xE0000000', '0xe0000000'):
            self.assertNotIn(value, text)


if __name__ == '__main__':
    unittest.main()
