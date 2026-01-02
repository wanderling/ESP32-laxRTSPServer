// Portions based on ESP32-RTSPServer by rjsachse (MIT License)

#include "LaxRTSPCompat.h"
#include "ESP32-RTSPServer.h"
#include <cstring>

size_t LaxRTSPCompat::buildSdpDescription(const RTSPServer& server, const RTSP_Session& session, char* out, size_t maxLen) {
  if (!out || maxLen == 0) {
    return 0;
  }

  int len = snprintf(out, maxLen,
                     "v=0\r\n"
                     "o=- %ld 1 IN IP4 %s\r\n"
                     "s=\r\n"
                     "c=IN IP4 0.0.0.0\r\n"
                     "t=0 0\r\n"
                     "a=control:*\r\n",
                     session.sessionID,
                     WiFi.localIP().toString().c_str());

  if (server.isVideo) {
    len += snprintf(out + len, maxLen - len,
                    "m=video 0 RTP/AVP 26\r\n"
                    "a=control:video\r\n");
  }

  const char* mediaCondition = "sendrecv";

  if (server.isAudio) {
    len += snprintf(out + len, maxLen - len,
                    "m=audio 0 RTP/AVP 97\r\n"
                    "a=rtpmap:97 L16/%lu/1\r\n"
                    "a=control:audio\r\n"
                    "a=%s\r\n",
                    server.sampleRate,
                    mediaCondition);
  }

  if (server.isSubtitles) {
    len += snprintf(out + len, maxLen - len,
                    "m=text 0 RTP/AVP 98\r\n"
                    "a=rtpmap:98 t140/1000\r\n"
                    "a=control:subtitles\r\n");
  }

  if (len < 0) {
    out[0] = '\0';
    return 0;
  }

  size_t finalLen = static_cast<size_t>(len);
  if (finalLen >= maxLen) {
    finalLen = maxLen - 1;
    out[finalLen] = '\0';
  }
  return finalLen;
}

void LaxRTSPCompat::ensureDescribe(RTSPServer& server, RTSP_Session& session, const char* reason) {
  if (!LaxRTSPSession::shouldSynthesizeDescribe(session.laxState)) {
    return;
  }

  char synthesized[sizeof(session.fallbackSdp)];
  size_t len = buildSdpDescription(server, session, synthesized, sizeof(synthesized));

  if (len > 0) {
    memcpy(session.fallbackSdp, synthesized, len);
    session.fallbackSdp[len] = '\0';
    session.fallbackSdpLen = static_cast<uint16_t>(len);
    session.hasFallbackSdp = true;
  } else {
    session.hasFallbackSdp = false;
    session.fallbackSdpLen = 0;
    session.fallbackSdp[0] = '\0';
  }

  LaxRTSPSession::noteDescribe(session.laxState);
  RTSP_LOGW(RTSPServer::LOG_TAG,
            "Session %u triggered fallback DESCRIBE (%s)",
            session.sessionID,
            reason ? reason : "automatic");
}

bool LaxRTSPCompat::resumeDeferredPlay(RTSP_Session& session) {
  if (!LaxRTSPSession::hasDeferredPlay(session.laxState)) {
    return false;
  }

  LaxRTSPSession::clearDeferredPlay(session.laxState);
  session.isPlaying = true;
  return true;
}
