#!/bin/bash

if [[ $# -eq 0 ]]; then
    echo "Introduceți numerele separate prin spațiu:"
    read -a numbers  
else
    numbers=("$@")  
fi

read p

if ! [[ $p =~ ^-?[0-9]+$ ]]; then
    echo "p trebuie sa fie un număr întreg!" >&2
    exit 1
fi

for num in "${numbers[@]}"; do
    if [[ $num -le $p ]]; then
        echo -n "$num " 
    fi
done

echo  
