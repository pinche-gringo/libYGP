# RPM spec file for libYGP (and libXGP)
#
# The version is passed from outside (it's maintained in meson.build only):
#   rpmbuild -ba --define "ygp_version 0.9.51" libYGP.spec
# with libYGP-<version>.tar.xz (from "meson dist") in the SOURCES directory.

%{!?ygp_version: %{error:Define ygp_version (e.g. rpmbuild --define "ygp_version 0.9.51")}}

Name:           libYGP
Version:        %{ygp_version}
Release:        1%{?dist}
Summary:        General purpose C++ library
License:        GPL-3.0-or-later
URL:            https://github.com/pinche-gringo/libYGP
Source0:        %{name}-%{version}.tar.xz

BuildRequires:  gcc
BuildRequires:  gcc-c++
BuildRequires:  meson
BuildRequires:  ninja-build
BuildRequires:  gettext
BuildRequires:  boost-devel
BuildRequires:  gtkmm4.0-devel

%description
libYGP is a C++ library with classes of general use like attributes,
date/time-values, parsers, regular expressions, directory searching,
INI-files, logging and processes.

%package        devel
Summary:        Development files for libYGP
Requires:       %{name}%{?_isa} = %{version}-%{release}
Requires:       boost-devel

%description    devel
Header files, pkg-config file and the development library to build
programs using libYGP.

%package     -n libXGP
Summary:        GTK 4 based C++ library extending libYGP
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description -n libXGP
libXGP contains classes for GUI applications based on gtkmm 4 like
dialogs, widgets to display or edit attributes and an HTML viewer.

%package     -n libXGP-devel
Summary:        Development files for libXGP
Requires:       libXGP%{?_isa} = %{version}-%{release}
Requires:       %{name}-devel%{?_isa} = %{version}-%{release}
Requires:       gtkmm4.0-devel

%description -n libXGP-devel
Header files, pkg-config file and the development library to build
programs using libXGP.

%prep
%autosetup

%build
%meson -Dnls=enabled -Dxgp=enabled
%meson_build

%install
%meson_install
%find_lang %{name}

%check
%meson_test

%files -f %{name}.lang
%license COPYING
%doc AUTHORS CREDITS NEWS README
%{_libdir}/libYGP-%{version}.so

%files devel
%{_bindir}/mgeni.pl
%{_includedir}/config.h
%{_includedir}/ygp-cfg.h
%{_includedir}/YGP/
%{_libdir}/libYGP.so
%{_libdir}/pkgconfig/YGP.pc

%files -n libXGP
%{_libdir}/libXGP-%{version}.so
%{_datadir}/XGP/

%files -n libXGP-devel
%{_includedir}/XGP/
%{_libdir}/libXGP.so
%{_libdir}/pkgconfig/XGP.pc

%changelog
