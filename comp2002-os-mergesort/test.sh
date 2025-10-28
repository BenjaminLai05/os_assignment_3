#!/bin/bash

run_visible_test() {
    size=$1
    cutoff=$2
    seed=$3

    echo "Test: Size=$size, Cutoff=$cutoff, Seed=$seed"
    echo ""
    ./test-mergesort $size $cutoff $seed
    if [ $? -eq 0 ]; then
        echo ""
        echo "PASSED"
        echo""
    else
        echo ""
        echo "FAILED"
        echo""
        
    fi
    echo ""

}

run_visible_test 10 1 42
run_visible_test 10 2 42
run_visible_test 20 0 123
run_visible_test 20 1 123
run_visible_test 20 2 123