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

GrTemperatureUnit
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

        if (unit == GR_PREFERRED_UNIT_LOCALE) {
#ifdef LC_MEASUREMENT
                const char *fmt;

                fmt = nl_langinfo (_NL_MEASUREMENT_MEASUREMENT);
                if (fmt && *fmt == 2)
                        unit = GR_PREFERRED_UNIT_IMPERIAL;
                else
#endif
                        unit = GR_PREFERRED_UNIT_METRIC;
        }

        return unit;
}

GrPreferredUnit
get_weight_unit (void)
{
        int unit;
        GSettings *settings = gr_settings_get ();

        unit = g_settings_get_enum (settings, "weight-unit");

        if (unit == GR_PREFERRED_UNIT_LOCALE) {
#ifdef LC_MEASUREMENT
                const char *fmt;

                fmt = nl_langinfo (_NL_MEASUREMENT_MEASUREMENT);
                if (fmt && *fmt == 2)
                        unit = GR_PREFERRED_UNIT_IMPERIAL;
                else
#endif
                        unit = GR_PREFERRED_UNIT_METRIC;
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

        if (user_volume_unit == GR_PREFERRED_UNIT_IMPERIAL) {

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
        if (user_volume_unit == GR_PREFERRED_UNIT_METRIC) {

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

        if (user_weight_unit == GR_PREFERRED_UNIT_IMPERIAL) {
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
        
        if (user_weight_unit == GR_PREFERRED_UNIT_METRIC) {
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

void 
human_readable (double *amount, GrUnit *unit)
{
        double amount1 = *amount;        
        GrUnit unit1 = *unit;
        
        gboolean unit_changed = TRUE;

    while(unit_changed) {
        switch (unit1) {
                case GR_UNIT_GRAM: 
                        if (amount1 >= 1000) {
                                amount1 = (amount1 / 1000);
                                unit1 = GR_UNIT_KILOGRAM;
                        }
                break;

                case GR_UNIT_KILOGRAM:
                        if (amount1 < 1) {
                                amount1 = (amount1 * 1000);
                                unit1 = GR_UNIT_GRAM;
                        }
                break;

                case GR_UNIT_POUND:
                        if (amount1 < 1) {
                                amount1 = (amount1 * 16);
                                unit1 = GR_UNIT_OUNCE;
                        }
                break;

                case GR_UNIT_OUNCE:
                        if (amount1 >= 16) {
                                amount1 = (amount1 / 16);
                                unit1 = GR_UNIT_POUND;  

                        }
                break;

                case GR_UNIT_TEASPOON:
                        if (amount1 >= 3) {
                                amount1 = (amount1 / 3);
                                unit1 = GR_UNIT_TABLESPOON; 
                        }
                break;

                case GR_UNIT_TABLESPOON:
                        if (amount1 >= 16) {
                                amount1 = (amount1 / 16);
                                unit1 = GR_UNIT_CUP;
                        }
                        
                        else if ((amount1 < 1) && (amount1 > 0)) {
                                amount1 = (amount1 * 3);
                                unit1 = GR_UNIT_TEASPOON;
                        }     
                break;
                
                case GR_UNIT_CUP:
                        if (amount1 >= 4) {
                                amount1 = (amount1 / 4);
                                unit1 = GR_UNIT_QUART;
                        }
                        
                        else if (amount1 < 1) {
                                amount1 = amount1 / 16;
                                unit1 = GR_UNIT_TABLESPOON;

                        }
                break;

                case GR_UNIT_MILLILITER:
                        if (amount1 >= 1000) {
                                amount1 = amount1 / 1000;
                                unit1 = GR_UNIT_LITER;
                        }
                break;

                case GR_UNIT_DECILITER:
                        if (amount1 < 1) {
                                amount1 = amount1 * 100;
                                unit1 = GR_UNIT_MILLILITER;
                        }
                        else if (amount1 >= 10) {
                                amount1 = amount1 / 10;
                                unit1 = GR_UNIT_LITER;
                        }
                break;

                case GR_UNIT_LITER: 
                        if (amount1 < 1) {
                                amount1 = amount1 * 1000;
                                unit1 = GR_UNIT_MILLILITER;
                        }
                break;

                default:
                ;
    }

                if (*unit == unit1) {
                unit_changed = FALSE; 
        }
                *amount = amount1;
                *unit = unit1;
    }
}

void
multiple_units (double *amount1, GrUnit *unit1, double *amount2, GrUnit *unit2)
{

        double n1 = *amount1;
        double n2 = *amount2;
        GrUnit u1 = *unit1;
        GrUnit u2 = *unit2;
        double fractional, integer;

        human_readable(&n1, &u1);

        fractional = modf(n1, &integer);

        if (u1 != GR_UNIT_UNKNOWN) {
                if (fractional > 0)
                {
                        n2 = fractional;
                        u2 = u1;
                        human_readable(&n2, &u2);

                        if (u1 != u2)
                        {
                                n1 = integer;
                        }
                        
                        else 
                        {
                                n2 = 0;
                                u2 = GR_UNIT_UNKNOWN;
                        }
                }
                
                else
                {
                                n2 = 0;
                                u2 = GR_UNIT_UNKNOWN;
                }
        }

        else
        {
                                n2 = 0;
                                u2 = GR_UNIT_UNKNOWN;
        }

        *amount1 = n1;
        *unit1 = u1;
        *amount2 = n2;
        *unit2 = u2;
}

char *            
format_for_display  (char *a1, const char *u1, char *a2, const char *u2) {
                g_autoptr(GString) s = NULL;
                s = g_string_new ("");

                if (u1 == NULL) {
                        g_string_append(s, a1);

                }
                else if (u2 == NULL) {
                        g_string_append(s, a1);
                        g_string_append(s, " ");
                        g_string_append(s, u1);
                }

                else {
                        g_string_append(s, a1);
                        g_string_append(s, " ");
                        g_string_append(s, u1);
                        g_string_append(s, ", ");
                        g_string_append(s, a2);
                        g_string_append(s, " ");
                        g_string_append(s, u2);
                }
    
    return g_strdup (s->str);
}
