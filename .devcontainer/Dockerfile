FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
  vim tmux sudo git wget curl cmake build-essential

RUN apt-get update && apt-get install -y \
  xterm \
  libboost-system-dev \
  libssl-dev \
  && apt-get clean

RUN apt-get update && apt-get install -y \
  curl \
  gnupg \
  ca-certificates \
  && curl -fsSL https://deb.nodesource.com/setup_18.x | bash - \
  && apt-get install -y nodejs \
  && npm install -g wscat \
  && apt-get clean

ARG USER=dev
ARG UID=1000


RUN echo 'useradd -m -s /bin/bash -u $UID $USER && echo "$USER ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers'
RUN useradd -m -s /bin/bash -u $UID $USER && echo "$USER ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
ARG USER=dev

USER ${USER}

COPY .tmux.conf /home/${USER}