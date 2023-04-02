#
# LIBGFXPRIM-CURL specfile
#
# (C) Cyril Hrubis metan{at}ucw.cz 2013-2023
#

Summary: CURL integration into gfxprim widgets
Name: libgfxprim-curl
Version: git
Release: 1
License: LGPL-2.1-or-later
Group: System/Libraries
Url: https://github.com/gfxprim/libgfxprim-curl
Source: libgfxprim-curl-%{version}.tar.bz2
BuildRequires: libcurl-devel
BuildRequires: libgfxprim-devel

BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot

%package -n libgfxprim-curl1
Group: System/Libraries
Summary: CURL integration into gfxprim widgets

%package devel
Group: Development/Libraries/C and C++
Summary: Development files for libgfxprim-curl library
Requires: libgfxprim-curl1

%description
CURL integration into gfxprim widgets.

%description -n libgfxprim-curl1
CURL integration into gfxprim widgets.

%description devel
Devel package for libgfxprim-curl library

%prep
%setup -n libgfxprim-curl-%{version}

%build
./configure --prefix='/usr' --libdir=%{_libdir} --includedir=%{_includedir}
make %{?jobs:-j%jobs}

%install
DESTDIR="$RPM_BUILD_ROOT" make install

%files -n libgfxprim-curl1
%defattr(-,root,root)
%{_libdir}/libgfxprim-curl.so.1
%{_libdir}/libgfxprim-curl.so.1.0.0

%files devel
%defattr(-,root,root)
%{_libdir}/libgfxprim-curl.so
%{_libdir}/libgfxprim-curl.a
%{_includedir}/gfxprim/widgets/*.h

%changelog
* Sun Mar 5 2023 Cyril Hrubis <metan@ucw.cz>

  Initial version.
