#!/bin/bash

# Output file
output_file="concatenated.c"

# List of files to skip
skip_files=("stb_image.h")

# Clear the output file if it already exists
> "$output_file"

# Function to check if a file is in the skip list
should_skip() {
    local file="$1"
    for skip in "${skip_files[@]}"; do
        if [[ "$file" == *"$skip" ]]; then
            return 0
        fi
    done
    return 1
}

# Find all .c and .h files recursively within the src folder and loop through each file
find ./src -type f \( -name "*.c" -o -name "*.h" \) | while read -r file; do
    # Skip files in the skip list
    if should_skip "$file"; then
        echo "Skipping $file"
        continue
    fi
    # Write the start marker, file content, and end marker
    echo "// $file" >> "$output_file"
    cat "$file" >> "$output_file"
    echo -e "\n// $file end\n" >> "$output_file"
done

echo "All specified files within the src folder have been recursively concatenated into $output_file"
