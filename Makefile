CXX = icpc
OPTS = -std=c++11 -qopenmp-simd -O3 -xHost

test:test_pstl_partition.cpp pstl_partition.h
	$(CXX) $(OPTS) $< -o $@ -ltbb
