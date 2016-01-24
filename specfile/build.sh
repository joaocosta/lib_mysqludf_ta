#!/bin/sh

set -euo pipefail

cd ~/
rm -fR ~/rpmbuild
rpmdev-setuptree
ln -vs lib_mysqludf_ta/ lib_mysqludf_ta-0.1
tar --exclude=.git -hczPf ~/rpmbuild/SOURCES/lib_mysqludf_ta-0.1.tar.gz lib_mysqludf_ta-0.1/
rm lib_mysqludf_ta-0.1
rpmbuild -v -ba --clean --sign lib_mysqludf_ta/specfile/lib_mysqludf_ta.spec
