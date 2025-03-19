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

#include "gc/shared/patchingBarrierSetAssembler.hpp"
#include "gc/g1/c2/g1BarrierSetC2.hpp"
#include "gc/g1/g1BarrierSetAssembler_aarch64.hpp"
#include "gc/z/zBarrierSetAssembler_aarch64.hpp"

PatchingBarrierSlowPathPackageC2 PatchingBarrierSetAssembler::slow_path_c2(MacroAssembler* masm, 
                                                                           const MachNode *node,
                                                                           Address ref_addr,
                                                                           Register ref,
                                                                           Register _tmp1,
                                                                           Register _tmp2,
                                                                           Register _tmp3) {
  // G1 slow path.
  assert_different_registers(noreg, _tmp1, _tmp2, _tmp3);
  // Generate the G1 stubs, but here we may have to be careful about register preservation.
  // We therefore alias the registers with their corresponding names in the G1 assembler.
  // Consequently, e.g. tmp3 becomes tmp2, but this is simplifies understanding grealy.
  G1PreBarrierStubC2* const g1 = G1PreBarrierStubC2::create(node);
  Register obj = noreg, pre_val = _tmp1, thread = rthread, tmp1 = _tmp2, tmp2 = _tmp3;
  g1->initialize_registers(obj, pre_val, thread, tmp1, tmp2);

  // Generate the ZGC slow path, this takes care of register preservations itself.
  ZLoadBarrierStubC2Aarch64* const z = ZLoadBarrierStubC2Aarch64::create(node, ref_addr, ref);
  
  // Pass on all of the labels and continuations.
  PatchingBarrierSlowPathPackageC2 pack {g1->entry(), z->entry(), g1->continuation(), z->continuation()};
  return pack;
}
