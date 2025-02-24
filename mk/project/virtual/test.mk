# common libraries for -test variants

#MODULES += app/stringtests
#MODULES += app/tests
#MODULES += arch/test
MODULES += lib/aes
MODULES += lib/aes/test
MODULES += lib/cksum
MODULES += lib/debugcommands
MODULES += lib/unittest
MODULES += lib/version

# set a build system variable for other modules to include test code
# on their own.
WITH_TESTS := true
GLOBAL_DEFINES += WITH_TESTS=1

