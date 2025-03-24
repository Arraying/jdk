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
#include "gc/shared/gc_globals.hpp"
#include "gc/z/c2/zBarrierSetC2.hpp"

const uint8_t PatchingBarrierStrong      =  1;
const uint8_t PatchingBarrierWeak        =  2;
const uint8_t PatchingBarrierPhantom     =  4;
const uint8_t PatchingBarrierNoKeepalive =  8;
const uint8_t PatchingBarrierNative      = 16;
const uint8_t PatchingBarrierElided      = 32;

class PatchingBarrierSetC2Logic : public AllStatic {
private:
  // Consolidated function that checks if the barrier is needed.
  static bool barrier_needed(DecoratorSet decorators, BasicType type);
public:
  // Writes barrier data for a C2 write.
  static void write_barrier_data(C2Access& access);
};

// The ZGC implementation.
class PatchingZBarrierSetC2 : public ZBarrierSetC2 {
protected:
  virtual Node* load_at_resolved(C2Access& access, const Type* val_type) const {
    if (UseLoadPB) {
      PatchingBarrierSetC2Logic::write_barrier_data(access);
      return BarrierSetC2::load_at_resolved(access, val_type);
    }
    return ZBarrierSetC2::load_at_resolved(access, val_type);
  }
};

// The G1 implementation.
class PatchingG1BarrierSetC2 : public G1BarrierSetC2 {
protected:
  virtual Node* load_at_resolved(C2Access& access, const Type* val_type) const {
    if (UseLoadPB) {
      PatchingBarrierSetC2Logic::write_barrier_data(access);
      return BarrierSetC2::load_at_resolved(access, val_type);
    }
    return G1BarrierSetC2::load_at_resolved(access, val_type);
  }
};

// Serial & Parallel need some state for stub emission.
// This is done in the ZGC style.
class PatchingBarrierSetC2State : public BarrierSetC2State {
private:
  GrowableArray<BarrierStubC2*>* _stubs;
  int                            _trampoline_stubs_count;
  int                            _stubs_start_offset;

public:
  PatchingBarrierSetC2State(Arena* arena)
    : BarrierSetC2State(arena),
      _stubs(new (arena) GrowableArray<BarrierStubC2*>(arena, 8,  0, nullptr)),
      _trampoline_stubs_count(0),
      _stubs_start_offset(0) {}

  GrowableArray<BarrierStubC2*>* stubs() {
    return _stubs;
  }

  bool needs_liveness_data(const MachNode* mach) const {
    // Don't need liveness data for nodes without barriers
    return mach->barrier_data() != PatchingBarrierElided;
  }

  bool needs_livein_data() const {
    return true;
  }

  void inc_trampoline_stubs_count() {
    assert(_trampoline_stubs_count != INT_MAX, "Overflow");
    ++_trampoline_stubs_count;
  }

  int trampoline_stubs_count() {
    return _trampoline_stubs_count;
  }

  void set_stubs_start_offset(int offset) {
    _stubs_start_offset = offset;
  }

  int stubs_start_offset() {
    return _stubs_start_offset;
  }
};

// The Serial & Parallel implementation.
class PatchingCardTableBarrierSetC2 : public CardTableBarrierSetC2 {
protected:
  virtual Node* load_at_resolved(C2Access& access, const Type* val_type) const {
    if (UseLoadPB) {
      PatchingBarrierSetC2Logic::write_barrier_data(access);
      return BarrierSetC2::load_at_resolved(access, val_type);
    }
    return CardTableBarrierSetC2::load_at_resolved(access, val_type);
  }
public:
  // Need to inject custom state which doesn't exist yet since it's not late barrier expanded.
  virtual void* create_barrier_state(Arena* comp_arena) const;
  // Manually emit stubs, since currently this is a no-op.
  virtual void emit_stubs(CodeBuffer& cb) const;
};



#endif // SHARE_GC_SHARED_C2_PATCHINGBARRIERSETC2_HPP
