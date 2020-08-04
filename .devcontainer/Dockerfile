FROM archlinux:latest

RUN curl "https://www.archlinux.org/mirrorlist/?country=CN&protocol=https" > /etc/pacman.d/mirrorlist \
    && sed -i -E "s/^#S/S/g" /etc/pacman.d/mirrorlist \
    && pacman -Syu --noconfirm sudo git git-lfs \
    && pacman -Sc --noconfirm

RUN useradd --create-home tianlan \
    && echo "tianlan ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

RUN pacman -Sy --noconfirm \
    arm-none-eabi-gcc arm-none-eabi-newlib make ninja gn \
    python python-pip \
    && pacman -Sc --noconfirm \
    && pip install fire
