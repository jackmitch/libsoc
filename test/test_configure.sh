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

./configure ${CONFIGURE_FLAGS} --prefix=$(pwd)/test/tmpinstall
make
make install
find test/ -name *python* | grep -qz . && { echo "python found when not configured"; exit 1; rm -r test/tmpinstall; }
rm -r test/tmpinstall
make clean

./configure ${CONFIGURE_FLAGS} --enable-python=no --prefix=$(pwd)/test/tmpinstall
make
make install
find test/ -name *python* | grep -qz . && { echo "python found when set to no"; exit 1; rm -r test/tmpinstall; }
rm -r test/tmpinstall
make clean

if ./configure ${CONFIGURE_FLAGS} --enable-python=/usr/bin/python-nope ; then
	echo "configure succeeded when given bad python path"
	exit 1
fi

if [ $python2 -ne 0 ]; then

	./configure ${CONFIGURE_FLAGS} --enable-python --prefix=$(pwd)/test/tmpinstall
	make
	make install
	find test/ -name *python* | grep -qz . || { echo "python auto failed"; exit 1; rm -r test/tmpinstall; }
	rm -r test/tmpinstall
	make clean

	./configure ${CONFIGURE_FLAGS} --enable-python=2 --prefix=$(pwd)/test/tmpinstall
	make
	make install
	find test/ -name *python2* | grep -qz . || { echo "python2 version build failed"; exit 1; rm -r test/tmpinstall; }
	rm -r test/tmpinstall
	make clean

	./configure ${CONFIGURE_FLAGS} --enable-python=/usr/bin/python2 --prefix=$(pwd)/test/tmpinstall
	make
	make install
	find test/ -name *python2* | grep -qz . || { echo "python2 full path build failed"; exit 1; rm -r test/tmpinstall; }
	rm -r test/tmpinstall
	make clean

fi

if [ $python3 -ne 0 ]; then

	./configure ${CONFIGURE_FLAGS} --enable-python --prefix=$(pwd)/test/tmpinstall
	make
	make install
	find test/ -name *python* | grep -qz . || { echo "python auto build failed"; exit 1; rm -r test/tmpinstall; }
	rm -r test/tmpinstall
	make clean

	./configure ${CONFIGURE_FLAGS} --enable-python=3 --prefix=$(pwd)/test/tmpinstall
	make
	make install
	find test/ -name *python3* | grep -qz . || { echo "python3 version only build failed"; exit 1; rm -r test/; }
	rm -r test/tmpinstall
	make clean

	./configure ${CONFIGURE_FLAGS} --enable-python=/usr/bin/python3 --prefix=$(pwd)/test/tmpinstall
	make
	make install
	find test/ -name *python3* | grep -qz . || { echo "python3 full path build failed"; exit 1; rm -r test/; }
	rm -r test/tmpinstall
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

echo -e "\n\n\n\n.................. SUCCESS ...................\n\n\n\n"
