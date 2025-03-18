#!/bin/bash

calculate_power() {
    local k=$1
    if [[ $k -eq 0 ]]; then
        echo "2^0 = 1" >&2  
    else
        calculate_power $((k - 1))  
        echo "2^$k = $((2 * (2**(k-1))))" >&2  
    fi
}

if [[ -z $1 ]]; then
    read -p n
else
    n=$1
fi

if ! [[ $n =~ ^[0-9]+$ ]]; then
    echo "n trebuie să fie un numar întreg pozitiv!" >&2
    exit 1
fi

calculate_power "$n"

