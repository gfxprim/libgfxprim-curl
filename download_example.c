// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023 Cyril Hrubis <metan@ucw.cz>
 */

#include <widgets/gp_widgets.h>
#include "gp_dialog_download.h"

struct url_path {
	gp_widget *url;
	gp_widget *path;
};

static int run_download_dialog(gp_widget_event *ev)
{
	struct url_path *url_path = ev->self->priv;

	if (ev->type != GP_WIDGET_EVENT_WIDGET)
		return 0;

	gp_dialog_download_run(gp_widget_tbox_text(url_path->url), gp_widget_tbox_text(url_path->path));

	return 0;
}

gp_app_info app_info = {
	.name = "Download Example",
	.desc = "Libcurl integration example",
	.version = "1.0",
	.url = "http://github.com/gfxprim/libgfxprim-curl",
	.license = "GPL-2.0-or-later",
};

int main(int argc, char *argv[])
{
	gp_widget *layout = gp_widget_grid_new(1, 3, 0);
	gp_widget *button = gp_widget_button_new("Download", 0);
	gp_widget *url = gp_widget_tbox_new("http://metan.ucw.cz/outgoing/stojan.jpg", 0, 40, 0, NULL, GP_WIDGET_TBOX_URL);
	gp_widget *path = gp_widget_tbox_new(getenv("HOME"), 0, 40, 0, NULL, GP_WIDGET_TBOX_PATH);

	gp_widget_grid_put(layout, 0, 0, url);
	gp_widget_grid_put(layout, 0, 1, path);
	gp_widget_grid_put(layout, 0, 2, button);

	struct url_path url_path = {
		.url = url,
		.path = path,
	};

	gp_widget_on_event_set(button, run_download_dialog, &url_path);

	gp_widgets_main_loop(layout, NULL, argc, argv);

	return 0;
}
