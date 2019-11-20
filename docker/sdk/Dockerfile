FROM ubuntu:18.04 as base
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -qy --no-install-recommends \
  apt-utils \
  autoconf2.64 \
  autogen \
  automake \
  binutils \
  bison \
  bzip2 \
  ca-certificates \
  ccache \
  cmake \
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
  golang-go \
  gperf \
  gpg \
  gpg-agent \
  iasl \
  kmod \
  libc6-dev \
  libc6-dev-armel-cross \
  libc6-dev-i386 \
  libssl-dev \
  libtool \
  libxml2-utils \
  make \
  mtools \
  net-tools \
  openssh-client \
  parted \
  patch \
  picocom \
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
  sshpass \
  subversion \
  sudo \
  tcl \
  tzdata \
  u-boot-tools \
  udev \
  uml-utilities \
  unzip \
  wget \
  xorriso \
  xsltproc \
  xz-utils \
  yasm \
  zlib1g-dev

ENV TOOLCHAIN_X86          https://dev.gapfruit.com/genode-toolchain/genode-toolchain-19.05-x86.tar.xz
ENV TOOLCHAIN_ARM          https://dev.gapfruit.com/genode-toolchain/genode-toolchain-19.05-arm.tar.xz
ENV TOOLCHAIN_ARMHF        https://dev.gapfruit.com/genode-toolchain/genode-toolchain-19.05-armhf.tar.xz
ENV TOOLCHAIN_ARMHF_LINUX  https://dev.gapfruit.com/arm-linux-gnueabihf/arm-linux-gnueabihf-x86_64_7.4.tar.gz

RUN ["/bin/bash", "-c", "set -o pipefail && curl -sL \"$TOOLCHAIN_X86\"         | tar xPJf -"]
RUN ["/bin/bash", "-c", "set -o pipefail && curl -sL \"$TOOLCHAIN_ARM\"         | tar xPJf -"]
RUN ["/bin/bash", "-c", "set -o pipefail && curl -sL \"$TOOLCHAIN_ARMHF\"       | tar xPJf -"]
RUN ["/bin/bash", "-c", "set -o pipefail && curl -sL \"$TOOLCHAIN_ARMHF_LINUX\" | tar xPzf - -C /"]

FROM base
ENV GENODE_CONTRIB_DIR /var/cache/genode/contrib
ENV GENODE_TOOLCHAIN_DIR /var/cache/genode/toolchain
RUN mkdir -p $GENODE_CONTRIB_DIR
RUN echo "fuse" >> /etc/modules
COPY contrib/*.tar.xz $GENODE_CONTRIB_DIR/
COPY toolchain/*.tar.xz $GENODE_TOOLCHAIN_DIR/
COPY toolchain/netperf-* /usr/bin/

ENV GOPATH /usr
RUN go get github.com/fullstorydev/grpcurl
RUN go install github.com/fullstorydev/grpcurl/cmd/grpcurl
