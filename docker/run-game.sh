#!/usr/bin/env bash
set -euo pipefail

cleanup() {
    jobs -p | xargs -r kill
}
trap cleanup EXIT

XVFB_SCREEN="${SCREEN_WIDTH}x${SCREEN_HEIGHT}x24"

Xvfb "${DISPLAY}" -screen 0 "${XVFB_SCREEN}" -ac +extension GLX +render -noreset &
sleep 1

fluxbox >/tmp/fluxbox.log 2>&1 &
x11vnc -display "${DISPLAY}" -forever -shared -nopw -rfbport 5900 >/tmp/x11vnc.log 2>&1 &
websockify --web=/usr/share/novnc 6080 localhost:5900 >/tmp/novnc.log 2>&1 &

echo "Game is running in Linux. Open http://localhost:6080/vnc.html in your browser."

exec ./build/I_HAVE_A_DREAM
