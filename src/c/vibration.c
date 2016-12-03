#include <pebble.h>
#include "vibration.h"

void vibration_vibes_1()  {
	if (quiet_time_is_active()) return;
	vibes_enqueue_custom_pattern(custom_pattern_1);
}

void vibration_vibes_2()  {
	if (quiet_time_is_active()) return;
	vibes_enqueue_custom_pattern(custom_pattern_2);
}

void vibration_vibes_3()  {
	if (quiet_time_is_active()) return;
	vibes_enqueue_custom_pattern(custom_pattern_3);
}