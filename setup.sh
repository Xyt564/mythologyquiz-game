#!/bin/bash

echo "╔════════════════════════════════════════════════════════════╗"
echo "║                                                            ║"
echo "║        MYTHOLOGY QUIZ - MODERN EDITION SETUP               ║"
echo "║        Powered by Dear ImGui + OpenGL + GLFW               ║"
echo "║                                                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Color codes for pretty output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
else
    echo -e "${RED}✗ Unsupported OS: $OSTYPE${NC}"
    echo "This script supports Linux and macOS only."
    echo "For Windows, please install dependencies manually and use CMake."
    exit 1
fi

echo -e "${GREEN}✓ Detected OS: $OS${NC}"
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install dependencies based on OS
echo -e "${CYAN}📦 Installing dependencies...${NC}"
echo ""

if [ "$OS" = "linux" ]; then
    # Detect Linux distribution
    if command_exists apt-get; then
        echo -e "${BLUE}Detected Debian/Ubuntu-based system${NC}"
        sudo apt-get update
        sudo apt-get install -y build-essential cmake git pkg-config
        
        # Install OpenGL and GLFW dependencies
        echo -e "${YELLOW}Installing OpenGL and GLFW...${NC}"
        sudo apt-get install -y libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev libxrandr-dev \
            libxinerama-dev libxcursor-dev libxi-dev libx11-dev
        
        # Install TrueType fonts for better text rendering
        echo -e "${YELLOW}Installing TrueType fonts...${NC}"
        sudo apt-get install -y fonts-dejavu-core fonts-liberation ttf-mscorefonts-installer
        
        echo -e "${GREEN}✓ All dependencies installed!${NC}"
        
    elif command_exists dnf; then
        echo -e "${BLUE}Detected Fedora-based system${NC}"
        sudo dnf groupinstall -y "Development Tools"
        sudo dnf install -y cmake git pkg-config
        sudo dnf install -y glfw-devel mesa-libGL-devel mesa-libGLU-devel \
            libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
        
        # Install TrueType fonts
        echo -e "${YELLOW}Installing TrueType fonts...${NC}"
        sudo dnf install -y dejavu-sans-fonts liberation-sans-fonts
        
        echo -e "${GREEN}✓ All dependencies installed!${NC}"
        
    elif command_exists yum; then
        echo -e "${BLUE}Detected RHEL/CentOS-based system${NC}"
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y cmake git pkg-config
        sudo yum install -y glfw-devel mesa-libGL-devel mesa-libGLU-devel
        
        # Install TrueType fonts
        echo -e "${YELLOW}Installing TrueType fonts...${NC}"
        sudo yum install -y dejavu-sans-fonts liberation-sans-fonts
        
        echo -e "${GREEN}✓ All dependencies installed!${NC}"
        
    elif command_exists pacman; then
        echo -e "${BLUE}Detected Arch-based system${NC}"
        sudo pacman -Syu --noconfirm
        sudo pacman -S --noconfirm base-devel cmake git pkg-config glfw-x11 mesa
        
        # Install TrueType fonts
        echo -e "${YELLOW}Installing TrueType fonts...${NC}"
        sudo pacman -S --noconfirm ttf-dejavu ttf-liberation
        
        echo -e "${GREEN}✓ All dependencies installed!${NC}"
    else
        echo -e "${RED}✗ Unsupported Linux distribution${NC}"
        echo "Please install the following manually:"
        echo "  - build-essential (gcc, g++, make)"
        echo "  - cmake"
        echo "  - git"
        echo "  - GLFW development libraries"
        echo "  - OpenGL development libraries"
        exit 1
    fi
    
elif [ "$OS" = "macos" ]; then
    echo -e "${BLUE}Detected macOS${NC}"
    
    # Check for Homebrew
    if ! command_exists brew; then
        echo -e "${YELLOW}Installing Homebrew...${NC}"
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    else
        echo -e "${GREEN}✓ Homebrew already installed${NC}"
    fi
    
    # Install dependencies
    echo -e "${YELLOW}Installing build tools and libraries...${NC}"
    brew install cmake glfw
    
    echo -e "${GREEN}✓ All dependencies installed!${NC}"
fi

echo ""

# Check if CMake is available
if ! command_exists cmake; then
    echo -e "${RED}✗ CMake installation failed. Please install manually.${NC}"
    exit 1
fi

# Download Dear ImGui if not present
IMGUI_VERSION="v1.91.5"
if [ ! -d "imgui" ]; then
    echo -e "${CYAN}📥 Downloading Dear ImGui ${IMGUI_VERSION}...${NC}"
    
    if command_exists git; then
        git clone --depth 1 --branch ${IMGUI_VERSION} https://github.com/ocornut/imgui.git
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ Dear ImGui downloaded successfully!${NC}"
        else
            echo -e "${RED}✗ Failed to download Dear ImGui${NC}"
            exit 1
        fi
    else
        echo -e "${RED}✗ Git is not installed. Cannot download Dear ImGui.${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}✓ Dear ImGui already present${NC}"
fi

echo ""
echo -e "${CYAN}🔨 Building the project...${NC}"
echo ""

# Create build directory
if [ -d "build" ]; then
    echo -e "${YELLOW}Cleaning old build directory...${NC}"
    rm -rf build
fi

mkdir build
cd build

# Configure with CMake
echo -e "${BLUE}Running CMake configuration...${NC}"
if ! cmake ..; then
    echo ""
    echo -e "${RED}✗ CMake configuration failed!${NC}"
    echo ""
    echo "Troubleshooting tips:"
    echo "1. Make sure GLFW is properly installed"
    echo "2. Check that OpenGL development files are present"
    echo "3. Verify that you have a C++ compiler installed"
    exit 1
fi

echo ""
echo -e "${BLUE}Compiling application...${NC}"
if ! cmake --build . --config Release; then
    echo ""
    echo -e "${RED}✗ Build failed!${NC}"
    exit 1
fi

echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                                                            ║"
echo "║              ✅ BUILD SUCCESSFUL! 🎉                       ║"
echo "║                                                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo -e "${GREEN}Your modern mythology quiz is ready to run!${NC}"
echo ""
echo "To launch the application:"
echo ""
echo -e "${CYAN}══════════════════════════════════════════════════════${NC}"
echo -e "${GREEN}Enjoy your modern quiz experience! 🎮✨${NC}"
echo -e "${CYAN}══════════════════════════════════════════════════════${NC}"
echo ""

if [ -f "../launch.sh" ]; then
    echo -e "${BLUE}Setting launch.sh as executable...${NC}"
    chmod +x ../launch.sh
    echo -e "${GREEN}launch.sh is ready to run.${NC}"
else
    echo -e "${RED}Warning: launch.sh not found.${NC}"
fi

echo ""
echo -e "${YELLOW}You can start the application using one of the following options:${NC}"
echo -e "${YELLOW}  1) Recommended: Run the launch script from the project root:${NC}"
echo -e "${YELLOW}       ./launch.sh${NC}"
echo -e "${YELLOW}  2) Or run the executable directly from the build directory:${NC}"
echo -e "${YELLOW}       cd build${NC}"
echo -e "${YELLOW}       ./mythologyquiz${NC}"
echo ""