#!/bin/bash

num_runs=100
xMax=128
yMax=128
steps=10000

echo "Doing $num_runs runs each"
# Loop for different thread counts
for threads in 1 2 4 8 16; do
    total_time=0

    for ((i = 1; i <= num_runs; i++)); do
        time_output=$(./myocean-omp $xMax $yMax $steps $threads < myoceancopy2.in | grep "TIME" | awk '{print $2}')
        
        if [[ -n "$time_output" ]]; then
            total_time=$(echo "$total_time + $time_output" | bc -l)
        fi
    done

    average_time=$(echo "$total_time / $num_runs" | bc -l)
    echo "Average TIME with $threads threads: $average_time"
done
