#!/bin/sh

rm -fR ~/myrpms/
mkdir -p ~/myrpms/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
tar -czPf ~/myrpms/SOURCES/lib_mysqludf_ta-0.1.tar.gz src/*
rpmbuild -v -bb --clean specfile/lib_mysqludf_ta.spec



rm -fR ~/rpmbuild
rpmdev-setuptree
ln -vs lib_mysqludf_ta/ lib_mysqludf_ta-0.1
tar -hczPf ~/rpmbuild/SOURCES/lib_mysqludf_ta-0.1.tar.gz lib_mysqludf_ta-0.1/
rm lib_mysqludf_ta-0.1
rpmbuild -v -bb --clean lib_mysqludf_ta/specfile/lib_mysqludf_ta.spec
