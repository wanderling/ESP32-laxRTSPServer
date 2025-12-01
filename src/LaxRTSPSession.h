// Portions based on ESP32-RTSPServer by rjsachse (MIT License)

#pragma once

struct LaxRTSPState {
  bool didDescribe = false;
  bool didSetup = false;
  bool didPlay = false;
  bool looseMode = false;
};

class LaxRTSPSession {
public:
  enum class RequestType {
    Describe,
    Setup,
    Play,
  };

  static void reset(LaxRTSPState& state);
  static void noteDescribe(LaxRTSPState& state);
  static void noteSetup(LaxRTSPState& state);
  static void notePlay(LaxRTSPState& state);

  static bool detectAndEnableLax(LaxRTSPState& state, RequestType request);

  static bool shouldSynthesizeDescribe(const LaxRTSPState& state);
  static bool shouldAllowSetup(const LaxRTSPState& state);
  static bool shouldAllowPlay(const LaxRTSPState& state);
  static bool inLaxMode(const LaxRTSPState& state);
};
