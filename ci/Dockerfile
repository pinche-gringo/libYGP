# Base image for the CI-jobs of libYGP (see .gitlab-ci.yml)
#
# Changing this file rebuilds the image on the next push
FROM fedora:45

RUN dnf install -y --setopt=install_weak_deps=False \
        gcc gcc-c++ meson ninja-build git gettext gettext-devel perl rpm-build \
        boost-devel gtkmm4.0-devel \
        cppcheck curl \
    && dnf clean all
