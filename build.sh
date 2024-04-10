cd "$(dirname "$0")/build"
rm -rf *

clear && cmake ../ && make && ./camera
