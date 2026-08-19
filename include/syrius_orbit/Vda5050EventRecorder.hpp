#pragma once

#include <cstdint>
#include <string>

#include "syrius_orbit/db/Vda5050EventsRepository.hpp"
#include "syrius_orbit/mqtt/VDA5050EndPoint.hpp"

namespace syrius_orbit {

/// Attaches event recording callbacks to VDA5050EndPoint instances.
///
/// Registers listeners on both local and cloud endpoints that construct
/// Vda5050Event records from incoming VDA5050 messages and write them to
/// the vda5050_events table via the provided repository.
///
/// Usage:
///   Vda5050EventRecorder recorder(local_ep, cloud_ep, repo);
///   recorder.attach();  // registers 8 listeners (4 uplink, 4 downlink)
class Vda5050EventRecorder {
 public:
  Vda5050EventRecorder(VDA5050EndPoint *local_ep,
                       VDA5050EndPoint *cloud_ep,
                       Vda5050EventsRepository &repo);

  /// Registers record callbacks on both endpoints. Safe to call after
  /// the endpoints have been initialized.
  void attach();

 private:
  static void insertEvent(Vda5050EventsRepository &repo,
                          const std::string &topic,
                          const std::string &item_id,
                          const VDA5050TopicContext &context,
                          const std::string &direction,
                          std::int64_t header_id,
                          const std::string &version,
                          const std::string &timestamp,
                          const std::string &payload);

  VDA5050EndPoint *local_ep_;
  VDA5050EndPoint *cloud_ep_;
  Vda5050EventsRepository &repo_;
};

}  // namespace syrius_orbit