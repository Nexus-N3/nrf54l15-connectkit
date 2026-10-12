/*
 * Copyright (c) 2026 Nexus N3 contributors
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include "bridge_stats.h"

struct bridge_stats g_bridge_stats;

#define VALUE(field) ((unsigned int)(uint32_t)atomic_get(&g_bridge_stats.field))

static int cmd_bridge_stats(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "uptime_ms=%u", k_uptime_get_32());
    shell_print(sh, "uart_rx events=%u bytes=%u last_ms=%u consumed=%u",
                VALUE(uart_rx_events), VALUE(uart_rx_bytes),
                VALUE(uart_rx_last_ms), VALUE(uart_rx_events_consumed));
    shell_print(sh, "uart_rx buffers alloc=%u freed=%u alloc_fail=%u rsp_fail=%u",
                VALUE(uart_rx_buf_allocs), VALUE(uart_rx_buf_frees),
                VALUE(uart_rx_buf_alloc_fails), VALUE(uart_rx_buf_rsp_fails));
    shell_print(sh, "uart_rx stopped=%u reason=%d disabled=%u enable_ok=%u enable_fail=%u",
                VALUE(uart_rx_stopped),
                (int)atomic_get(&g_bridge_stats.uart_rx_stop_reason),
                VALUE(uart_rx_disabled), VALUE(uart_rx_enable_ok),
                VALUE(uart_rx_enable_fails));
    shell_print(sh, "uart_rx recovery attempts=%u success=%u failures=%u last_err=%d",
                VALUE(uart_rx_recovery_attempts), VALUE(uart_rx_recovery_success),
                VALUE(uart_rx_recovery_failures),
                (int)atomic_get(&g_bridge_stats.uart_rx_recovery_last_error));
    shell_print(sh, "cdc_dtr=%u cdc_host_rx events=%u bytes=%u overflow_bytes=%u",
                VALUE(cdc_dtr), VALUE(cdc_host_rx_events),
                VALUE(cdc_host_rx_bytes), VALUE(cdc_host_rx_overflow_bytes));
    shell_print(sh, "cdc_to_uart enqueued_bytes=%u dropped_bytes=%u tx_start_fail=%u tx_aborted=%u",
                VALUE(uart_tx_enqueued_bytes), VALUE(uart_tx_dropped_bytes),
                VALUE(uart_tx_start_fails), VALUE(uart_tx_aborted));
    shell_print(sh, "uart_to_cdc events=%u requested=%u written=%u dropped=%u no_dtr=%u last_ms=%u",
                VALUE(uart_to_cdc_events), VALUE(uart_to_cdc_requested_bytes),
                VALUE(uart_to_cdc_written_bytes), VALUE(uart_to_cdc_dropped_bytes),
                VALUE(uart_to_cdc_no_dtr_bytes), VALUE(uart_to_cdc_last_ms));
    shell_print(sh, "usb errors=%u resets=%u disconnects=%u",
                VALUE(usb_error_events), VALUE(usb_reset_events),
                VALUE(usb_disconnect_events));
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_bridge,
    SHELL_CMD(stats, NULL, "Show USB/UART bridge counters", cmd_bridge_stats),
    SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(bridge, &sub_bridge, "USB/UART bridge diagnostics", NULL);
