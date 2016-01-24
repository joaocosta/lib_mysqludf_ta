Name:           lib_mysqludf_ta
Version:        0.1
Release:        1%{?dist}
Summary: Technical analisys functions implemented as MySQL user defined functions
Group:          Development/Libraries
License:        GPL/MIT
URL:            http://www.mysqludf.org/lib_mysqludf_ta/lib_mysqludf_ta-%{version}.tar.gz
Source0:        lib_mysqludf_ta-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:       mariadb-server >= 5.5
Requires:       mariadb
BuildRequires:  mariadb
BuildRequires:  mariadb-devel

%description
Implements technical analysis functions as MySQL UDFs.

Currently implemented functions are:

TA_SMA
TA_EMA
TA_RSI
TA_TR (True Range)
TA_SUM (Running sum, as opposed to aggregate sum provided by mysql)
TA_PREVIOUS

Other indicators which can be derived from those functions include:

MACD
Bollinger Bands
ADX

Possibly others


%prep
#Runs at build time
%setup -q
rm -f *.o


%build
%configure
#Runs at build time
make %{?_smp_mflags}


%install
#runs at build time, executes these in the build host

rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
#runs after a build (not after installation)
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc
%{_libdir}/mysql/plugin/lib_mysqludf_ta.la
%{_libdir}/mysql/plugin/lib_mysqludf_ta.so
%{_libdir}/mysql/plugin/lib_mysqludf_ta.so.0
%{_libdir}/mysql/plugin/lib_mysqludf_ta.so.0.0.0
#%{_libexecdir}/%{name}/



%changelog
