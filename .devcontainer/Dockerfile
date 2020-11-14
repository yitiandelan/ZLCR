FROM archlinux

RUN curl "https://www.archlinux.org/mirrorlist/?country=CN&protocol=https" > /etc/pacman.d/mirrorlist \
    && sed -i -E "s/^#S/S/g" /etc/pacman.d/mirrorlist \
    && pacman -Syu --noconfirm --needed sudo git git-lfs \
    && useradd --create-home tianlan \
    && echo "tianlan ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers \
    && pacman -Sc --noconfirm

USER tianlan

RUN sudo pacman -Sy --noconfirm --needed \
    arm-none-eabi-gcc arm-none-eabi-newlib make ninja gn \
    python python-pip \
    && sudo pacman -Sc --noconfirm \
    && pip install --user fire
