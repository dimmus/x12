/* SPDX-License-Identifier: MIT */
#include "xwm.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void
xwm_config_defaults(XwmConfig *c)
{
    memset(c, 0, sizeof(*c));
    snprintf(c->font_name, sizeof(c->font_name), "%s", "fixed");
    c->col_frame = 0x1a2a3a;
    c->col_frame_focus = 0x2a5a8a;
    c->col_title_fg = 0xe6e6e6;
    c->col_title_fg_unfocus = 0xa0a0a0;
    c->col_button = 0x3a4a5a;
    c->col_menu_bg = 0x1a2a3a;
    c->col_menu_fg = 0xe6e6e6;
    c->col_menu_hi = 0x2a5a8a;
    c->random_placement = true;
    c->title_focus = true;
    snprintf(c->term_cmd, sizeof(c->term_cmd), "%s", "xterm");
}

static unsigned long
parse_color(const char *s)
{
    if (!s)
        return 0;
    if (s[0] == '#')
        s++;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
        s += 2;
    return strtoul(s, NULL, 16);
}

static void
trim_inplace(char *s)
{
    char *start = s;
    char *end;

    while (*start && isspace((unsigned char)*start))
        start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
    end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1]))
        *--end = '\0';
}

int
xwm_config_load(XwmState *s, const char *path)
{
    FILE *fp;
    char line[512];

    xwm_config_defaults(&s->cfg);
    if (!path || !path[0])
        return 0;
    fp = fopen(path, "r");
    if (!fp)
        return -1;

    while (fgets(line, sizeof(line), fp)) {
        char *eq, *key, *val;
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\0')
            continue;
        eq = strchr(line, '=');
        if (!eq)
            continue;
        *eq = '\0';
        key = line;
        val = eq + 1;
        trim_inplace(key);
        trim_inplace(val);
        if (!strcmp(key, "font"))
            snprintf(s->cfg.font_name, sizeof(s->cfg.font_name), "%s", val);
        else if (!strcmp(key, "frame"))
            s->cfg.col_frame = parse_color(val);
        else if (!strcmp(key, "frame_focus"))
            s->cfg.col_frame_focus = parse_color(val);
        else if (!strcmp(key, "title_fg"))
            s->cfg.col_title_fg = parse_color(val);
        else if (!strcmp(key, "term"))
            snprintf(s->cfg.term_cmd, sizeof(s->cfg.term_cmd), "%s", val);
        else if (!strcmp(key, "random_placement"))
            s->cfg.random_placement =
                strcmp(val, "0") != 0 && strcasecmp(val, "false") != 0;
    }
    fclose(fp);
    return 0;
}
