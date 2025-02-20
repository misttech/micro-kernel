// Copyright 2020 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#include <lib/arch/testing/x86/fake-cpuid.h>
#include <lib/arch/x86/cpuid.h>

#include <gtest/gtest.h>

namespace {

TEST(FakeCpuidIoTests, Get) {
  arch::testing::FakeCpuidIo cpuid;
  cpuid.Populate(0x0, 0x0, 0x0000'0014, 0x756e'6547, 0x6c65'746e, 0x4965'6e69)
      .Populate(0x1, 0x0, 0x0, 0x0, 0x7ffe'fbff, 0xbfeb'fbff);

  // Access by various types corresponding to leaf 0x0 should all yield the
  // same CpuidIo* - and its values should coincide with those provided above.
  auto* io0A = cpuid.Get<arch::CpuidMaximumLeaf>();
  auto* io0B = cpuid.Get<arch::CpuidVendorB>();
  auto* io0C = cpuid.Get<arch::CpuidVendorC>();
  auto* io0D = cpuid.Get<arch::CpuidVendorD>();
  EXPECT_EQ(io0A, io0B);
  EXPECT_EQ(io0A, io0C);
  EXPECT_EQ(io0A, io0D);

  auto* io0 = io0A;
  ASSERT_NE(nullptr, io0);
  EXPECT_EQ(0x0000'0014u, io0->values_[arch::CpuidIo::kEax]);
  EXPECT_EQ(0x756e'6547u, io0->values_[arch::CpuidIo::kEbx]);
  EXPECT_EQ(0x6c65'746eu, io0->values_[arch::CpuidIo::kEcx]);
  EXPECT_EQ(0x4965'6e69u, io0->values_[arch::CpuidIo::kEdx]);

  // Ditto for leaf 0x1.
  auto* io1C = cpuid.Get<arch::CpuidFeatureFlagsC>();
  auto* io1D = cpuid.Get<arch::CpuidFeatureFlagsD>();
  EXPECT_EQ(io1C, io1D);

  auto* io1 = io1C;
  ASSERT_NE(nullptr, io1);
  EXPECT_EQ(0x0u, io1->values_[arch::CpuidIo::kEax]);
  EXPECT_EQ(0x0u, io1->values_[arch::CpuidIo::kEbx]);
  EXPECT_EQ(0x7ffe'fbffu, io1->values_[arch::CpuidIo::kEcx]);
  EXPECT_EQ(0xbfeb'fbffu, io1->values_[arch::CpuidIo::kEdx]);
}

TEST(FakeCpuidIoTests, Read) {
  arch::testing::FakeCpuidIo cpuid;
  cpuid.Populate(0x0, 0x0, arch::CpuidIo::kEax, 0x0000'0014);

  auto* io = cpuid.Get<arch::CpuidMaximumLeaf>();
  ASSERT_NE(nullptr, io);

  // Read should be a shortcut to reading our the value type.
  EXPECT_EQ(0x0000'0014u, io->values_[arch::CpuidIo::kEax]);
  EXPECT_EQ(0x0000'0014u, cpuid.Read<arch::CpuidMaximumLeaf>().leaf());
}

TEST(FakeCpuidIoTests, PopulateOverwrites) {
  arch::testing::FakeCpuidIo cpuid;
  cpuid.Populate(0x0, 0x0, 0x0000'0014, 0x0, 0x0, 0x0);

  auto* io = cpuid.Get<arch::CpuidMaximumLeaf>();
  ASSERT_NE(nullptr, io);

  EXPECT_EQ(0x0000'0014u, io->values_[arch::CpuidIo::kEax]);
  cpuid.Populate(0x0, 0x0, arch::CpuidIo::kEax, 0x0000'0020);
  EXPECT_EQ(0x0000'0020u, io->values_[arch::CpuidIo::kEax]);
}

}  // namespace
