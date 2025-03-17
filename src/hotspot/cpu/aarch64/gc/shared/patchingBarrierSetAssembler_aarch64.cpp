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
#include "gc/g1/g1BarrierSetRuntime.hpp"
#include "gc/z/zBarrierSetAssembler_aarch64.hpp"

PatchingBarrierSlowPathPackageC2 PatchingBarrierSetAssembler::slow_path_c2(MacroAssembler* masm, 
                                                                           const MachNode *node,
                                                                           Address ref_addr,
                                                                           Register ref,
                                                                           Register tmp1,
                                                                           Register tmp2,
                                                                           Register tmp3) {
  // G1 slow path.
  assert_different_registers(noreg, tmp1, tmp2, tmp3);
  G1BarrierSetAssembler* g1_asm = static_cast<G1BarrierSetAssembler*>(BarrierSet::barrier_set()->barrier_set_assembler());
  // Generate the G1 stubs, but here we may have to be careful about register preservation.
  G1PreBarrierStubC2* const g1 = G1PreBarrierStubC2::create(node);  
  g1->initialize_registers(noreg, tmp1, rthread, tmp2, tmp3);
  // We need a G1 runtime call to jump to if the buffer is full.
  Label runtime;
  // We delegate the creation of the slow path.
  G1BarrierSetAssembler::generate_pre_barrier_slow_path(masm, 
                                                        g1->obj(), 
                                                        g1->pre_val(), 
                                                        g1->thread(), 
                                                        g1->tmp1(), 
                                                        g1->tmp2(), 
                                                        *g1->continuation(), 
                                                        runtime);

  masm->bind(runtime);
  // For the runtime call, make a new scope such that the register saving/recovering works.
  {
    Register arg = g1->pre_val();
    SaveLiveRegisters save_registers(masm, g1);
    if (c_rarg0 != arg) {
      masm->mov(c_rarg0, arg);
    }
    masm->mov(c_rarg1, rthread);
    masm->mov(rscratch1, CAST_FROM_FN_PTR(address, G1BarrierSetRuntime::write_ref_field_pre_entry));
    masm->blr(rscratch1);
  }

  // Generate the ZGC slow path, this takes care of preservations itself.
  ZLoadBarrierStubC2Aarch64* const z = ZLoadBarrierStubC2Aarch64::create(node, ref_addr, ref);

  // Pass on all of the labels and continuations.
  PatchingBarrierSlowPathPackageC2 pack {g1->entry(), z->entry(), g1->continuation(), z->continuation()};
  return pack;
}
