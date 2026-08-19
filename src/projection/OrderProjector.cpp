#include "syrius_orbit/projection/OrderProjector.hpp"

#include <plog/Log.h>

#include "syrius_orbit/vda5050_schemas/Order.hpp"

namespace syrius_orbit {

OrderProjector::OrderProjector(OrdersRepository& orders_repo)
    : orders_repo_(orders_repo) {}

void OrderProjector::project(const Vda5050Event& event) {
  auto msg = nlohmann::json::parse(event.payload).get<vda5050::Order>();

  orders_repo_.upsertFromOrder(
      msg.orderId, event.robot_id, msg.orderUpdateId,
      msg.orderDescription.value_or(""),
      nlohmann::json(msg.nodes).dump(),
      nlohmann::json(msg.edges).dump(),
      event.received_at);
}

}  // namespace syrius_orbit