Name:           lib_mysqludf_ta
Version:        0.1
Release:        1.%{?dist}
Summary: Technical analisys functions implemented as MySQL user defined functions
Group:          Development/Libraries
License:        GPL/MIT
URL:            http://www.mysqludf.org/lib_mysqludf_ta/lib_mysqludf_ta-%{version}.tar.gz
Source0:        lib_mysqludf_ta-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires:       mysql-server >= 5.1
Requires:       mysql
BuildRequires:  mysql

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
%setup -q
rm -f *.o


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc
/usr/lib/mysql/plugin/lib_mysqludf_ta.la
/usr/lib/mysql/plugin/lib_mysqludf_ta.a
/usr/lib/mysql/plugin/lib_mysqludf_ta.so
/usr/lib/mysql/plugin/lib_mysqludf_ta.so.0
/usr/lib/mysql/plugin/lib_mysqludf_ta.so.0.0.0



%changelog
