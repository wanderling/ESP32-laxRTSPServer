// Portions based on ESP32-RTSPServer by rjsachse (MIT License)

#pragma once

#include <cstddef>
#include "LaxRTSPSession.h"

struct RTSP_Session;
class RTSPServer;

class LaxRTSPCompat {
public:
  static size_t buildSdpDescription(const RTSPServer& server, const RTSP_Session& session, char* out, size_t maxLen);
  static void ensureDescribe(RTSPServer& server, RTSP_Session& session, const char* reason);
  static bool resumeDeferredPlay(RTSP_Session& session);
};
