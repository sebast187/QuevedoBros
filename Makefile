# Quevedo Bros Build System
CC = clang
CFLAGS = -Wall -std=c99 -O2

# Prefer pkg-config, fallback to Homebrew Cellar paths (Apple Silicon)
PKG_CONFIG ?= pkg-config
RAY_CFLAGS := $(shell $(PKG_CONFIG) --cflags raylib 2>/dev/null || echo -I/opt/homebrew/Cellar/raylib/5.5/include)
RAY_LIBS   := $(shell $(PKG_CONFIG) --libs   raylib 2>/dev/null || echo -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL -framework CoreVideo)

SRC_DIR = src
BUILD_DIR = build
RESOURCES_DIR = resources
TARGET = $(BUILD_DIR)/quevedo_bros
SRC = $(wildcard $(SRC_DIR)/*.c)

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(RAY_CFLAGS) $(RAY_LIBS)
	@echo "Build complete! Run './$(TARGET)' to play."

clean:
	rm -rf $(BUILD_DIR)

app: $(TARGET)
	@mkdir -p $(BUILD_DIR)/QuevedoBros.app/Contents/MacOS
	@mkdir -p $(BUILD_DIR)/QuevedoBros.app/Contents/Resources

	@# Copy Executable
	@cp $(TARGET) $(BUILD_DIR)/QuevedoBros.app/Contents/MacOS/QuevedoBros

	@# 1. Copy Icon directly into Resources so Mac Finder can see it
	@cp $(RESOURCES_DIR)/MyIcon.icns $(BUILD_DIR)/QuevedoBros.app/Contents/Resources/ || true
	
	@# 2. Copy the whole resources folder so the C code can find "resources/bgm.mp3"
	@cp -R $(RESOURCES_DIR) $(BUILD_DIR)/QuevedoBros.app/Contents/Resources/

	@echo '<?xml version="1.0" encoding="UTF-8"?>\
	<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">\
	<plist version="1.0">\
	<dict>\
		<key>CFBundleExecutable</key>\
		<string>QuevedoBros</string>\
		<key>CFBundleIdentifier</key>\
		<string>com.indie.quevedobros</string>\
		<key>CFBundleName</key>\
		<string>Quevedo Bros</string>\
		<key>CFBundleIconFile</key>\
		<string>MyIcon.icns</string>\
		<key>CFBundleVersion</key>\
		<string>4.1</string>\
		<key>LSMinimumSystemVersion</key>\
		<string>10.15</string>\
	</dict>\
	</plist>' > $(BUILD_DIR)/QuevedoBros.app/Contents/Info.plist

	@# Force macOS to refresh the icon cache
	@touch $(BUILD_DIR)/QuevedoBros.app
	@echo "App Bundle created at $(BUILD_DIR)/QuevedoBros.app"

.PHONY: all clean app