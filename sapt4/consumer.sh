#!/bin/bash

sum=0
read -a numbers

if [[ ${#numbers[@]} -eq 0 ]]; then
    echo "Consumer: computed sum is NULL."
    exit 1
fi

for num in "${numbers[@]}"; do
    sum=$((sum + num * num * num))
done

echo "Consumer: computed sum is: $sum."
exit 0

