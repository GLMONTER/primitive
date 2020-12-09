#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/monter/Documents/primitive
  make -f /Users/monter/Documents/primitive/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/monter/Documents/primitive
  make -f /Users/monter/Documents/primitive/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/monter/Documents/primitive
  make -f /Users/monter/Documents/primitive/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/monter/Documents/primitive
  make -f /Users/monter/Documents/primitive/CMakeScripts/ReRunCMake.make
fi

