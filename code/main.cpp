/**
 *  @filename   :   main.cpp
 *  @brief      :   4.2inch e-paper display (B) demo
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 15 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <ctime>
#include <unistd.h>
#include <pango/pangocairo.h>
#include "screen.h"
#include "gcal.h"
#include "secrets.h"
#include "../lib/json.hpp"

using json = nlohmann::json;

const char *GOOGLE_TIME_FORMAT = "%Y-%m-%dT%H:%M:%S%z";
const char *GOOGLE_DATE_FORMAT = "%Y-%m-%d";
const char *HEADPHONES_PNG = "/home/pi/upNext/resources/icons/events/icons8-headphones-50.png";
const char *PARTY_PNG = "/home/pi/upNext/resources/icons/events/icons8-confetti-50.png";

const char *TITLE_FONT = "Proxima Nova Regular 40";
const char *SUBTITLE_FONT = "Proxima Nova Regular 24";
const char *SMALL_TEXT_BOLD_FONT = "Proxima Nova Bold 16";
const char *SMALL_TEXT_REGULAR_FONT = "Proxima Nova Regular 16";

// FUNCTIONS
void draw_clock(cairo_t *cr);


void draw_clock(cairo_t *cr) {
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

  PangoLayout *layout = pango_cairo_create_layout (cr);
  PangoFontDescription *font_description = pango_font_description_from_string (SMALL_TEXT_BOLD_FONT);
  pango_layout_set_font_description (layout, font_description);
  // Top right alignment
  int width = 100;
  int margin = 10;
  cairo_move_to (cr, 400 - (width + margin), margin);
  pango_layout_set_width (layout, width * PANGO_SCALE);
  pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);

  static char outstr[8];
  time_t t = time(0);   // get time now
  struct tm * now = localtime( & t );
  //sprintf(outstr, "%d:%02d", now->tm_hour, now->tm_min);
  strftime(outstr, 8 * sizeof(char), "%-I:%M%P", now);

  pango_layout_set_text (layout, outstr, -1);
  pango_cairo_show_layout(cr, layout);

  pango_font_description_free (font_description);
  g_object_unref (layout);
}


void draw_image(cairo_t *cr, string message, const char* filepath) {
  int margin = 10;
  PangoLayout *layout = pango_cairo_create_layout (cr);
  PangoFontDescription *font_description = pango_font_description_from_string (SUBTITLE_FONT);
  pango_layout_set_font_description (layout, font_description);
  // Center, slightly below center
  cairo_move_to (cr, margin, 200);
  pango_layout_set_width (layout, (400 - 2 * margin) * PANGO_SCALE);
  // Only draw one line
  pango_layout_set_height (layout, -1);
  pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);

  pango_layout_set_text (layout, message.c_str(), -1);
  pango_cairo_show_layout(cr, layout);

  // Draw image, restore source
  cairo_surface_t *image = cairo_image_surface_create_from_png(filepath);
  if (cairo_surface_status(image) == CAIRO_STATUS_SUCCESS) {
    cairo_set_source_surface (cr, image, 125, 50);
    cairo_paint (cr);
  }

  cairo_surface_destroy (image);
  pango_font_description_free (font_description);
  g_object_unref (layout);
}




// MAIN PROGRAM //
int main(void)
{
    Screen screen;
    if (screen.Init() != 0) {
        printf("Screen initialization failed\n");
        return -1;
    }

    //screen.Clear();
    screen.HardWipe();

    cairo_surface_t *surface = screen.GetCairoSurface();
    cairo_t *cr = cairo_create (surface);

    // Every 10 seconds, fetch events and re-render
    while(true) {

      draw_clock(cr);

      screen.Render();

      // clear cairo context
      cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
      cairo_paint (cr);
      cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

      sleep(10);
    }

    cairo_destroy (cr);
    screen.Cleanup();
    return 0;
}
