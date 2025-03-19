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

#ifndef SHARE_GC_SHARED_C2_PATCHINGBARRIERSETC2_HPP
#define SHARE_GC_SHARED_C2_PATCHINGBARRIERSETC2_HPP

#include "gc/shared/c2/cardTableBarrierSetC2.hpp"
#include "gc/g1/c2/g1BarrierSetC2.hpp"
#include "gc/z/c2/zBarrierSetC2.hpp"

class PatchingBarrierSetC2 : public G1BarrierSetC2 {
protected:
  // The load emission itself, here the barrier data has to be populated.
  virtual Node* load_at_resolved(C2Access& access, const Type* val_type) const;
private: 
  // Consolidated function that checks if the barrier is needed.
  static bool barrier_needed(DecoratorSet decorators, BasicType type);
  static void write_barrier_data(C2Access& access);
};

const uint8_t PatchingBarrierStrong      =  1;
const uint8_t PatchingBarrierWeak        =  2;
const uint8_t PatchingBarrierPhantom     =  4;
const uint8_t PatchingBarrierNoKeepalive =  8;
const uint8_t PatchingBarrierNative      = 16;
//const uint8_t PatchingBarrierElided      = 32;



#endif // SHARE_GC_SHARED_C2_PATCHINGBARRIERSETC2_HPP
