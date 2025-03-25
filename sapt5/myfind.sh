#!/bin/bash
dir="$1"
if [ ! -d "$dir" ]; then
    echo "nu este directory" >&2
    exit 1
elif [ ! -r "$dir" ] || [ ! -x "$dir" ]; then
    echo "nu este readable/executable"
    exit 2
else
    echo "este fisier valid"
fi
script_dir=$(dirname "$0")
second_script="${script_dir}/subdirectory/script.sh"

if [ ! -f "$second_script" ]; then
    echo "nu exista script-ul" >&2
    exit 3
fi

sh "$second_script"
exit_code=$?

echo "$exit_code"

exit 0
