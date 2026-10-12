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
    /* Successful physical UART baud reconfiguration and attempted changes. */
    atomic_t uart_baud_actual;
    atomic_t uart_baud_previous;
    atomic_t uart_baud_last_requested;
    atomic_t uart_baud_changes;
    atomic_t uart_baud_last_change_ms;
    atomic_t uart_baud_config_attempts;
    atomic_t uart_baud_last_attempt_ms;
    atomic_t uart_baud_config_in_progress;
    atomic_t uart_baud_config_failures;
    /* Host CDC line coding and DTR transitions (not physical UART settings). */
    atomic_t cdc_line_baud;
    atomic_t cdc_line_baud_changes;
    atomic_t cdc_line_baud_last_change_ms;
    atomic_t cdc_dtr_changes;
    atomic_t cdc_dtr_last_change_ms;
    /* Snapshot taken inside UART_RX_STOPPED, prior to recovery. */
    atomic_t uart_fault_ms;
    atomic_t uart_fault_last_rx_ms;
    atomic_t uart_fault_rx_bytes;
    atomic_t uart_fault_uart_baud;
    atomic_t uart_fault_uart_change_ms;
    atomic_t uart_fault_baud_attempt_ms;
    atomic_t uart_fault_baud_config_in_progress;
    atomic_t uart_fault_cdc_baud;
    atomic_t uart_fault_cdc_baud_change_ms;
    atomic_t uart_fault_dtr;
    atomic_t uart_fault_dtr_change_ms;
    atomic_t uart_recovery_last_success_ms;
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
