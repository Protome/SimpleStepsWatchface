#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_steps_layer;
static Layer *s_bg_layer;
static GFont s_time_font;
static GFont s_steps_font;
static char s_value_buffer[24];

static GColor s_border_colour;
static bool s_show_steps;

void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char buffer[] = "00:00";

	if(clock_is_24h_style() == true) {
		strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	}
	else {
		strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
	}

	text_layer_set_text(s_time_layer, buffer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

int health_get_metric_sum() {
	HealthMetric metric = HealthMetricStepCount;
	HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, 
																																				 time_start_of_today(), time(NULL));
	if(mask == HealthServiceAccessibilityMaskAvailable) {
		return (int)health_service_sum_today(metric);
	} 
	else {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Data unavailable!");
		return 0;
	}
}

void update_steps() {	
	if (health_get_metric_sum() >= 0) {
		if(s_show_steps) {
			snprintf(s_value_buffer, sizeof(s_value_buffer), "Steps: %d", 
                                              health_get_metric_sum());
		}
		else {
			snprintf(s_value_buffer, sizeof(s_value_buffer), "%d", 
                                              health_get_metric_sum());
		}
		text_layer_set_text(s_steps_layer, s_value_buffer);
	}
	else {
		text_layer_set_text(s_steps_layer, "");
	}
}

void health_handler(HealthEventType event, void *context) {
	// Which type of event occured?
	switch(event) {
		case HealthEventSignificantUpdate:
		update_steps();
		break;
		case HealthEventMovementUpdate:
		update_steps();
		break;
		case HealthEventSleepUpdate:
		APP_LOG(APP_LOG_LEVEL_INFO, 
						"New HealthService HealthEventSleepUpdate event");
		break;
	}
}

void update_bg_proc(Layer *bgLayer, GContext *ctx) {
	GRect bounds = layer_get_bounds(bgLayer);
	graphics_context_set_fill_color(ctx, s_border_colour);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Read color preferences
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColour);
  if(bg_color_t) {
    GColor bg_color = GColorFromHEX(bg_color_t->value->int32);
		window_set_background_color(s_main_window, bg_color);
  }

  Tuple *stripe_color_t = dict_find(iter, MESSAGE_KEY_StripeColour);
  if(stripe_color_t) {
    GColor stripe_color = GColorFromHEX(stripe_color_t->value->int32);
		text_layer_set_background_color(s_time_layer, stripe_color);
  }
	
	Tuple *border_color_t = dict_find(iter, MESSAGE_KEY_BorderColour);
  if(border_color_t) {
    GColor border_color = GColorFromHEX(border_color_t->value->int32);
		s_border_colour = border_color;
		layer_mark_dirty(s_bg_layer);
  }
	
	Tuple *time_color_t = dict_find(iter, MESSAGE_KEY_TimeColour);
  if(time_color_t) {
    GColor time_color = GColorFromHEX(time_color_t->value->int32);
		text_layer_set_text_color(s_time_layer, time_color);
  }
	
	Tuple *steps_color_t = dict_find(iter, MESSAGE_KEY_StepsColour);
  if(steps_color_t) {
    GColor steps_color = GColorFromHEX(steps_color_t->value->int32);
		text_layer_set_text_color(s_steps_layer, steps_color);
  }

  // Read boolean preferences
  Tuple *show_steps_t = dict_find(iter, MESSAGE_KEY_StepsText);
  if(show_steps_t) {
    s_show_steps = show_steps_t->value->int32 == 1;
		update_steps();
  }

}

void main_window_load(Window *window) {  
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	window_set_background_color(window, GColorOrange);

	int timeYCoord = 50;
	int borderWidth = 8;
	int timeHeight = 50;
	int backgroundHeight = timeHeight + (borderWidth*2);
	
	s_bg_layer = layer_create(GRect(0,timeYCoord, bounds.size.w, backgroundHeight));
	layer_set_update_proc(s_bg_layer, update_bg_proc);
	s_border_colour = GColorBlack;
	layer_mark_dirty(s_bg_layer);
	
	layer_add_child(window_get_root_layer(window), s_bg_layer);
	
	s_time_layer = text_layer_create(GRect(0, timeYCoord + borderWidth, bounds.size.w, timeHeight));
	text_layer_set_background_color(s_time_layer, GColorWhite);
	text_layer_set_text_color(s_time_layer, GColorBlack);

	s_time_font = fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS);
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

	s_steps_layer = text_layer_create(GRect(0, timeYCoord + backgroundHeight, bounds.size.w, 30));
	text_layer_set_background_color(s_steps_layer, GColorClear);
	text_layer_set_text_color(s_steps_layer, GColorBlack);
	s_steps_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
	
	text_layer_set_font(s_steps_layer, s_steps_font);
	text_layer_set_text_alignment(s_steps_layer, GTextAlignmentCenter);
	s_show_steps = true;

	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_steps_layer));
}

void main_window_unload(Window *window) { 
	text_layer_destroy(s_time_layer);
}

void init() {
	s_main_window = window_create();

	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	window_stack_push(s_main_window, true);
	update_time();

	#if defined(PBL_HEALTH)
	// Attempt to subscribe 
	if(!health_service_events_subscribe(health_handler, NULL)) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
	}
	#else
	APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
	#endif
	
	// Open AppMessage connection
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
}

void deinit() {
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}