/*
 * KORUZA nodewatcher-agent module
 *
 * Copyright (C) 2016 Jernej Kos <jernej@kos.mx>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <nodewatcher-agent/module.h>
#include <nodewatcher-agent/json.h>
#include <nodewatcher-agent/utils.h>

#include <syslog.h>

static int nw_general_start_acquire_data(struct nodewatcher_module *module,
                                         struct ubus_context *ubus,
                                         struct uci_context *uci)
{
  json_object *object = json_object_new_object();

  // Get current status of the KORUZA device.
  uint32_t ubus_id;
  if (ubus_lookup_id(ubus, "koruza", &ubus_id)) {
    return nw_module_finish_acquire_data(module, object);
  }

  json_object *data = NULL;
  static struct blob_buf req;
  blob_buf_init(&req, 0);

  if (ubus_invoke(ubus, ubus_id, "get_status", req.head, nw_json_from_ubus, &data, 500) != UBUS_STATUS_OK) {
    syslog(LOG_WARNING, "koruza: Failed to invoke get_status method!");
    return nw_module_finish_acquire_data(module, object);
  }

  if (!data) {
    syslog(LOG_WARNING, "koruza: Failed to parse status data!");
    return nw_module_finish_acquire_data(module, object);
  }

  json_object_object_add(object, "status", data);

  return nw_module_finish_acquire_data(module, object);
}

static int nw_general_init(struct nodewatcher_module *module,
                           struct ubus_context *ubus,
                           struct uci_context *uci)
{
  return 0;
}

// Module descriptor.
struct nodewatcher_module nw_module = {
  .name = "irnas.koruza",
  .author = "Jernej Kos <jernej@kos.mx>",
  .version = 1,
  .hooks = {
    .init               = nw_general_init,
    .start_acquire_data = nw_general_start_acquire_data,
  },
  .schedule = {
    .refresh_interval = 30,
  },
};
