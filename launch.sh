#!/bin/bash

# Mythology Quiz Launcher
# Automatically detects screen resolution and launches with optimal window size

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
EXECUTABLE="$BUILD_DIR/mythologyquiz"

# Color codes
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo ""
echo -e "${CYAN}╔════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║                                                            ║${NC}"
echo -e "${CYAN}║        MYTHOLOGY QUIZ - MODERN EDITION LAUNCHER           ║${NC}"
echo -e "${CYAN}║                                                            ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    echo -e "${RED}✗ Executable not found!${NC}"
    echo "Please run setup.sh first to build the application."
    exit 1
fi

# Detect screen resolution
echo -e "${CYAN}🔍 Detecting screen resolution...${NC}"

RESOLUTION=""
if command -v xrandr >/dev/null 2>&1; then
    # Get the primary display resolution
    RESOLUTION=$(xrandr | grep " connected primary" | grep -oP '\d+x\d+' | head -n1)
    
    # If no primary, get the first connected display
    if [ -z "$RESOLUTION" ]; then
        RESOLUTION=$(xrandr | grep " connected" | grep -oP '\d+x\d+' | head -n1)
    fi
fi

# macOS detection
if [ -z "$RESOLUTION" ] && command -v system_profiler >/dev/null 2>&1; then
    RESOLUTION=$(system_profiler SPDisplaysDataType | grep Resolution | head -n1 | awk '{print $2"x"$4}')
fi

if [ -n "$RESOLUTION" ]; then
    echo -e "${GREEN}✓ Detected resolution: $RESOLUTION${NC}"
    
    # Extract width and height
    WIDTH=$(echo $RESOLUTION | cut -d'x' -f1)
    HEIGHT=$(echo $RESOLUTION | cut -d'x' -f2)
    
    # Calculate optimal window size
    WIN_WIDTH=$(echo "scale=0; $WIDTH * 0.8 / 1" | bc)
    WIN_HEIGHT=$(echo "scale=0; $HEIGHT * 0.8 / 1" | bc)
    
    # Ensure minimum size
    if [ $WIN_WIDTH -lt 1200 ]; then
        WIN_WIDTH=1200
    fi
    if [ $WIN_HEIGHT -lt 800 ]; then
        WIN_HEIGHT=800
    fi
    
    echo -e "${GREEN}✓ Optimal window size: ${WIN_WIDTH}x${WIN_HEIGHT}${NC}"
else
    echo -e "${YELLOW}⚠ Could not detect screen resolution${NC}"
    echo -e "${YELLOW}Using default size: 1400x900${NC}"
    WIN_WIDTH=1400
    WIN_HEIGHT=900
fi

echo ""
echo -e "${GREEN}🚀 Launching Mythology Quiz...${NC}"
echo ""

# Export window size as environment variables that the app can read
export MYTHOLOGY_QUIZ_WIDTH=$WIN_WIDTH
export MYTHOLOGY_QUIZ_HEIGHT=$WIN_HEIGHT

# Launch the application
cd "$BUILD_DIR"
./mythologyquiz

exit_code=$?

if [ $exit_code -ne 0 ]; then
    echo ""
    echo -e "${RED}✗ Application exited with error code: $exit_code${NC}"
    exit $exit_code
fi

echo ""
echo -e "${GREEN}Thanks for playing! 🎮✨${NC}"
echo ""

exit 0
