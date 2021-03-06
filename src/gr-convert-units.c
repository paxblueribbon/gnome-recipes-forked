/* gr-convert-units.c:
 *
 * Copyright (C) 2017 Paxana Xander <paxana@paxana.me>
 *
 * Licensed under the GNU General Public License Version 3
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <string.h>
#include "gr-settings.h"
#include <stdlib.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <langinfo.h>
#include <gtk/gtk.h>
#include "gr-unit.h"
#include "gr-convert-units.h"
#include <math.h>
#include "gr-number.c"
#include "gr-utils.h"

int
get_temperature_unit (void)
{
        int unit;
        GSettings *settings = gr_settings_get ();

        unit = g_settings_get_enum (settings, "temperature-unit");

        if (unit == GR_TEMPERATURE_UNIT_LOCALE) {
#ifdef LC_MEASUREMENT
                const char *fmt;

                fmt = nl_langinfo (_NL_MEASUREMENT_MEASUREMENT);
                if (fmt && *fmt == 2)
                        unit = GR_TEMPERATURE_UNIT_FAHRENHEIT;
                else
#endif
                        unit = GR_TEMPERATURE_UNIT_CELSIUS;
        }

        return unit;
}

GrPreferredUnit
get_volume_unit (void)
{
        int unit;
        GSettings *settings = gr_settings_get ();

        unit = g_settings_get_enum (settings, "volume-unit");

        if (unit == GR_VOLUME_UNIT_LOCALE) {
#ifdef LC_MEASUREMENT
                const char *fmt;

                fmt = nl_langinfo (_NL_MEASUREMENT_MEASUREMENT);
                if (fmt && *fmt == 2)
                        unit = GR_VOLUME_UNIT_IMPERIAL;
                else
#endif
                        unit = GR_VOLUME_UNIT_METRIC;
        }

        return unit;
}

GrPreferredUnit
get_weight_unit (void)
{
        int unit;
        GSettings *settings = gr_settings_get ();

        unit = g_settings_get_enum (settings, "weight-unit");

        if (unit == GR_VOLUME_UNIT_LOCALE) {
#ifdef LC_MEASUREMENT
                const char *fmt;

                fmt = nl_langinfo (_NL_MEASUREMENT_MEASUREMENT);
                if (fmt && *fmt == 2)
                        unit = GR_VOLUME_UNIT_IMPERIAL;
                else
#endif
                        unit = GR_VOLUME_UNIT_METRIC;
        }
        return unit;
}


void
convert_temp (int *num, int *unit, int user_unit)
{
    int num1 = *num;
    int unit1 = *unit;
                
        if (unit1 == GR_TEMPERATURE_UNIT_CELSIUS &&
                user_unit == GR_TEMPERATURE_UNIT_FAHRENHEIT) {
                    num1 = (num1 * 1.8) + 32;
                    unit1 = user_unit;
                }

        else if (unit1 == GR_TEMPERATURE_UNIT_FAHRENHEIT &&
                user_unit == GR_TEMPERATURE_UNIT_CELSIUS) {
                    num1 = (num1 - 32) / 1.8;
                    unit1 = user_unit;
                }
                                
                *unit = unit1;
                *num = num1;
}

void 
convert_volume (double *amount, GrUnit *unit, int user_volume_unit)
{
        double amount1 = *amount;
        GrUnit unit1 = *unit;        

        if (user_volume_unit == GR_VOLUME_UNIT_IMPERIAL) {

                switch (unit1) {
                        case GR_UNIT_MILLILITER:
                                amount1 = (amount1 / 4.92892);
                                unit1 = GR_UNIT_TEASPOON;   
                        break;

                        case GR_UNIT_DECILITER:
                                amount1 = (amount1 * 20.2884);
                                unit1 = GR_UNIT_TEASPOON;
                        break;

                        case GR_UNIT_LITER:
                                amount1 = (amount1 * 202.884);
                                unit1 = GR_UNIT_TEASPOON;
                        break;

                        default:
                        ;
                }

        }
        if (user_volume_unit == GR_VOLUME_UNIT_METRIC) {

                switch (unit1) {
                        case GR_UNIT_TEASPOON:
                                amount1 = (amount1 * 4.92892);
                                unit1 = GR_UNIT_MILLILITER;
                        break;

                        case GR_UNIT_TABLESPOON:           
                                amount1 = (amount1 * 14.79);
                                unit1 = GR_UNIT_MILLILITER;
                        break;

                        case GR_UNIT_CUP:
                                amount1 = (amount1 * 236.59);
                                unit1 = GR_UNIT_MILLILITER;
                        break;  

                        case GR_UNIT_PINT:
                                amount1 = (amount1 * 473.176);
                                unit1 = GR_UNIT_MILLILITER;
                        break;

                        case GR_UNIT_QUART:
                                amount1 = (amount1 * 946.353);
                                unit1 = GR_UNIT_MILLILITER;
                        break;

                        case GR_UNIT_GALLON:
                                amount1 = (amount1 * 3785.41);
                                unit1 = GR_UNIT_MILLILITER;
                        break;

                        case GR_UNIT_FLUID_OUNCE:
                                amount1 = (amount1 * 29.5735);
                                unit1 = GR_UNIT_MILLILITER;
                        break;

                        default:
                        ;        
                }
        }                      
        *amount = amount1;
        *unit = unit1;
}

void 
convert_weight (double *amount, GrUnit *unit, int user_weight_unit)
{
        double amount1 = *amount;        
        GrUnit unit1 = *unit;

        if (user_weight_unit == GR_VOLUME_UNIT_IMPERIAL) {
                switch (unit1) {
                        case GR_UNIT_GRAM:
                                amount1 = (amount1 * 0.035274);
                                unit1 = GR_UNIT_OUNCE;
                        break;

                        case GR_UNIT_KILOGRAM:
                                amount1 = (amount1 * 35.274);
                                unit1 = GR_UNIT_OUNCE;
                        break;

                        default:
                        ;        
                }
        }
        
        if (user_weight_unit == GR_VOLUME_UNIT_METRIC) {
                switch (unit1) {
                        case GR_UNIT_OUNCE:
                                amount1 = (amount1 * 28.3495);
                                unit1 = GR_UNIT_GRAM;                                
                        break;

                        case GR_UNIT_POUND:
                                amount1 = (amount1 * 453.592);
                                unit1 = GR_UNIT_GRAM;
                        break;

                        case GR_UNIT_STONE:
                                amount1 = (amount1 * 6350.29);
                                unit1 = GR_UNIT_GRAM;
                        break;

                        default:
                        ;
                }
        }
                                *amount = amount1;
                                *unit = unit1;
}