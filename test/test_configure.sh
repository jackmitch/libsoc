#! /bin/sh -e

python2=1
python3=1

while [[ $# > 0 ]]
do

key="$1"

	case $key in
		--no-python2)
			python2=0
			;;
		--no-python3)
			python3=0
			;;
		*)
			echo "unrecognised argument"
			exit 1
			;;
	esac

	shift
done

cd ..

autoreconf -i
./configure ${CONFIGURE_FLAGS}
make
make clean

if [ $python2 -ne 0 ]; then
	./configure ${CONFIGURE_FLAGS} #--enable-python=2
	make
	test -e ../bindings/python/gpio_python.lo || echo "python2 build failed" && exit 1
	make clean
fi

if [ $python3 -ne 0 ]; then
	./configure ${CONFIGURE_FLAGS} --enable-python=3
	make
	test -e ../bindings/python/gpio_python.lo || echo "python3 build failed" && exit 1
	make clean
fi

./configure ${CONFIGURE_FLAGS} --enable-board=beaglebone_black
make
make clean

./configure ${CONFIGURE_FLAGS} --with-board-configs
make
make clean

./configure ${CONFIGURE_FLAGS} --disable-debug
make
make clean
