/*
 * Copyright 2020 New Relic Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file contains internal functions for commands from agent to daemon
 */
#ifndef NR_COMMANDS_PRIVATE_HDR
#define NR_COMMANDS_PRIVATE_HDR

#include "util_flatbuffers.h"

/*
 * The minimum size of a flatbuffer message (no agent run or message body).
 * This should be updated when new fields are added.
 */
#define MIN_FLATBUFFER_SIZE 21

/*
 * Purpose : Check that the first offset is actually within the bounds of the
 *           message length.
 *
 * Returns : 1 if the first offset is invalid and 0 otherwise.
 */
extern int nr_command_is_flatbuffer_invalid(nr_flatbuffer_t* msg,
                                            size_t msglen);

/*
 * The following enums define the field indexes for the various types
 * of data that sent to and received from the daemon. These *MUST*
 * match the values generated by the flatbuffers compiler and consumed
 * by the daemon.
 *
 * See: src/newrelic/protocol.fbs for details.
 */

/* Generated from: union MessageBody */
enum {
  MESSAGE_BODY_NONE = 0,
  MESSAGE_BODY_APP = 1,
  MESSAGE_BODY_APP_REPLY = 2,
  MESSAGE_BODY_TXN = 3,
  MESSAGE_BODY_SPAN_BATCH = 4,
};

/* Generated from: table Message */
enum {
  MESSAGE_FIELD_AGENT_RUN_ID = 0,
  MESSAGE_FIELD_DATA_TYPE = 1,
  MESSAGE_FIELD_DATA = 2,
  MESSAGE_NUM_FIELDS = 3,
};

/* Generated from: enum AppStatus */
enum {
  APP_STATUS_UNKNOWN = 0,
  APP_STATUS_DISCONNECTED = 1,
  APP_STATUS_INVALID_LICENSE = 2,
  APP_STATUS_CONNECTED = 3,
  APP_STATUS_STILL_VALID = 4,
};

/* Generated from: table App */
enum {
  APP_FIELD_LICENSE = 0,
  APP_FIELD_APPNAME = 1,
  APP_FIELD_AGENT_LANGUAGE = 2,
  APP_FIELD_AGENT_VERSION = 3,
  APP_FIELD_HIGH_SECURITY = 4,
  APP_FIELD_REDIRECT_COLLECTOR = 5,
  APP_FIELD_ENVIRONMENT = 6,
  APP_FIELD_SETTINGS = 7,
  APP_FIELD_LABELS = 8,
  APP_DISPLAY_HOST = 9,
  APP_SECURITY_POLICY_TOKEN = 10,
  APP_SUPPORTED_SECURITY_POLICIES = 11,
  APP_HOST = 12,
  APP_TRACE_OBSERVER_HOST = 13,
  APP_TRACE_OBSERVER_PORT = 14,
  APP_SPAN_QUEUE_SIZE = 15,
  APP_SPAN_EVENTS_MAX_SAMPLES_STORED = 16,
  APP_METADATA = 17,
  APP_LOG_EVENTS_MAX_SAMPLES_STORED = 18,
  APP_CUSTOM_EVENTS_MAX_SAMPLES_STORED = 19,
  APP_NUM_FIELDS = 20,
};

/* Generated from: table AppReply */
enum {
  APP_REPLY_FIELD_STATUS = 0,
  APP_REPLY_FIELD_CONNECT_REPLY = 1,
  APP_REPLY_FIELD_SECURITY_POLICIES = 2,
  APP_REPLY_FIELD_CONNECT_TIMESTAMP = 3,
  APP_REPLY_FIELD_HARVEST_FREQUENCY = 4,
  APP_REPLY_FIELD_SAMPLING_TARGET = 5,
  APP_REPLY_NUM_FIELDS = 6,
};

/* Generated from: table Transaction */
enum {
  TRANSACTION_FIELD_NAME = 0,
  TRANSACTION_FIELD_URI = 1,
  TRANSACTION_FIELD_SYNTHETICS_RESOURCE_ID = 2,
  TRANSACTION_FIELD_PID = 3,
  TRANSACTION_FIELD_TXN_EVENT = 4,
  TRANSACTION_FIELD_METRICS = 5,
  TRANSACTION_FIELD_ERRORS = 6,
  TRANSACTION_FIELD_SLOW_SQLS = 7,
  TRANSACTION_FIELD_CUSTOM_EVENTS = 8,
  TRANSACTION_FIELD_TRACE = 9,
  TRANSACTION_FIELD_ERROR_EVENTS = 10,
  TRANSACTION_FIELD_SAMPLING_PRIORITY = 11,
  TRANSACTION_FIELD_SPAN_EVENTS = 12,
  TRANSACTION_FIELD_LOG_EVENTS = 13,
  TRANSACTION_NUM_FIELDS = 14,
};

/* Generated from: table Event */
enum {
  EVENT_FIELD_DATA = 0,
  EVENT_NUM_FIELDS = 1,
};

/* Generated from: table Error */
enum {
  ERROR_FIELD_PRIORITY = 0,
  ERROR_FIELD_DATA = 1,
  ERROR_NUM_FIELDS = 2,
};

/* Generated from: table Metric */
enum {
  METRIC_FIELD_NAME = 0,
  METRIC_FIELD_DATA = 1,
  METRIC_NUM_FIELDS = 2,
};

/* Generated from: struct MetricData */
enum {
  METRIC_DATA_VOFFSET_COUNT = 0,
  METRIC_DATA_VOFFSET_TOTAL = 8,
  METRIC_DATA_VOFFSET_EXCLUSIVE = 16,
  METRIC_DATA_VOFFSET_MIN = 24,
  METRIC_DATA_VOFFSET_MAX = 32,
  METRIC_DATA_VOFFSET_SOS = 40,
  METRIC_DATA_VOFFSET_SCOPED = 48,
  METRIC_DATA_VOFFSET_FORCED = 49,
};

/* Generated from: table SlowSQL */
enum {
  SLOWSQL_FIELD_ID = 0,
  SLOWSQL_FIELD_COUNT = 1,
  SLOWSQL_FIELD_TOTAL_MICROS = 2,
  SLOWSQL_FIELD_MIN_MICROS = 3,
  SLOWSQL_FIELD_MAX_MICROS = 4,
  SLOWSQL_FIELD_METRIC = 5,
  SLOWSQL_FIELD_QUERY = 6,
  SLOWSQL_FIELD_PARAMS = 7,
  SLOWSQL_NUM_FIELDS = 8,
};

/* Generated from: table Trace */
enum {
  TRACE_FIELD_TIMESTAMP = 0,
  TRACE_FIELD_DURATION = 1,
  TRACE_FIELD_GUID = 2,
  TRACE_FIELD_FORCE_PERSIST = 3,
  TRACE_FIELD_DATA = 4,
  TRACE_NUM_FIELDS = 5,
};

/* Generated from: table SpanBatch */
enum {
  SPAN_BATCH_FIELD_COUNT = 0,
  SPAN_BATCH_FIELD_ENCODED = 1,
  SPAN_BATCH_NUM_FIELDS = 2,
};

extern nr_flatbuffer_t* nr_appinfo_create_query(const char* agent_run_id,
                                                const char* system_host_name,
                                                const nr_app_info_t* info);

extern nr_status_t nr_cmd_appinfo_process_reply(const uint8_t* data,
                                                int len,
                                                nrapp_t* app);

extern void nr_cmd_appinfo_process_harvest_timing(nr_flatbuffers_table_t* reply,
                                                  nrapp_t* app);

extern void nr_cmd_appinfo_process_event_harvest_config(
    const nrobj_t* config,
    nr_app_limits_t* app_limits,
    nr_app_info_t info);

extern int nr_cmd_appinfo_process_get_harvest_limit(const nrobj_t* limits,
                                                    const char* key,
                                                    int default_value);

extern char* nr_txndata_error_to_json(const nrtxn_t* txn);

/*
 * Purpose : Encode span events into the given flatbuffer.
 *
 * Params  : 1. The destination flatbuffer.
 *           2. The span events.
 *           3. The transaction limit on how many span events should be
 *              encoded.
 *
 * Returns : The offset of the span event table in the flatbuffer.
 *
 * Notes   : 1. The fb parameter is not checked for a NULL value.
 *           2. span_events is not declared const because C, but is treated as
 *              const.
 */
extern uint32_t nr_txndata_prepend_span_events(nr_flatbuffer_t* fb,
                                               nr_vector_t* span_events,
                                               size_t span_event_limit);

extern nr_flatbuffer_t* nr_txndata_encode(const nrtxn_t* txn);

#endif /* NR_COMMANDS_PRIVATE_HDR */
