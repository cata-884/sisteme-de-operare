#!/bin/bash

target_dir="$1"
total_bash=0

while IFS= read -r -d '' file; do
    if [ file -b "$file" | grep -q "Bourne-Again shell script" ]; then
        sha=$(sha256sum "$file" | awk '{print $1}')
        echo "$sha $file"
        ((total_bash++))
    else
        newline_count=$(tr -dc '\n' < "$file" | wc -c)
        echo "$file $newline_count" >&2
    fi
done < <(find "$target_dir" -type f -print0)

echo "script-uri bash: $total_bash"
exit 0
