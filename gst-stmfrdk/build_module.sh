#!/bin/sh

set -e

while getopts cbi option
do
        case "$option" in
	c)	opt_configure=1 ;;
	b)	opt_build=1 ;;
	i)	opt_install=1 ;;
	*)    echo >&2 "$0: wrong arg: -$option"; exit 1;;
	esac
done



case "$ARCH" in
armv7)
	export CC=armv7-linux-gcc;
	export CXX=armv7-linux-g++;
	export AS=armv7-linux-as;
	export LD=armv7-linux-ld;
	export AR=armv7-linux-ar;
	export RANLIB=armv7-linux-ranlib;
	export NM=armv7-linux-nm;
	configure_arg_host=arm-cortex-linux-gnueabi
	;;
sh4)
	configure_arg_host=$ARCH-linux
	;;
*)
	echo >&2 "Wrong arch: \"$ARCH\". Aborting."
	exit 1
	;;
esac

target=${TARGET_DIR:=$STM_ROOT/devkit/$ARCH/target}
export PATH=$STM_ROOT/host/bin:$STM_ROOT/devkit/$ARCH/bin:$PATH
export PKG_CONFIG_PATH=$target/usr/lib/pkgconfig
export PKG_PREFIX_BASE=$target
export PKG_CONFIG=$STM_ROOT/host/bin/pkg-config
export PKG_CONFIG_LIBDIR=$target/usr/lib/pkgconfig
export PKG_CONFIG_SYSROOT_DIR=$target
export LIBTOOL_PREFIX_BASE=$target

run ()
{
	if [ "$SDK2_SYSROOT" = "/" ]
	then
		"$@"
	else
		proot -b $HOME -w $PWD $SDK2_SYSROOT "$@"
	fi
}

if [ "$opt_configure" ]; then
	run autoreconf --verbose --force --install || exit $?
	run ./configure --host=$configure_arg_host --build=`./config.guess` --prefix=/usr || exit $?
fi
if [ "$opt_build" ]; then
	run make || exit $?
fi
if [ "$opt_install" ]; then
	run make DESTDIR=$target install || exit $?
fi
