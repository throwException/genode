FROM ubuntu:16.04 as base
RUN apt-get update && apt-get install -qy --no-install-recommends \
  apt-utils \
  autoconf2.64 \
  autogen \
  binutils \
  bison \
  bzip2 \
  ca-certificates \
  cpp \
  curl \
  dosfstools \
  e2tools \
  expect \
  flex \
  g++ \
  gawk \
  gcc \
  gdisk \
  git \
  gperf \
  iasl \
  libc6-dev \
  libxml2-utils \
  make \
  mtools \
  parallel \
  parted \
  patch \
  python \
  python-future \
  python-pip \
  python-setuptools \
  qt5-default \
  qtchooser \
  subversion \
  sudo \
  tcl \
  tzdata \
  udev \
  unzip \
  wget \
  xorriso \
  xsltproc \
  xz-utils \
  yasm \
  zlib1g-dev
ENV GENODE_GCC https://sourceforge.net/projects/genode/files/genode-toolchain/17.05/genode-toolchain-17.05-x86_64.tar.xz
RUN ["/bin/bash", "-c", "set -o pipefail && curl -sL \"$GENODE_GCC\" | tar xPJf -"]

FROM base
ENV GENODE_CONTRIB_DIR /var/cache/genode/contrib
ENV GENODE_TOOLCHAIN_DIR /var/cache/genode/toolchain
RUN mkdir -p $GENODE_CONTRIB_DIR
COPY contrib/*.tar.xz $GENODE_CONTRIB_DIR/
COPY toolchain/*.tar.xz $GENODE_TOOLCHAIN_DIR/
