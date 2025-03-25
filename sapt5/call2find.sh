#!/bin/bash
if [ $# -eq 0 ]; then
    read -a n    
else
    n=$1
fi

if [ ! find "$HOME" -type f -maxdepth "$n" -mtime -28 -print0 > /tmp/find_output.txt 2>/dev/null ]; then
    echo "Command has failed" >&2
    exit 1
fi
while IFS= read -r -d $'\0' file; do
    if [ file "$file" | grep -q "C source" ]; then
        output="${file%.*}"
        if [ gcc "$file" -o "$output" 2>/dev/null ]; then
            echo "$file -> Compilat ok!"
        else
            echo "$file -> Eroare la compilare!" >&2
        fi
    else
        date_mod=$(date -r "$file" "+%Y-%m-%d %H:%M:%S")
        echo "$date_mod $file"
    fi
done < /tmp/find_output.txt
rm -f /tmp/find_output.txt
