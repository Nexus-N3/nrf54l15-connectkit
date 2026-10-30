/*
 * Copyright (c) 2026 Nexus N3 contributors
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include "bridge_stats.h"
#include <zephyr/init.h>
#include <hal/nrf_timer.h>

struct bridge_stats g_bridge_stats;

#define VALUE(field) ((unsigned int)(uint32_t)atomic_get(&g_bridge_stats.field))

#define BRIDGE_TRACE_CAPACITY 128

struct bridge_trace_entry {
    uint32_t time_us;
    uint16_t code;
    uint16_t value;
};

static struct bridge_trace_entry trace[BRIDGE_TRACE_CAPACITY];
static struct k_spinlock trace_lock;
static uint16_t trace_next;
static uint16_t trace_count;
static bool trace_frozen;

/*
 * Dedicated high-resolution timestamp counter.
 *
 * TIMER3: 16 MHz / 2^4 = 1 MHz
 * Resolution: 1 microsecond
 * Counter: 32-bit, free-running
 *
 * No interrupts, PPI channels or UART configuration changes.
 */
static int bridge_trace_timer_init(void)
{
    nrf_timer_task_trigger(NRF_TIMER3, NRF_TIMER_TASK_STOP);

    nrf_timer_mode_set(NRF_TIMER3, NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(NRF_TIMER3, NRF_TIMER_BIT_WIDTH_32);
    nrf_timer_prescaler_set(NRF_TIMER3, 4);

    nrf_timer_task_trigger(NRF_TIMER3, NRF_TIMER_TASK_CLEAR);
    nrf_timer_task_trigger(NRF_TIMER3, NRF_TIMER_TASK_START);

    return 0;
}

SYS_INIT(bridge_trace_timer_init, POST_KERNEL, 70);

static uint32_t bridge_trace_now_us(void)
{
    nrf_timer_task_trigger(NRF_TIMER3, NRF_TIMER_TASK_CAPTURE0);
    return nrf_timer_cc_get(NRF_TIMER3, NRF_TIMER_CC_CHANNEL0);
}

void bridge_trace_record(enum bridge_trace_code code, uint16_t value)
{
    k_spinlock_key_t key = k_spin_lock(&trace_lock);

    if (!trace_frozen) {
        struct bridge_trace_entry *entry = &trace[trace_next];

        entry->time_us = bridge_trace_now_us();
        entry->code = (uint16_t)code;
        entry->value = value;

        trace_next = (trace_next + 1) % BRIDGE_TRACE_CAPACITY;

        if (trace_count < BRIDGE_TRACE_CAPACITY) {
            trace_count++;
        }

        if (code == BRIDGE_TRACE_FRAMING_ERROR) {
            trace_frozen = true;
        }
    }

    k_spin_unlock(&trace_lock, key);
}

static int cmd_bridge_trace(const struct shell *sh,
                            size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    k_spinlock_key_t key = k_spin_lock(&trace_lock);

    uint16_t count = trace_count;
    uint16_t start =
        (trace_next + BRIDGE_TRACE_CAPACITY - count) %
        BRIDGE_TRACE_CAPACITY;
    bool frozen = trace_frozen;

    k_spin_unlock(&trace_lock, key);

    shell_print(sh, "trace frozen=%u count=%u",
                (unsigned int)frozen, (unsigned int)count);

    if (!frozen) {
        shell_print(sh, "Waiting for first UART framing error");
        return 0;
    }

    uint32_t previous_us = 0;

    for (uint16_t i = 0; i < count; i++) {
        struct bridge_trace_entry entry =
            trace[(start + i) % BRIDGE_TRACE_CAPACITY];

        uint32_t delta = (i == 0) ?
            0 : entry.time_us - previous_us;

        shell_print(sh,
                    "trace idx=%u time_us=%u delta_us=%u event=%u value=%u",
                    (unsigned int)i,
                    entry.time_us,
                    delta,
                    (unsigned int)entry.code,
                    (unsigned int)entry.value);

        previous_us = entry.time_us;
    }

    return 0;
}

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
    shell_print(sh, "uart_baud actual=%u requested=%u previous=%u changes=%u last_change_ms=%u",
                VALUE(uart_baud_actual), VALUE(uart_baud_last_requested),
                VALUE(uart_baud_previous), VALUE(uart_baud_changes),
                VALUE(uart_baud_last_change_ms));
    shell_print(sh, "uart_baud_config attempts=%u last_attempt_ms=%u in_progress=%u failures=%u",
                VALUE(uart_baud_config_attempts), VALUE(uart_baud_last_attempt_ms),
                VALUE(uart_baud_config_in_progress), VALUE(uart_baud_config_failures));
    shell_print(sh, "cdc_line baud=%u baud_changes=%u baud_last_ms=%u dtr_changes=%u dtr_last_ms=%u",
                VALUE(cdc_line_baud), VALUE(cdc_line_baud_changes),
                VALUE(cdc_line_baud_last_change_ms), VALUE(cdc_dtr_changes),
                VALUE(cdc_dtr_last_change_ms));
    shell_print(sh, "uart_fault at_ms=%u last_rx_ms=%u rx_bytes=%u uart_baud=%u cdc_baud=%u dtr=%u",
                VALUE(uart_fault_ms), VALUE(uart_fault_last_rx_ms),
                VALUE(uart_fault_rx_bytes), VALUE(uart_fault_uart_baud),
                VALUE(uart_fault_cdc_baud), VALUE(uart_fault_dtr));
    shell_print(sh, "uart_fault_context uart_baud_change_ms=%u baud_attempt_ms=%u baud_config_in_progress=%u cdc_baud_change_ms=%u dtr_change_ms=%u",
                VALUE(uart_fault_uart_change_ms), VALUE(uart_fault_baud_attempt_ms),
                VALUE(uart_fault_baud_config_in_progress),
                VALUE(uart_fault_cdc_baud_change_ms), VALUE(uart_fault_dtr_change_ms));
    shell_print(sh, "uart_recovery last_success_ms=%u",
                VALUE(uart_recovery_last_success_ms));
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_bridge,
    SHELL_CMD(stats, NULL, "Show USB/UART bridge counters", cmd_bridge_stats),
    SHELL_CMD(trace, NULL, "Show frozen UART event trace", cmd_bridge_trace),
    SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(bridge, &sub_bridge, "USB/UART bridge diagnostics", NULL);
