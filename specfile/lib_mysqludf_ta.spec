%define _topdir	 	/home/joao/myrpms
%define name		lib_mysqludf_ta 
%define release		1
%define version 	0.1
%define buildroot %{_topdir}/%{name}-%{version}-root

Summary: Technical analisys functions implemented as MySQL user defined functions
License: GPL/MIT
Group: Libraries
Requires: mysql-server >= 5.1
Requires: mysql
Name: 			%{name}
Version: 		%{version}
Release: 		%{release}
Source: 		%{name}-%{version}.tar.gz
Group: 			Development/Libraries
BuildRoot:	%{buildroot}

%description

%prep
#Happens at build time

%setup -c -q
#-q quiet
#"-c " create directory and cd into it before unpacking
# The setup macro unpacks libmysqludfta.tar.gz and makes it available in the build system
# Reference available at:
# http://www.rpm.org/max-rpm/s1-rpm-inside-macros.html


%build
#happens at build time
cd src
make


%install
#runs at build time, executes these in the build host
set -e
ROOT_DIR=usr/lib/mysql/plugin

mkdir -p $RPM_BUILD_ROOT/$ROOT_DIR

for file in lib_mysqludf_ta.so db_install_lib_mysqludf_ta db_uninstall_lib_mysqludf_ta; do
  install --mode 644 src/$file $RPM_BUILD_ROOT/$ROOT_DIR/
done

%pre
#happens just before install (after calling rpm)

%post
#happens at install time, after installation
mysql < /usr/lib/mysql/plugin/db_install_lib_mysqludf_ta

%preun
#Runs during uninstall
mysql < /usr/lib/mysql/plugin/db_uninstall_lib_mysqludf_ta

%clean
#runs after a build (not after installation)

%files
#list of files used at build time
%defattr(644,root,root)
/usr/lib/mysql/plugin/lib_mysqludf_ta.so
/usr/lib/mysql/plugin/db_install_lib_mysqludf_ta
/usr/lib/mysql/plugin/db_uninstall_lib_mysqludf_ta
