<?php
/*
 * Copyright 2020 New Relic Corporation. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/*DESCRIPTION
Tests newrelic_record_custom_event() and checking that if the 
max samples is set to an invalid value of -1 that the default
value of 30000 is used.

The actual test suggests spreading the events over a minute
but the integration runner cannot test more than a single harvest.
So this test simply sends the events in a burst and verifies
the number in the harvest matches what is expected.
*/

/*INI
newrelic.custom_events.max_samples_stored = -1
newrelic.custom_insights_events.enabled = 1
*/

/*EXPECT_CUSTOM_EVENTS
[
  "?? agent run id",
  {
    "reservoir_size": "??",
    "events_seen": 2500
  },
  "??"
]
*/

/* Send more events than the limit per harvest */
for ($i=0; $i < 5000; $i++) {
  newrelic_record_custom_event("alpha", array("beta"=>$i));
}