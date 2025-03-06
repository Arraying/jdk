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
 *
 */

#include "gc/shared/c2/patchingBarrierSetC2.hpp"

// The high level of this load is to consolidate all of the processing done by all GC.
Node* PatchingBarrierSetC2::load_at_resolved(C2Access& access, const Type* val_type) const {
  write_barrier_data(access);
  return BarrierSetC2::load_at_resolved(access, val_type);
}

// Copied basically from ZGC.
bool PatchingBarrierSetC2::barrier_needed(DecoratorSet decorators, BasicType type) {
  assert((decorators & AS_RAW) == 0, "Unexpected decorator");

  if (is_reference_type(type)) {
    assert((decorators & (IN_HEAP | IN_NATIVE)) != 0, "Where is reference?");
    // Barrier needed even when IN_NATIVE, to allow concurrent scanning.
    return true;
  }

  // Barrier not needed
  return false;
}

// Copied basically from ZGC.
void PatchingBarrierSetC2::write_barrier_data(C2Access& access) {
  if (!barrier_needed(access.decorators(), access.type())) {
    return;
  }
  // Normally ZGC checks if it's a C2_TIGHTLY_COUPLED_ALLOC.
  // If that's the case, it elides the barrier.
  // For simplicity, barrier elidation has been disabled.

  uint8_t barrier_data = 0;

  // Determine the strength of the access.
  // Phantom and weak are sort of handled together.
  if (access.decorators() & ON_PHANTOM_OOP_REF) {
    barrier_data |= PatchingBarrierPhantom;
  } else if (access.decorators() & ON_WEAK_OOP_REF) {
    barrier_data |= PatchingBarrierWeak;
  } else {
    barrier_data |= PatchingBarrierStrong;
  }

  // Extra checks for native and keepalive barriers.
  if (access.decorators() & IN_NATIVE) {
    barrier_data |= PatchingBarrierNative;
  }

  if (access.decorators() & AS_NO_KEEPALIVE) {
    barrier_data |= PatchingBarrierNoKeepalive;
  }

  access.set_barrier_data(barrier_data);
}

