#pragma once
#include <pebble.h>

static const VibePattern custom_pattern_1 = {
  .durations = (uint32_t []) {100},
  .num_segments = 1
};  // 1 short vibes

static const VibePattern custom_pattern_2 = {
  .durations = (uint32_t []) {100, 100, 100},
  .num_segments = 3
};  // 2 short vibes

static const VibePattern custom_pattern_3 = {
  .durations = (uint32_t []) {100, 100, 100, 100, 100},
  .num_segments = 5
};  // 3 short vibes

void vibration_vibes_1();
void vibration_vibes_2();
void vibration_vibes_3();
