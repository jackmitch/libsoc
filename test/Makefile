CPATH += ../lib/include
LIBRARY_PATH := ../lib/.libs $(LIBRARY_PATH)
export
test: adc_test adc_test.test
%: %.c
	gcc -o $@ $< -lsoc
%.test: %
	./$<
