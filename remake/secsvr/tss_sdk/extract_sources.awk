# Extract the `for f in \ ... ; do` file list from link_check.sh.
/^for f in \\$/ { inlist = 1; next }
inlist {
    line = $0
    sub(/^[ \t]+/, "", line)
    if (line ~ /; do$/) {
        sub(/; do$/, "", line)
        sub(/[ \t]+$/, "", line)
        print line
        inlist = 0
    } else {
        sub(/\\$/, "", line)
        sub(/[ \t]+$/, "", line)
        print line
    }
}
