/* unit.c
 *
 * Copyright (C) 2016 Matthias Clasen <mclasen@redhat.com#}#>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more &details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "gr-unit.h"
#include "gr-number.h"
#include "gr-convert-units.h"
#include "gr-utils.h"

static GString *string;

static void
test_line (const char *line)
{
        char *input;
        GrUnit unit;
        double amount;
        GrDimension dimension;
        g_autoptr(GError) error = NULL;
        g_auto(GStrv) strv = NULL;
        char *p;

        g_string_append_printf (string, "INPUT '%s'\n", line);

        input = (char *)line;

        strv = g_strsplit (input, "\t", 0);
        g_assert (g_strv_length (strv) == 2);

        p = strv[0];
        gr_number_parse (&amount, &p, &error);
        g_assert_no_error (error);

        p = strv[1];
        unit = gr_unit_parse (&p, &error);
        dimension = gr_unit_get_dimension (unit);

        g_string_append_printf (string, "DIMENSION %d\n", dimension);
        if (dimension == GR_DIMENSION_VOLUME) {
                double a;
                GrUnit u;

                a = amount;
                u = unit;
                gr_convert_volume (&a, &u, GR_PREFERRED_UNIT_METRIC);
                g_string_append_printf (string, "METRIC %g %s\n", a, gr_unit_get_display_name (u));

                a = amount;
                u = unit;
                gr_convert_volume (&a, &u, GR_PREFERRED_UNIT_IMPERIAL);
                g_string_append_printf (string, "IMPERIAL %g %s\n", a, gr_unit_get_display_name (u));
        }
        else if (dimension == GR_DIMENSION_MASS) {
                double a;
                GrUnit u;

                a = amount;
                u = unit;
                gr_convert_weight (&a, &u, GR_PREFERRED_UNIT_METRIC);
                g_string_append_printf (string, "METRIC %g %s\n", a, gr_unit_get_display_name (u));

                a = amount;
                u = unit;
                gr_convert_weight (&a, &u, GR_PREFERRED_UNIT_IMPERIAL);
                g_string_append_printf (string, "IMPERIAL %g %s\n", a, gr_unit_get_display_name (u));
        }

        g_string_append (string, "\n");
}

static void
test_file (const char *filename)
{
        g_autofree char *contents = NULL;
        gsize length;
        g_autoptr(GError) error = NULL;
        g_auto(GStrv) lines = NULL;
        int i;

        if (!g_file_get_contents (filename, &contents, &length, &error)) {
                fprintf (stderr, "%s\n", error->message);
                return;
        }

        lines = g_strsplit (contents, "\n", -1);
        for (i = 0; lines[i]; i++) {
                if (lines[i][0] != 0 && lines[i][0] != '#')
                        test_line (lines[i]);
        }
}

static char *
get_expected_filename (const char *filename)
{
        char *f, *p, *expected;

        f = g_strdup (filename);
        p = strstr (f, ".in");
        if (p)
                *p = 0;
        expected = g_strconcat (f, ".expected", NULL);

        g_free (f);

        return expected;
}

static char *
diff_with_file (const char  *file1,
                GString     *string,
                GError     **error)
{
        const char *command[] = { "diff", "-u", file1, NULL, NULL };
        char *diff, *tmpfile;
        int fd;

        diff = NULL;

        /* write the text buffer to a temporary file */
        fd = g_file_open_tmp (NULL, &tmpfile, error);
        if (fd < 0)
                return NULL;

        if (write (fd, string->str, string->len) != (int) string->len) {
                close (fd); 
                g_set_error (error,
                             G_FILE_ERROR, G_FILE_ERROR_FAILED,
                             "Could not write data to temporary file '%s'", tmpfile);
                goto done;
        }
        close (fd);
        command[3] = tmpfile;

        /* run diff command */
        g_spawn_sync (NULL, (char **)command, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, &diff, NULL, NULL, error);

done:
        g_unlink (tmpfile);
        g_free (tmpfile);

        return diff;
}

static void
test_parse (gconstpointer d)
{
        const char *filename = d;
        char *expected_file;
        GError *error = NULL;
        char *diff;

        expected_file = get_expected_filename (filename);

        string = g_string_sized_new (0);

        test_file (filename);

        diff = diff_with_file (expected_file, string, &error);
        g_assert_no_error (error);

        if (diff && diff[0]) {
                g_test_message ("Resulting output doesn't match reference:\n%s", diff);
                g_test_fail ();
        }
        g_free (diff);

        g_string_free (string, TRUE);

        g_free (expected_file);
}

int main (int argc, char *argv[])
{
        GDir *dir;
        GError *error;
        const char *name;
        char *path;

        g_setenv ("LC_ALL", "en_US.UTF-8", TRUE);
        setlocale (LC_ALL, "");

        g_test_init (&argc, &argv, NULL);

        /* allow to easily generate expected output for new test cases */
        if (argc > 1) {
                string = g_string_sized_new (0);
                test_file (argv[1]);
                g_print ("%s", string->str);
                return 0;
        }

        error = NULL;
        path = g_test_build_filename (G_TEST_DIST, "convert-data", NULL);
        dir = g_dir_open (path, 0, &error);
        g_free (path);
        g_assert_no_error (error);
        while ((name = g_dir_read_name (dir)) != NULL) {
                if (!g_str_has_suffix (name, ".in"))
                        continue;

                path = g_strdup_printf ("/unit/convert/%s", name);
                g_test_add_data_func_full (path, g_test_build_filename (G_TEST_DIST, "convert-data", name, NULL),
                                           test_parse, g_free);
                g_free (path);
        }
        g_dir_close (dir);

  return g_test_run ();
}
