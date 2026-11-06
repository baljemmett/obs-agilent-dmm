#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libsigrok/libsigrok.h>

// uni-t-ut61e:conn=1a86.e008


struct display {
	time_t last_update;
	float value;
	int decimal;
	bool DC;
	bool AC;
	bool hold;
	bool diode;
	bool continuity;
	bool delta;
	bool micro;
	bool milli;
	bool volts;
	bool amps;
	bool c;
	bool f;
	bool percent;
	bool mega;
	bool kilo;
	bool ohms;
	bool hz;
	bool millif;
	bool microf;
	bool nanof;
	bool farad;
	bool autorange;
	bool manurange;
};

struct display display_data;
static const struct sr_channel *main_display = NULL;
static const struct sr_channel *secondary_display = NULL;
static const struct sr_channel *temperature_display = NULL;

void datafeed_in(const struct sr_dev_inst *sdi, const struct sr_datafeed_packet *packet, void *cb_data) {

	struct sr_channel *display = NULL;
	int scale = 0;

	if (SR_DF_ANALOG == packet->type) {

		display_data.last_update = time(NULL);
		const struct sr_datafeed_analog *data = packet->payload;

		display = g_slist_nth_data(data->meaning->channels, 0);

		if (display == main_display) {
			printf("Reading for main display.\n");
		} else if (display == secondary_display) {
			printf("Reading for secondary display.\n");
		} else if (display == temperature_display) {
			printf("Reading for temperature display.\n");
		} else {
			printf("Reading for unknown display %p.\n", display);
		}

		if (display != main_display) {
			return;
		}

		float value;

		sr_analog_to_float(data, &value);

  		display_data.AC = (data->meaning->mqflags & SR_MQFLAG_AC);
    	display_data.DC = (data->meaning->mqflags & SR_MQFLAG_DC);
    	display_data.diode = (data->meaning->mqflags & SR_MQFLAG_DIODE);
    	display_data.hold = (data->meaning->mqflags & SR_MQFLAG_HOLD);
    	display_data.autorange = (data->meaning->mqflags & SR_MQFLAG_AUTORANGE);
    	display_data.manurange = !(data->meaning->mqflags & SR_MQFLAG_AUTORANGE);
    	display_data.delta = (data->meaning->mqflags & SR_MQFLAG_RELATIVE);

		switch (data->meaning->mq) {
 			case SR_MQ_VOLTAGE:
				display_data.volts = 1;

				if (value < 1.0) {
					value *= 1000.0;
					scale ++;
				}

				if (value < 1.0) {
					value *= 1000.0;
					scale ++;
				}

				display_data.milli = (scale == 1);
				display_data.micro = (scale == 2);
				display_data.value = value;
				break;

    		case SR_MQ_CURRENT:
				break;

    		case SR_MQ_RESISTANCE:
				break;

			case SR_MQ_CAPACITANCE:
				break;

			case SR_MQ_TEMPERATURE:
				break;

			case SR_MQ_FREQUENCY:
				break;

			case SR_MQ_DUTY_CYCLE:
				break;

			case SR_MQ_CONTINUITY:
				break;

		}


		printf("[1;1H");
		if (display_data.DC) printf("DC"); else printf("  ");
		if (display_data.AC) printf("AC"); else printf("  ");

		if (display_data.hold) printf(" H "); else printf("   ");
		if (display_data.diode) printf("-|>-"); else printf("    ");
		if (display_data.continuity) printf("·))"); else printf("   ");

		if (display_data.delta) printf("Δ"); else printf(" ");
		if (display_data.micro) printf("µ"); else printf(" ");
		if (display_data.milli) printf("m"); else printf(" ");
		if (display_data.volts) printf("V"); else printf(" ");
		if (display_data.amps) printf("A"); else printf(" ");
		printf("\n");

		printf("%.4f\n", display_data.value);
		if (display_data.autorange) printf("Auto"); else printf("    ");
		if (display_data.manurange) printf("Manu"); else printf("    ");
		printf("\n");

	}

//struct sr_datafeed_analog {
//    void *data;
//    uint32_t num_samples;
//    struct sr_analog_encoding *encoding;
//    struct sr_analog_meaning *meaning;
//    struct sr_analog_spec *spec;

}

static void determine_displays(const struct sr_dev_inst *sdi)
{
        struct input_stream_props *props;
        GSList *l, *channels;
        const struct sr_channel *ch;
	int i;

        channels = g_slist_copy(sr_dev_inst_channels_get(sdi));
        if (!channels)
                return;

        for (i = 0, l = channels; l; l = l->next) {
                ch = l->data;
                if (!ch->enabled)
                        continue;
                if (ch->type != SR_CHANNEL_ANALOG)
                        continue;

		switch (++i)
		{
		case 1:
			main_display = ch;
			break;
		case 2:
			secondary_display = ch;
			break;
		case 3:
			temperature_display = ch;
			break;
		default:
			printf("Unexpected display number %u at %p.\n", i, ch);
			break;
		}
        }

	printf("Main display at %p\n", main_display);
	printf("Secondary display at %p\n", secondary_display);
	printf("Temperature display at %p\n", temperature_display);
}

int main(int argc, char **argv) {
	struct sr_context *sr_ctx = NULL;
	struct sr_dev_driver *driver;
	struct sr_dev_driver **drivers;
	struct sr_config *conf;
	struct sr_session *session;
	struct sr_dev_inst *current_device = NULL;

	GSList *devices;
	GSList *drvopts = NULL;

	GVariantBuilder *vbl;

	GMainLoop *main_loop = NULL;

	int i;

	printf("[2J\n");


	if (sr_init(&sr_ctx) != SR_OK) {
		printf("Error creating context\n");
		return -1;
	}

	//printf("Context created OK.\n");

	drivers = sr_driver_list(sr_ctx);
	driver = NULL;

	for (i = 0; drivers[i]; i++) {
		//printf("Found driver %d: '%s'\n", i, drivers[i]->name);

		//if (strcmp(drivers[i]->name, "uni-t-ut61e") == 0) {
		if (strcmp(drivers[i]->name, "agilent-dmm") == 0) {
			driver = drivers[i];
			break;
		}
	}

	if (NULL == driver) {
		printf("Error finding the driver. Is your sigrok up to date?\n");
		sr_exit(sr_ctx);
		return -1;
	}

//	printf("Found relevant driver %p '%s'.\n", driver, driver->name);

	if (SR_OK != sr_driver_init(sr_ctx, driver)) {
		printf("Error initializing the driver\n");
		sr_exit(sr_ctx);
		return -1;
	}

//	printf("Driver initialised successfully.\n");

	conf = malloc(sizeof(struct sr_config) * 2);

//	vbl = g_variant_builder_new(G_VARIANT_TYPE_DICTIONARY);
//	g_variant_builder_add(vbl, "{ss}", "conn", "1a86.e008");

	conf[0].key = SR_CONF_CONN;
//	conf[0].data = g_variant_builder_end(vbl);
	conf[0].data = g_variant_new_string("/dev/tty.PL2303-USBtoUART13430");

	conf[1].key = SR_CONF_SERIALCOMM;
	conf[1].data = g_variant_new_string("9600/8n1");

	drvopts = g_slist_append(drvopts, &conf[0]);
	drvopts = g_slist_append(drvopts, &conf[1]);

	while (1) {

		if (current_device == NULL) {
			sleep(1);

			devices = sr_driver_scan(driver, drvopts);

			if (devices == NULL) {
				continue;
			}


			current_device = devices->data;
			printf("Found %s\n", sr_dev_inst_model_get(current_device));
			determine_displays(current_device);
			g_slist_free(devices);

			sr_session_new(sr_ctx, &session);
			sr_session_datafeed_callback_add(session, datafeed_in, NULL);

			if (SR_OK != sr_dev_open(current_device)) {
				printf("Unable to open device!\n");
				sr_session_destroy(session);
				current_device = NULL;
				continue;
			}

			if (SR_OK != sr_session_dev_add(session, current_device)) {
				printf("Error adding device to the session\n");
				sr_session_destroy(session);
				current_device = NULL;
				continue;
			}

//			main_loop = g_main_loop_new(NULL, FALSE);
//			sr_session_stopped_callback_set(session, (sr_session_stopped_callback)g_main_loop_quit, main_loop);

			sr_session_start(session);
		}

		printf("Started main loop\n");

//		g_main_loop_run(main_loop);
		sr_session_run(session);
		printf("Ended main loop\n");

		current_device = NULL;

		sleep(1);

	}
	



	return 0;
}
