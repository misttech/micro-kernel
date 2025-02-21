#!/usr/bin/env bash

# Copyright 2025 Mist Tecnologia Ltda.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

echo "Replacing //build/zircon/ with //build/kernel/"
grep -rl '//build/zircon/' . | xargs sed -i '' 's|//build/zircon/|//build/kernel/|g'

echo "Replacing //build/config/zircon/ with //build/config/kernel/"
grep -rl '//build/config/zircon/' . | xargs sed -i '' 's|//build/config/zircon/|//build/config/kernel/|g'

echo "Replacing //zircon/system/ulib/ with //mk/ulib/"
grep -rl '//zircon/system/ulib/' . | xargs sed -i '' 's|//zircon/system/ulib/|//mk/ulib/|g'

echo "Replacing //zircon/kernel with //mk"
grep -rl '//zircon/kernel' . | xargs sed -i '' 's|//zircon/kernel|//mk|g'

echo "Replacing zircon_cpu with kernel_cpu"
grep -rl '$zircon_cpu' . | xargs sed -i '' 's|$zircon_cpu|$kernel_cpu|g'

echo "Replacing zircon_toolchain with kernel_toolchain"
grep -rl 'zircon_toolchain' . | xargs sed -i '' 's|zircon_toolchain|kernel_toolchain|g'

echo "Replacing //sdk/lib/stdcompat with //mk/lib/stdcompat"
grep -rl '//sdk/lib/stdcompat' . | xargs sed -i '' 's|//sdk/lib/stdcompat|//mk/lib/stdcompat|g'

echo "Replacing //sdk/lib/fit with //mk/lib/fit"
grep -rl '//sdk/lib/fit' . | xargs sed -i '' 's|//sdk/lib/fit|//mk/lib/fit|g'

echo "Replacing //sdk/lib/zbi-format with //mk/lib/zbi-format"
grep -rl '//sdk/lib/zbi-format' . | xargs sed -i '' 's|//sdk/lib/zbi-format|//mk/lib/zbi-format|g'

echo "Replacing //src/performance/lib/fxt with //mk/lib/fxt"
grep -rl '//src/performance/lib/fxt' . | xargs sed -i '' 's|//src/performance/lib/fxt|//mk/lib/fxt|g'

echo "Replacing //zircon/third_party/ulib/cksum with //mk/third_party/ulib/cksum"
grep -rl '//zircon/third_party/ulib/cksum' . | xargs sed -i '' 's|//zircon/third_party/ulib/cksum|//mk/third_party/ulib/cksum|g'

echo "Replacing zircon_cpu with kernel_cpu"
grep -rl 'zircon_cpu' . | xargs sed -i '' 's|zircon_cpu|kernel_cpu|g'

echo "Replacing standard_fuchsia_cpus with standard_mk_cpus"
grep -rl 'standard_fuchsia_cpus' . | xargs sed -i '' 's|standard_fuchsia_cpus|standard_mk_cpus|g'

echo "Replacing build/toolchain/zircon with build/toolchain/kernel"
grep -rl '//build/toolchain/zircon' . | xargs sed -i '' 's|//build/toolchain/zircon|//build/toolchain/kernel|g'
