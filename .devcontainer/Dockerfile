FROM archlinux:base-devel

RUN curl "https://archlinux.org/mirrorlist/?country=CN&protocol=https" \
    | sed -r "s/^#Server/Server/;/^#/d;" > /etc/pacman.d/mirrorlist \
    && pacman -Syu --noconfirm \
    && echo "tianlan ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers \
    && useradd --create-home tianlan \
    && pacman -Sy --noconfirm --needed git git-lfs python make ninja gn \
    && pacman -Sy --noconfirm --needed arm-none-eabi-gcc arm-none-eabi-newlib \
    && pacman -Sc --noconfirm

USER tianlan
