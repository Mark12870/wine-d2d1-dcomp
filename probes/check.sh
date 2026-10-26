#!/bin/bash
# Runs each probe under a Wine build that carries none of the patches in patches/, and fails
# for every probe that finds the failure its patch fixes gone. Run it inside org.gnome.Sdk//50,
# with DISPLAY naming an X server.
# usage: check.sh <unpatched wine directory> <directory holding the built probes>
set -euo pipefail

here=$(dirname "$(realpath "$0")")
wine=$(realpath "$1")/bin/wine
probes=$(realpath "$2")
work=$(mktemp -d)
trap '"$wine" wineserver -k || true; rm -rf "$work"' EXIT
export WINEPREFIX=$work/prefix WINEDEBUG=-all WINEDLLOVERRIDES="mscoree=d;mshtml=d"

"$wine" wineboot --init
openssl req -x509 -newkey rsa:2048 -nodes -subj /CN=probe -days 1 \
    -keyout "$work/key.pem" -out "$work/cert.pem" 2>/dev/null
openssl pkcs12 -export -nokeys -in "$work/cert.pem" -passout pass: -out "$work/cert-only.pfx"

stale=0
verdict() {
    echo "$1: $3"
    if [ "$3" = "$2" ]; then
        echo "::error::Wine no longer needs patches/$1; remove it and its PATCHES.md section."
        stale=1
    fi
}

said=$("$wine" "$probes/pfx-cert-only.exe" "Z:${work//\//\\}\\cert-only.pfx" | tr -d '\r')
verdict crypt32-cert-only-pfx.patch imported "${said%% *}"

"$wine" "$probes/layered-child.exe" > "$work/layered" &
for _ in $(seq 60); do
    windows=$(grep -aoE '^[0-9a-f]+ [0-9a-f]+' "$work/layered" || true)
    [ -n "$windows" ] && break
    sleep 1
done
verdict winex11-layered-child-compose.patch inside "$(python3 "$here/x-parent.py" $windows)"
wait

exit $stale
