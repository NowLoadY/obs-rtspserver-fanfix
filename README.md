# OBS RTSP Server Plugin - Fan Fix Version

🎯 **Qt6 Compatibility Fix for Ubuntu 25.04 + OBS Studio 32.0.0**

This is a community-maintained compatibility fix for the excellent [OBS RTSP Server Plugin](https://github.com/iamscottxu/obs-rtspserver) by [iamscottxu](https://github.com/iamscottxu).

## 🚨 Why This Fix?

The original plugin has Qt5/Qt6 compatibility issues on newer systems:
- Ubuntu 25.04 ships with Qt6
- OBS Studio 32.0.0 uses Qt6
- Original plugin was built for Qt5

## ✅ What's Fixed?

- **Qt6 Compatibility**: Full Qt6 support
- **Compilation Errors**: Fixed missing headers
- **Stability**: Improved Qt object lifecycle management
- **Error Handling**: Better error reporting and logging

## 🧪 Tested Environment

- **OS**: Ubuntu 25.04
- **OBS Studio**: 32.0.0
- **Qt Version**: 6.8.3
- **Encoding**: x264 (software) + AAC audio

## 🚀 Quick Install

```bash
# Install dependencies
sudo apt install qt6-base-dev cmake build-essential libsimde-dev

# Clone and build
git clone https://github.com/YOUR_USERNAME/obs-rtspserver-fanfix.git
cd obs-rtspserver-fanfix
mkdir build && cd build

# Configure with correct paths
cmake -DLIBOBS_INCLUDE_DIR=/usr/include/obs \
      -DLIBOBS_LIB=/usr/lib/x86_64-linux-gnu/libobs.so \
      -DOBS_FRONTEND_API_INCLUDE_DIR=/usr/include/obs \
      -DOBS_FRONTEND_API_LIB=/usr/lib/x86_64-linux-gnu/libobs-frontend-api.so \
      ..

# Build and install
make -j$(nproc)
sudo cp obs-rtspserver.so /usr/lib/x86_64-linux-gnu/obs-plugins/
```

## 🎮 Usage

1. Start OBS Studio
2. Go to **Tools** → **RtspServer**
3. Configure settings (use port 8554 to avoid permission issues)
4. Click **Start Output**
5. Stream URL: `rtsp://localhost:8554/live` or `rtsp://[ip]:8554/live`

## 📺 Tested Players

- ✅ **MPV**: `mpv rtsp://[ip]:8554/live` (Recommended)
- ✅ **FFplay**: `ffplay rtsp://[ip]:8554/live`
- ✅ **VLC**

## 🙏 Credits

- **Original Author**: [iamscottxu](https://github.com/iamscottxu)
- **Original Project**: [obs-rtspserver](https://github.com/iamscottxu/obs-rtspserver)
- **This Fix**: Community contribution for Qt6 compatibility

## 📄 License

Same as original project (GPL-2.0)

## 🐛 Issues?

If you encounter problems:
1. Check [MODIFICATIONS.md](MODIFICATIONS.md) for detailed changes
2. Ensure you're using x264 encoder (not NVENC)
3. Try port 8554 instead of 554
4. Use MPV instead of VLC for playback

---

**This is a fan-maintained compatibility fix. All credit goes to the original author!** ❤️
