/*
 * Copyright (c) 2026 Nexus N3 contributors
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#ifndef IFMCU_BRIDGE_STATS_H_
#define IFMCU_BRIDGE_STATS_H_

#include <zephyr/sys/atomic.h>

struct bridge_stats {
    atomic_t uart_rx_events;
    atomic_t uart_rx_bytes;
    atomic_t uart_rx_last_ms;
    atomic_t uart_rx_buf_allocs;
    atomic_t uart_rx_buf_frees;
    atomic_t uart_rx_buf_alloc_fails;
    atomic_t uart_rx_buf_rsp_fails;
    atomic_t uart_rx_stopped;
    atomic_t uart_rx_stop_reason;
    atomic_t uart_rx_disabled;
    atomic_t uart_rx_enable_ok;
    atomic_t uart_rx_enable_fails;
    atomic_t uart_rx_recovery_attempts;
    atomic_t uart_rx_recovery_success;
    atomic_t uart_rx_recovery_failures;
    atomic_t uart_rx_recovery_last_error;
    atomic_t uart_rx_events_consumed;
    atomic_t uart_tx_enqueued_bytes;
    atomic_t uart_tx_dropped_bytes;
    atomic_t uart_tx_start_fails;
    atomic_t uart_tx_aborted;
    atomic_t cdc_dtr;
    atomic_t cdc_host_rx_events;
    atomic_t cdc_host_rx_bytes;
    atomic_t cdc_host_rx_overflow_bytes;
    atomic_t uart_to_cdc_events;
    atomic_t uart_to_cdc_requested_bytes;
    atomic_t uart_to_cdc_written_bytes;
    atomic_t uart_to_cdc_dropped_bytes;
    atomic_t uart_to_cdc_no_dtr_bytes;
    atomic_t uart_to_cdc_last_ms;
    atomic_t usb_error_events;
    atomic_t usb_reset_events;
    atomic_t usb_disconnect_events;
};

extern struct bridge_stats g_bridge_stats;

#endif /* IFMCU_BRIDGE_STATS_H_ */
