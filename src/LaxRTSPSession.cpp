// Portions based on ESP32-RTSPServer by rjsachse (MIT License)

#include "LaxRTSPSession.h"

namespace {
bool needsDescribeBefore(const LaxRTSPState& state) {
  return !(state.didDescribe || state.looseMode);
}

bool needsSetupBeforePlay(const LaxRTSPState& state) {
  return !(state.didSetup || state.looseMode);
}
}  // namespace

void LaxRTSPSession::reset(LaxRTSPState& state) {
  state.didDescribe = false;
  state.didSetup = false;
  state.didPlay = false;
  state.looseMode = false;
  state.pendingPlay = false;
}

bool LaxRTSPSession::detectAndEnableLax(LaxRTSPState& state, RequestType request) {
  bool violation = false;

  switch (request) {
    case RequestType::Describe:
      violation = state.didSetup || state.didPlay;
      break;
    case RequestType::Setup:
      violation = needsDescribeBefore(state);
      break;
    case RequestType::Play:
      violation = needsSetupBeforePlay(state);
      break;
  }

  if (violation && !state.looseMode) {
    state.looseMode = true;
  }

  return violation;
}

void LaxRTSPSession::noteDescribe(LaxRTSPState& state) {
  state.didDescribe = true;
}

void LaxRTSPSession::noteSetup(LaxRTSPState& state) {
  state.didSetup = true;
}

void LaxRTSPSession::notePlay(LaxRTSPState& state) {
  state.didPlay = true;
}

bool LaxRTSPSession::shouldSynthesizeDescribe(const LaxRTSPState& state) {
  return state.looseMode && !state.didDescribe;
}

bool LaxRTSPSession::shouldAllowSetup(const LaxRTSPState& state) {
  return state.looseMode || state.didDescribe;
}

bool LaxRTSPSession::shouldAllowPlay(const LaxRTSPState& state) {
  return state.looseMode || state.didSetup;
}

bool LaxRTSPSession::inLaxMode(const LaxRTSPState& state) {
  return state.looseMode;
}

void LaxRTSPSession::flagDeferredPlay(LaxRTSPState& state) {
  state.pendingPlay = true;
}

bool LaxRTSPSession::hasDeferredPlay(const LaxRTSPState& state) {
  return state.pendingPlay;
}

void LaxRTSPSession::clearDeferredPlay(LaxRTSPState& state) {
  state.pendingPlay = false;
}
