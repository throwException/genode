FROM ubuntu:18.04 as base
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -qy --no-install-recommends \
  apt-utils \
  autoconf2.64 \
  autogen \
  binutils \
  bison \
  bzip2 \
  ca-certificates \
  ccache \
  cpio \
  cpp \
  curl \
  dosfstools \
  e2tools \
  expect \
  flex \
  fuse \
  fuseext2 \
  g++ \
  gawk \
  gcc \
  gdisk \
  git \
  gperf \
  gpg \
  gpg-agent \
  iasl \
  kmod \
  libc6-dev \
  libc6-dev-i386 \
  libxml2-utils \
  make \
  mtools \
  openssh-client \
  parted \
  patch \
  python \
  python-future \
  python-pip \
  python-ply \
  python-setuptools \
  python-six \
  python-tempita \
  python3 \
  python3-argcomplete \
  qemu-system-arm \
  qemu-system-x86 \
  qt5-default \
  qtchooser \
  subversion \
  sudo \
  tcl \
  tzdata \
  u-boot-tools \
  udev \
  unzip \
  wget \
  xorriso \
  xsltproc \
  xz-utils \
  yasm \
  zlib1g-dev
ENV GENODE_GCC  https://dev.gapfruit.com/genode-toolchain/genode-toolchain-19.05-x86_64.tar.xz
RUN ["/bin/bash", "-c", "set -o pipefail && curl -sL \"$GENODE_GCC\" | tar xPJf -"]

FROM base
ENV GENODE_CONTRIB_DIR /var/cache/genode/contrib
ENV GENODE_TOOLCHAIN_DIR /var/cache/genode/toolchain
RUN mkdir -p $GENODE_CONTRIB_DIR
RUN echo "fuse" >> /etc/modules
COPY contrib/*.tar.xz $GENODE_CONTRIB_DIR/
COPY toolchain/*.tar.xz $GENODE_TOOLCHAIN_DIR/
