#!/bin/bash

RANGE_SIZE=7340032
#GRAIN_SIZE=4096
REP=100
SEED=1253513412
SIZES=(512 1024 2048 4096 8192 16384)
HEAD=head

for GRAIN_SIZE in ${SIZES[@]}; do
    ./test ori vector double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED} ${HEAD}
    HEAD=
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test det vector double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_ori vector double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_det vector double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done

for GRAIN_SIZE in ${SIZES[@]}; do
    ./test ori vector float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test det vector float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_ori vector float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_det vector float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done

for GRAIN_SIZE in ${SIZES[@]}; do
    ./test ori deque double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test det deque double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_ori deque double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_det deque double ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done

for GRAIN_SIZE in ${SIZES[@]}; do
    ./test ori deque float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test det deque float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_ori deque float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done
for GRAIN_SIZE in ${SIZES[@]}; do
    ./test stab_det deque float ${RANGE_SIZE} ${GRAIN_SIZE} ${REP} ${SEED}
done

#./test ori concurrent_vector double ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}
#./test det concurrent_vector double ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}
#./test stab_ori concurrent_vector double ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}
#./test stab_det concurrent_vector double ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}

#./test ori concurrent_vector float ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}
#./test det concurrent_vector float ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}
#./test stab_ori concurrent_vector float ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}
#./test stab_det concurrent_vector float ${RANGE_SIZE} ${REP} ${GRAIN_SIZE} ${SEED}

