/*
 * Copyright 2020 New Relic Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nr_axiom.h"

#include <stddef.h>
#include <stdbool.h>

#include "util_memory.h"
#include "nr_attributes.h"
#include "util_strings.h"
#include "nr_log_event.h"
#include "nr_log_event_private.h"

nr_log_event_t* nr_log_event_create() {
  return (nr_log_event_t*)nr_zalloc(sizeof(struct _nr_log_event_t));
}

void nr_log_event_destroy(nr_log_event_t** ptr) {
  nr_log_event_t* event = NULL;

  if ((NULL == ptr) || (NULL == *ptr)) {
    return;
  }

  event = *ptr;
  nr_free(event->trace_id);
  nr_free(event->log_level);
  nr_free(event->message);
  nr_free(event->span_id);
  nr_free(event->entity_guid);
  nr_free(event->entity_name);
  nr_free(event->hostname);

  nr_realfree((void**)ptr);
}

nr_log_event_t* nr_log_event_clone(const nr_log_event_t* src) {
  nr_log_event_t* clone = NULL;

  if (NULL == src) {
    return NULL;
  }

  clone = nr_log_event_create();
  if (NULL == clone) {
    return NULL;
  }

  if (NULL != src->trace_id) {
    clone->trace_id = nr_strdup(src->trace_id);
  }
  if (NULL != src->log_level) {
    clone->log_level = nr_strdup(src->log_level);
  }
  if (NULL != src->message) {
    clone->message = nr_strdup(src->message);
  }
  if (NULL != src->span_id) {
    clone->span_id = nr_strdup(src->span_id);
  }
  if (NULL != src->entity_guid) {
    clone->entity_guid = nr_strdup(src->entity_guid);
  }
  if (NULL != src->entity_name) {
    clone->entity_name = nr_strdup(src->entity_name);
  }
  if (NULL != src->hostname) {
    clone->hostname = nr_strdup(src->hostname);
  }
  clone->priority = src->priority;
  clone->timestamp = src->timestamp;

  return clone;
}

static bool add_log_field_to_buf(nrbuf_t* buf,
                                 const char* field_name,
                                 const char* field_value,
                                 const bool first,
                                 const bool required) {
  const char* final_value = field_value;

  if (NULL == buf || nr_strempty(field_name)) {
    return false;
  }

  if (nr_strempty(field_value)) {
    if (!required) {
      return false;
    } else {
      final_value = "null";
    }
  }

  if (!first) {
    nr_buffer_add(buf, NR_PSTR(","));
  }
  nr_buffer_add(buf, NR_PSTR("\""));
  nr_buffer_add(buf, field_name, nr_strlen(field_name));
  nr_buffer_add(buf, NR_PSTR("\""));
  nr_buffer_add(buf, NR_PSTR(":"));
  nr_buffer_add(buf, NR_PSTR("\""));
  nr_buffer_add(buf, final_value, nr_strlen(final_value));
  nr_buffer_add(buf, NR_PSTR("\""));

  return true;
}

char* nr_log_event_to_json(const nr_log_event_t* event) {
  nrbuf_t* buf = nr_buffer_create(0, 0);
  char* json = NULL;

  if (nr_log_event_to_json_buffer_ex(event, buf, false)) {
    nr_buffer_add(buf, NR_PSTR("\0"));
    json = nr_strdup(nr_buffer_cptr(buf));
  }

  nr_buffer_destroy(&buf);
  return json;
}

bool nr_log_event_to_json_buffer(const nr_log_event_t* event, nrbuf_t* buf) {
  if (NULL == event || NULL == buf) {
    return false;
  }

  return nr_log_event_to_json_buffer_ex(event, buf, false);
}

bool nr_log_event_to_json_buffer_ex(const nr_log_event_t* event,
                                    nrbuf_t* buf,
                                    bool partial) {
  if (NULL == event || NULL == buf) {
    return false;
  }

  // We'll build the JSON manually
  if (!partial) {
    nr_buffer_add(buf, NR_PSTR("["));
  }
  nr_buffer_add(buf, NR_PSTR("{"));

  // only add non-empty fields
  add_log_field_to_buf(buf, "message", event->message, true, true);
  add_log_field_to_buf(buf, "level", event->log_level, false, true);
  add_log_field_to_buf(buf, "trace.id", event->trace_id, false, false);
  add_log_field_to_buf(buf, "span.id", event->span_id, false, false);
  add_log_field_to_buf(buf, "entity.guid", event->entity_guid, false, false);
  add_log_field_to_buf(buf, "entity.name", event->entity_name, false, false);
  add_log_field_to_buf(buf, "hostname", event->hostname, false, false);

  // timestamp always present
  nr_buffer_add(buf, NR_PSTR(",\"timestamp\":"));
  nr_buffer_write_uint64_t_as_text(buf, event->timestamp);

  nr_buffer_add(buf, NR_PSTR("}"));
  if (!partial) {
    nr_buffer_add(buf, NR_PSTR("]"));
  }

  return true;
}

void nr_log_event_set_message(nr_log_event_t* event, const char* message) {
  if (NULL == event || NULL == message) {
    return;
  }
  if (NULL != event->message) {
    nr_free(event->message);
  }

  // spec says to truncate messages over max limit
  event->message = nr_strndup(message, NR_MAX_LOG_MESSAGE_LEN);
}

void nr_log_event_set_log_level(nr_log_event_t* event, const char* log_level) {
  if (NULL == event || NULL == log_level) {
    return;
  }
  if (NULL != event->log_level) {
    nr_free(event->log_level);
  }
  event->log_level = nr_strdup(log_level);
}

void nr_log_event_set_timestamp(nr_log_event_t* event, const nrtime_t time) {
  if (NULL == event) {
    return;
  }
  event->timestamp = time / NR_TIME_DIVISOR_MS;
}

void nr_log_event_set_trace_id(nr_log_event_t* event, const char* trace_id) {
  if (NULL == event || NULL == trace_id) {
    return;
  }
  if (NULL != event->trace_id) {
    nr_free(event->trace_id);
  }
  event->trace_id = nr_strdup(trace_id);
}

void nr_log_event_set_span_id(nr_log_event_t* event, const char* span_id) {
  if (NULL == event || NULL == span_id) {
    return;
  }
  if (NULL != event->span_id) {
    nr_free(event->span_id);
  }
  event->span_id = nr_strdup(span_id);
}

void nr_log_event_set_guid(nr_log_event_t* event, const char* guid) {
  if (NULL == event || NULL == guid) {
    return;
  }
  if (NULL != event->entity_guid) {
    nr_free(event->entity_guid);
  }
  event->entity_guid = nr_strdup(guid);
}

void nr_log_event_set_entity_name(nr_log_event_t* event,
                                  const char* entity_name) {
  if (NULL == event || NULL == entity_name) {
    return;
  }
  if (NULL != event->entity_name) {
    nr_free(event->entity_name);
  }
  event->entity_name = nr_strdup(entity_name);
}

void nr_log_event_set_hostname(nr_log_event_t* event, const char* hostname) {
  if (NULL == event || NULL == hostname) {
    return;
  }
  if (NULL != event->hostname) {
    nr_free(event->hostname);
  }
  event->hostname = nr_strdup(hostname);
}

void nr_log_event_set_priority(nr_log_event_t* event, int priority) {
  if (NULL == event) {
    return;
  }

  event->priority = priority;
}