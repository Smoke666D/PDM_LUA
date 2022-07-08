# Version
VER_MAJOR := $(word 1,$(shell grep "FIRMWARE_VERSION_MAJOR" ./common/Inc/version.h | awk '{print $$3}' ))
VER_MINOR := $(word 1,$(shell grep "FIRMWARE_VERSION_MINOR" ./common/Inc/version.h | awk '{print $$3}' ))
VER_PATCH := $(word 1,$(shell grep "FIRMWARE_VERSION_PATCH" ./common/Inc/version.h | awk '{print $$3}' ))
VER       := $(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)