%define prefix /usr

%define ver 1.0.1
%define rel 1

Name: surf
Version: %{ver}
Release: %{rel}
Group: X11/Utilities
Copyright: GPL
URL: http://surf.sourceforge.net/
Summary: visualization of real algebraic geometry

Source: http://download.sourceforge.net/surf/surf-%{ver}.tar.gz
BuildRoot: /tmp/surf-%{ver}-root

%description
surf is a tool to visualize some real algebraic geometry: plane algebraic
curves, algebraic surfaces and hyperplane sections of surfaces. surf is
script driven and has (optionally) a nifty GUI using the Gtk widget set.

The algorithms should be stable enough not to be confused by curve/surface
singularities in codimension greater than one and the degree of the
surface or curve. This has been achieved quite a bit. We have drawn curves
of degree up to 30 and surfaces of degree up to 20 successfully. However,
there are examples of curves/surfaces of lower degree where surf fails to
produce perfect images. This happens especially if the equation of the
curve/surface is not reduced. Best results are achieved using reduced
equations. On the other hand, surf displays the Fermat-curves accurately
for degree up to 98.

surf is free software distributed under the GNU General Public License
(GPL).

%prep

%setup

%build
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix}
make

%install
rm -rf $RPM_BUILD_ROOT
make install prefix=$RPM_BUILD_ROOT%{prefix}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS COPYING README TODO docs/ examples/
%{prefix}/bin/surf
%{prefix}/man/man1/surf.1
%{prefix}/share/surf/*
