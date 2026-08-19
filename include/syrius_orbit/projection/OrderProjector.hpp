#pragma once

#include <string>

#include "syrius_orbit/db/OrdersRepository.hpp"
#include "syrius_orbit/db/Vda5050EventsRepository.hpp"

namespace syrius_orbit {

/// Projects order events into the orders read table.
class OrderProjector {
 public:
  explicit OrderProjector(OrdersRepository& orders_repo);

  /// Returns the VDA5050 topic this projector handles.
  static std::string topic() { return "order"; }

  /// Parses the event payload and upserts order-related columns.
  void project(const Vda5050Event& event);

 private:
  OrdersRepository& orders_repo_;
};

}  // namespace syrius_orbit