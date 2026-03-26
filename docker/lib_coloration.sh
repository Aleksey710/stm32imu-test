#!/bin/bash 

#-----------------------------------------------------------------------
# Define colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Functions
function print_header() {
    echo -e "${YELLOW}#######################################################################${NC}"
    echo -e "${YELLOW}### ${1}${NC}"
    echo -e "${YELLOW}#######################################################################${NC}"
}

function print_step() {
    currdate=$(date +"%Y-%m-%d %H:%M:%S")
    echo -e "${currdate} ${BLUE}[ * ] ${1}${NC}"
}

function print_debug() {
    currdate=$(date +"%Y-%m-%d %H:%M:%S")
    echo -e "${currdate} ${PURPLE}[ * ] ${1}${NC}"
}

function print_success() {
    currdate=$(date +"%Y-%m-%d %H:%M:%S")
    echo -e "${currdate} ${GREEN}[ ✓ ] ${1}${NC}"
}

function print_error() {
    currdate=$(date +"%Y-%m-%d %H:%M:%S")
    echo -e "${currdate} ${RED}[ ✗ ] ${1}${NC}"
}

#-----------------------------------------------------------------------
#print_header "Заголовок"
#print_step "Шаг выполнения"
