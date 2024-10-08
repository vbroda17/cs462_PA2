#!/bin/bash

num_runs=1000
xMax=64
yMax=64
steps=10000
total_time=0

echo "Doing $num_runs runs"
# Loop for the specified number of runs
for ((i = 1; i <= num_runs; i++)); do
    # Run myocean with input file and capture the TIME value
    time_output=$(./myocean $xMax $yMax $steps < myoceancopy.in | grep "TIME" | awk '{print $2}')
    
    # Check if the output contains TIME value
    if [[ -n "$time_output" ]]; then
        # Add the TIME value to the total_time
        total_time=$(echo "$total_time + $time_output" | bc -l)
    fi
done

# Calculate the average TIME value
average_time=$(echo "$total_time / $num_runs" | bc -l)

# Print the average TIME value
echo "Average TIME: $average_time"
