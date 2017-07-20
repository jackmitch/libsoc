CPATH += ../lib/include
export
all: adc_test.test
%: %.c
	gcc -o $@ $< -lsoc
%.test: %
	./$<
