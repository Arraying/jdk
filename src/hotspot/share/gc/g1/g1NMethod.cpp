/*
 * Copyright (c) 2025, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include "gc/g1/g1NMethod.hpp"
#include "code/relocInfo.hpp"
#include "gc/shared/patchingBarrierRelocation.hpp"

void G1NMethod::register_nmethod(nmethod* nm) {
  // Find all barrier relocations.
  RelocIterator iter(nm);
  while (iter.next()) {
    if (iter.type() == relocInfo::barrier_type) {
      barrier_Relocation* const reloc = iter.barrier_reloc();
      // We can actually patch right now!
      G1NMethod::patch_barriers(reloc->addr(), reloc->format());
    }
  }
}

static void change_immediate(uint32_t& instr, uint32_t imm, uint32_t start, uint32_t end) {
  uint32_t imm_mask = ((1u << start) - 1u) ^ ((1u << (end + 1)) - 1u);
  instr &= ~imm_mask;
  instr |= imm << start;
}

void G1NMethod::patch_barriers(address addr, int format) {
  uint32_t* const patch_addr = (uint32_t*)addr;
  switch (format) {
  case PatchingBarrierRelocationFormatLoadGoodBeforeTbX:
    // Patch the TB(N)Z to use a different address register (ZR) for non-ZGC.
    // Since this is G1, we patch accordingly.
    // FIXME: Uses AArch64 assumption, would be good to factor out to own platform-dependent code.
    change_immediate(*patch_addr, 31u, 0, 4);
    break;
  case PatchingBarrierRelocationFormatMarkBadBeforeMov: 
    assert(false, "weak g1 patch not implemented yet");
    break;
  default:
    ShouldNotReachHere();
  }
}
