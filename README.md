# POSIX-Like OS for Cloud/HPC

## Overview

This project is a POSIX-like operating system designed to run unmodified Linux applications in Cloud and High-Performance Computing (HPC) environments. It is based on the LK/Zircon kernel from the Fuchsia/LK project, leveraging its modern microkernel architecture for performance, scalability, and security.

[Read more about LK](https://github.com/littlekernel/lk).

[Read more about Fuchsia](https://fuchsia.dev).

## Features

- **Linux Compatibility** – Run existing Linux applications without modification.
- **Cloud & HPC Focus** – Optimized for cloud workloads and high-performance computing.
- **Scalability** – Designed to efficiently handle cloud-scale workloads.

## Getting Started

# Jiri/Cipd Bootstrap:

```
cd micro-kernel

curl -s "https://fuchsia.googlesource.com/jiri/+/HEAD/scripts/bootstrap_jiri?format=TEXT" | base64 --decode | bash -s $PWD

# Update binary/packages dependencies using jiri
export PATH=$PWD/.jiri_root/bin:$PATH

# Import the mk integration
jiri import -name=integration mk https://github.com/misttech/mk-integration

# Download prebuilt binaries
jiri -v -vv fetch-packages
```

# Build:

```
./prebuilt/third_party/gn/mac-x64/gn gen build-x64
./prebuilt/third_party/ninja/mac-x64/ninja -C build-x64
```

## License

The LK Kernel is under an MIT-style license, copies of which may be found in [LICENSE.lk](LICENSE.lk).
