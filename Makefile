override PLUGIN_NAME=resourcemod
override PLUGIN_ALIAS=rmod
override SDK_NAME=cs2
override TARGET_PLATFORM=x86_64

.PHONY: all
all: cleanup configure build

.PHONY: cleanup
cleanup:
	@rm ${PWD}/build/* || true

.PHONY: configure
configure:
	@cd build && \
	  python3 ../configure.py \
	  --enable-optimize \
	  --plugin-name=$(PLUGIN_NAME) \
	  --plugin-alias=$(PLUGIN_ALIAS) \
	  --sdks=$(SDK_NAME) \
	  --targets=$(TARGET_PLATFORM) \
	  --mms_path=$(MMS_PATH) \
	  --hl2sdk-root=$(HL2SDK_ROOT)

.PHONY: build
build:
	@cd build && ambuild