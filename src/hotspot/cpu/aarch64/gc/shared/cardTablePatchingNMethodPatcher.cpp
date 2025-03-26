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

#include "gc/shared/cardTablePatchingNMethodPatcher.hpp"
#include "gc/shared/patchingBarrierRelocation.hpp"

void CardTablePatchingNMethodPatcher::patch_instruction(address addr, int format) {
  uint32_t* const patch_addr = (uint32_t*)addr;
  switch (format) {
  case PatchingBarrierRelocationFormatLoadGoodBeforeTbX:
    // Replace the register with WZR. Since this register 31 is all 1s, a logical OR works.
    // The register is written in the lowest 5 bits of the instruction, so no shift is needed.
    *addr |= 31u;
    break;
  case PatchingBarrierRelocationFormatGetStateBeforeLdrX: {
    // This is a G1 address which will segfault.
    // Extract the register, the lowest 5 bits are the destination registers for both ldrb/ldrw and mov.
    uint32_t reg = *patch_addr & 31u; // (1 << 5) - 1.
    // Change to mov dst, #0 instead.
    *patch_addr = 0xd2800000u | reg;
    break;
  }
  case PatchingBarrierRelocationFormatMarkBadBeforeMov:
    // This already moves a zero which will never trigger the ZGC slow path, no need to patch.
    break;
  case PatchingBarrierRelocationFormatShiftLsr:
    // This is a no-op.
    break;
  default:
    ShouldNotReachHere();
  }
}
