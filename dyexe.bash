
if [ "$(uname)" = "Darwin" ]; then
  DYLD_LIBRARY_PATH=$(pwd)/lib/libnet/ $*
else
  LD_LIBRARY_PATH=$(pwd)/lib/libnet/ $*
fi
