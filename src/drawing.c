#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <gd.h>

#include "dmm.h"

#define FONT_PATH "/Users/baljemmett/gits/obs-uni-t-ut61e/fonts/"

const char *text_font = FONT_PATH "LiberationSans-Bold.ttf";
const char *seg_font = FONT_PATH "DSEG7Classic-BoldItalic.ttf";
const char *dmm_font = FONT_PATH "DMM.ttf";

#define COORD(X, Y) ((X) + offset) * WIDTH, ((Y) + offset + local_v_offset) * WIDTH
#define SIZE(X) (X) * WIDTH


void main_image(gdImagePtr img, struct dmm *dmm, float offset, int color) {
	float local_v_offset = 0.15;

	if ((dmm == NULL) || (dmm->current_range & VOLTS) || (dmm->current_range & AMPS)) {
		if ((dmm == NULL) || (dmm->current_range & 0xF000) == DC)
			gdImageStringTTF(img, NULL, color, text_font, SIZE(0.035), 0, COORD(0.08, 0.08), "DC");
		if ((dmm == NULL) || (dmm->current_range & 0xF000) == AC)
			gdImageStringTTF(img, NULL, color, text_font, SIZE(0.035), 0, COORD(0.15, 0.08), "AC");
	}

	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, dmm_font, SIZE(0.070), 0, COORD(0.30, 0.082), "H");

	if ((dmm == NULL) || ((dmm->current_range & 0xFFF) == DIODE))
		gdImageStringTTF(img, NULL, color, dmm_font, SIZE(0.055), 0, COORD(0.55, 0.082), "D");
	if ((dmm == NULL) || (dmm->current_range & (CONT))) 
		gdImageStringTTF(img, NULL, color, dmm_font, SIZE(0.070), 0, COORD(0.63, 0.082), "C");
	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.060), 0, COORD(0.69, 0.082), "Δ");
	if ((dmm == NULL) || dmm->micro) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.035), 0, COORD(0.8, 0.08), "µ");
	if ((dmm == NULL) || dmm->milli) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.035), 0, COORD(0.83, 0.08), "m");

	if ((dmm == NULL) || ((dmm->current_range & (DIODE | VOLTS))))
		gdImageStringTTF(img, NULL, color, text_font, SIZE(0.035), 0, COORD(0.87, 0.08), "V");
	if ((dmm == NULL) || ((dmm->current_range & 0xFFF) == AMPS)) 
		gdImageStringTTF(img, NULL, color, text_font, SIZE(0.035), 0, COORD(0.90, 0.08), "A");

	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.835, 0.12), "°");
	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.87, 0.12), "°");

	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.84, 0.15), "C");
	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.88, 0.15), "F");

	if ((dmm == NULL) || (dmm->current_range & 0xFFF) == DUTY) 
		gdImageStringTTF(img, NULL, color, text_font, SIZE(0.050), 0, COORD(0.92, 0.14), "%");

	if ((dmm == NULL) || dmm->mega) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.84, 0.20), "M");
	if ((dmm == NULL) || dmm->kilo) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.88, 0.20), "k");

	if ((dmm == NULL) || (dmm->current_range & (CONT | OHMS))) 
		gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.92, 0.20), "Ω");

	if ((dmm == NULL) || (dmm->current_range & 0xFFF) == HZ) 
		gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.90, 0.25), "Hz");

	if ((dmm == NULL) || dmm->millif) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.82, 0.30), "m");
	if ((dmm == NULL) || dmm->microf) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.87, 0.30), "µ");
	if ((dmm == NULL) || dmm->nanof) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.905, 0.30), "n");
	if ((dmm == NULL) || (dmm->current_range & 0xFFF) == FARADS) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.935, 0.30), "F");


	//if ((dmm == NULL) || dmm->autorange) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.28, 0.37), "AUTO");
	//if ((dmm == NULL) || dmm->manurange) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.040), 0, COORD(0.45, 0.37), "MANU");


	if ((dmm == NULL) || dmm->is_negative) gdImageFilledRectangle(img, COORD(0.01, 0.195), COORD(0.07, 0.215), color);

	if (dmm == NULL) {
		gdImageStringTTF(img, NULL, color, seg_font, SIZE(0.14), 0, COORD(0.08, 0.30), "8.8.8.8.8");
	} else {
		gdImageStringTTF(img, NULL, color, seg_font, SIZE(0.14), 0, COORD(0.08, 0.30), dmm->text);
	}

	local_v_offset = 0.0;

	if (dmm == NULL) {
		gdImageStringTTF(img, NULL, color, seg_font, SIZE(0.07), 0, COORD(0.55, 0.125), "8.8.8.8.8");
	} else {
		gdImageStringTTF(img, NULL, color, seg_font, SIZE(0.07), 0, COORD(0.55, 0.125), dmm->text_temperature);
	}

	if ((1) || (dmm == NULL) || dmm->temperature_is_negative) gdImageFilledRectangle(img, COORD(0.50, 0.0725), COORD(0.54, 0.0825), color);

	if ((dmm == NULL) || 1) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.020), 0, COORD(0.935, 0.06), "°");
	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.020), 0, COORD(0.97, 0.06), "°");

	if ((dmm == NULL) || 1) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.020), 0, COORD(0.94, 0.08), "C");
	if ((dmm == NULL) || 0) gdImageStringTTF(img, NULL, color, text_font, SIZE(0.020), 0, COORD(0.98, 0.08), "F");

	if (dmm != NULL) {
		float maxval = 0;
		float minval = 9999999999999;
		for (int i = 0; i < MAX_HIST; i++) {
			if (dmm->previous_value[i] > maxval) maxval = dmm->previous_value[i];
			if (dmm->previous_value[i] < minval) minval = dmm->previous_value[i];
		}

		printf("Range: %f - %f\n", minval, maxval);

		float diff = maxval - minval;

		if (diff > 0.0000000001) {

			printf("Diff: %.9f\n", diff);

			for (int i = 0; i < MAX_HIST; i++) {
				float v = (dmm->previous_value[i] - minval) / diff;
				gdImageFilledRectangle(img, COORD((i * 0.04) + 0.05, 0.6), COORD((i * 0.04) + 0.075, 0.6 - (v / 10.0)), color);
			}

		}
	}
}

#ifndef TESTBED
void *video_thread(void *data) {
	struct dmm *dmm = data;
	uint32_t pixels[WIDTH * HEIGHT];
	uint64_t cur_time = os_gettime_ns();


	gdImagePtr background;

	background = gdImageCreateTrueColor(WIDTH, HEIGHT);
	gdImageSaveAlpha(background, true);
	gdImageAlphaBlending(background, true);

	int bgcol = gdImageColorAllocateAlpha(background, 128, 228, 188, 0);
	int speckle = gdImageColorAllocateAlpha(background, 115, 115, 115, 110);

	gdImageFilledRectangle(background, 0, 0, WIDTH, HEIGHT, bgcol);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if ((rand() % 100) > 80) {
				gdImageSetPixel(background, x, y, speckle);
			}
		}
	}

	struct obs_source_frame frame = {
		.data = {[0] = (uint8_t *)pixels},
		.linesize = {[0] = WIDTH * 4},
		.width = WIDTH,
		.height = HEIGHT,
		.format = VIDEO_FORMAT_BGRA,
	};


	while (os_event_try(dmm->stop_signal) == EAGAIN) {
		if ((time(NULL) - dmm->last_update) < 4) {
			if (dmm->fade < 255) {
				dmm->fade += 16;
			}
		} else {
			if (dmm->fade > 0) {
				dmm->fade -= 16;
			}
		}

		if (dmm->fade > 255) dmm->fade = 255;
		if (dmm->fade < 0) dmm->fade = 0;

		gdImagePtr img = gdImageCreateTrueColor(WIDTH, HEIGHT);
		gdImageSaveAlpha(img, true);
		gdImageAlphaBlending(img, true);

		gdImageCopy(img, background, 0, 0, 0, 0, WIDTH, HEIGHT);

		gdImagePtr lcd = gdImageCreateTrueColor(WIDTH, HEIGHT);
		gdImageSaveAlpha(lcd, true);
		gdImageAlphaBlending(lcd, false);

		int lcd_bg = gdImageColorAllocateAlpha(lcd, 200, 0, 0, 127);
		int lcd_shadow = gdImageColorAllocateAlpha(lcd, 0, 0, 0, 100);
		int lcd_hint = gdImageColorAllocateAlpha(lcd, 0, 0, 0, 120);
		int lcd_text = gdImageColorAllocateAlpha(lcd, 0, 0, 0, (rand() % 10) + 20);

		gdImageFilledRectangle(lcd, 0, 0, WIDTH, HEIGHT, lcd_bg);
		gdImageAlphaBlending(lcd, true);


		main_image(lcd, dmm, 0.0075, lcd_shadow);
		main_image(lcd, NULL, 0, lcd_hint);
		main_image(lcd, dmm, 0, lcd_text);


		gdImageCopy(img, lcd, 0, 0, 0, 0, WIDTH, HEIGHT);

		gdImageDestroy(lcd);

		int op = 0;
		for (int y = 0; y < HEIGHT; y++) {
			for (int x = 0; x < WIDTH; x++) {
				pixels[op++] = (dmm->fade << 24) | gdImageGetPixel(img, x, y);
			}
		}
		gdImageDestroy(img);
		frame.timestamp = cur_time;
		obs_source_output_video(dmm->source, &frame);
		os_sleepto_ns(cur_time += 30000000);
	}

	dmm_terminate(dmm);

	return NULL;
}
#endif

#ifdef TESTBED
int main(int argc, char *argv[]) {
	int annunciator_test = 0;
	if (argc == 2 && (strcmp(argv[1], "-a") == 0 || strcmp(argv[1], "--all") == 0)) {
		annunciator_test = 1;
	}

	struct dmm *dmm = calloc(1, sizeof(struct dmm));

	gdImagePtr background;

	background = gdImageCreateTrueColor(WIDTH, HEIGHT);
	gdImageSaveAlpha(background, true);
	gdImageAlphaBlending(background, true);

	int bgcol = gdImageColorAllocateAlpha(background, 128, 228, 188, 0);
	int speckle = gdImageColorAllocateAlpha(background, 115, 115, 115, 110);

	gdImageFilledRectangle(background, 0, 0, WIDTH, HEIGHT, bgcol);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if ((rand() % 100) > 80) {
				gdImageSetPixel(background, x, y, speckle);
			}
		}
	}

	dmm->fade = 255;

		gdImagePtr img = gdImageCreateTrueColor(WIDTH, HEIGHT);
		gdImageSaveAlpha(img, true);
		gdImageAlphaBlending(img, true);

		gdImageCopy(img, background, 0, 0, 0, 0, WIDTH, HEIGHT);

		gdImagePtr lcd = gdImageCreateTrueColor(WIDTH, HEIGHT);
		gdImageSaveAlpha(lcd, true);
		gdImageAlphaBlending(lcd, false);

		int lcd_bg = gdImageColorAllocateAlpha(lcd, 200, 0, 0, 127);
		int lcd_shadow = gdImageColorAllocateAlpha(lcd, 0, 0, 0, 100);
		int lcd_hint = gdImageColorAllocateAlpha(lcd, 0, 0, 0, 120);
		int lcd_text = gdImageColorAllocateAlpha(lcd, 0, 0, 0, (rand() % 10) + 20);

		gdImageFilledRectangle(lcd, 0, 0, WIDTH, HEIGHT, lcd_bg);
		gdImageAlphaBlending(lcd, true);


		main_image(lcd, annunciator_test ? NULL : dmm, 0.0075, lcd_shadow);
		main_image(lcd, NULL, 0, lcd_hint);
		main_image(lcd, annunciator_test ? NULL : dmm, 0, lcd_text);


		gdImageCopy(img, lcd, 0, 0, 0, 0, WIDTH, HEIGHT);

		gdImageDestroy(lcd);

		FILE *fpOut = fopen("output.png", "wb");
		if (fpOut != NULL) {
			gdImagePngEx(img, fpOut, 0);
		} else {
			perror("barf: ");
		}

		gdImageDestroy(img);

	return 0;
}
#endif
