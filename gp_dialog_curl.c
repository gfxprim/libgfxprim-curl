// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * Copyright (C) 2023 Cyril Hrubis <metan@ucw.cz>
 */

#include <errno.h>
#include <string.h>

#include <curl/curl.h>
#include <widgets/gp_widgets.h>

#include "gp_dialog_curl.h"
#include "dialog_download.json.h"

struct download_dialog {
	CURL *easy;
	CURLM *multi;
	gp_dialog dialog;
	gp_widget *progress;
};

static int xferinfo(void *p,
                    curl_off_t dltotal, curl_off_t dlnow,
                    curl_off_t ultotal, curl_off_t ulnow)
{
	if (!p)
		return 0;

	if (!dltotal)
		return 0;

	(void) ultotal;
	(void) ulnow;

	gp_widget_pbar_set_max(p, dltotal);
	gp_widget_pbar_set(p, dlnow);

	return 0;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

static int do_cancel(gp_widget_event *ev)
{
	gp_dialog *dialog = ev->self->priv;

	if (ev->type != GP_WIDGET_EVENT_WIDGET)
		return 0;

	dialog->retval = 1;

	return 0;
}

static const gp_widget_json_addr addrs[] = {
	{.id = "cancel", .on_event = do_cancel},
	{}
};

static const char *last_elem(const char *str)
{
	size_t len = strlen(str);

	while (len && str[len] != '/')
		len--;

	return str+len+1;
}

static gp_widget *load_download_layout(gp_dialog *dialog, const char *url, const char *dest_file, gp_widget **progress)
{
	gp_widget *ret, *w;
	gp_htable *uids = NULL;

	gp_widget_json_callbacks callbacks = {
		.default_priv = dialog,
		.addrs = addrs,
	};

	ret = gp_dialog_layout_load("dialog_download", &callbacks, dialog_download, &uids);
	if (!ret)
		return NULL;

	*progress = gp_widget_by_uid(uids, "progress", GP_WIDGET_PROGRESSBAR);

	w = gp_widget_by_uid(uids, "filename", GP_WIDGET_LABEL);
	if (w)
		gp_widget_label_printf(w, "Saving %s to %s", last_elem(url), last_elem(dest_file));


	gp_htable_free(uids);

	return ret;
}

static void parse_curl_msg(struct download_dialog *download)
{
	int running;
	CURLMsg *message;

	while ((message = curl_multi_info_read(download->multi, &running))) {
		if (message->msg != CURLMSG_DONE)
			continue;

		if (message->data.result) {
			gp_dialog_msg_printf_run(GP_DIALOG_MSG_ERR,
			                         "Download failed",
			                         "%s", curl_easy_strerror(message->data.result));
		}

		download->dialog.retval = 1;
	}
}

static int socket_data(struct gp_fd *self, struct pollfd *pfd)
{
	struct download_dialog *download = self->priv;
	CURLMcode rc;
	int running;
	int flags = 0;

	if (pfd->revents & POLLIN)
		flags |= CURL_CSELECT_IN;

	if (pfd->revents & POLLOUT)
		flags |= CURL_CSELECT_OUT;

	rc = curl_multi_socket_action(download->multi, pfd->fd, flags, &running);

	parse_curl_msg(download);

	return 0;
}

static int socket_cb(CURL *easy, curl_socket_t s, int action, void *userp, void *socketp)
{
	int flags = 0;

	gp_fds_rem(gp_widgets_fds, s);

	switch(action) {
	case CURL_POLL_IN:
		flags = POLLIN;
	break;
	case CURL_POLL_OUT:
		flags = POLLOUT;
	break;
	case CURL_POLL_INOUT:
		flags = POLLOUT | POLLIN;
	break;
	case CURL_POLL_REMOVE:
	default:
		return 0;
	}

	gp_fds_add(gp_widgets_fds, s, flags, socket_data, userp);

	return 0;
}

static uint32_t timeout_cb(gp_timer *self)
{
	CURLMcode rc;
	int running;
	struct download_dialog *download = self->priv;

	rc = curl_multi_socket_action(download->multi, CURL_SOCKET_TIMEOUT, 0, &running);
	printf(" multi_socket_action(TIMEOUT)! %i\n", rc);

	parse_curl_msg(download);

	return 0;
}

static int timer_cb(CURLM *multi, long timeout_ms, void *userp)
{
	gp_timer *timer = userp;

	if (timeout_ms < 0) {
		gp_widgets_timer_rem(timer);
		return 0;
	}

	if (timer->running)
		gp_widgets_timer_rem(timer);

	timer->expires = timeout_ms;
	gp_widgets_timer_ins(timer);

	return 0;
}

int gp_dialog_download_run(const char *url, const char *dest_file)
{
	CURLMcode res;
	FILE *dest;
	struct download_dialog download = {};
	gp_widget *progress;
	int ret = 0;

	if (curl_global_init(CURL_GLOBAL_ALL)) {
		ret = 1;
		goto err0;
	}

	download.dialog.layout = load_download_layout(&download.dialog, url, dest_file, &progress);
	if (!download.dialog.layout) {
		ret = 1;
		goto err0;
	}

	dest = fopen(dest_file, "wb");
	if (!dest) {
		gp_dialog_msg_printf_run(GP_DIALOG_MSG_ERR,
		                         "Failed to open file",
		                         "%s: %s", dest_file, strerror(errno));
		ret = 2;
		goto err1;
	}

	download.easy = curl_easy_init();
	download.multi = curl_multi_init();
	if (!download.easy || !download.multi) {
		ret = 3;
		goto err3;
	}

	gp_timer timeout_timer = {
		.callback = timeout_cb,
		.priv = &download,
		.id = "CURL timeout",
	};

	/* Set callbacks to add socket to poll() and start a timer */
	curl_multi_setopt(download.multi, CURLMOPT_SOCKETFUNCTION, socket_cb);
	curl_multi_setopt(download.multi, CURLMOPT_SOCKETDATA, &download);
	curl_multi_setopt(download.multi, CURLMOPT_TIMERFUNCTION, timer_cb);
	curl_multi_setopt(download.multi, CURLMOPT_TIMERDATA, &timeout_timer);

	/* sets URL */
	curl_easy_setopt(download.easy, CURLOPT_URL, url);
	/* enable progressbar */
	curl_easy_setopt(download.easy, CURLOPT_XFERINFOFUNCTION, xferinfo);
	curl_easy_setopt(download.easy, CURLOPT_XFERINFODATA, progress);
	curl_easy_setopt(download.easy, CURLOPT_NOPROGRESS, 0L);
	/* write to a file */
	//curl_easy_setopt(download.easy, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(download.easy, CURLOPT_WRITEDATA, dest);
	//BE VERBOSE
	curl_easy_setopt(download.easy, CURLOPT_VERBOSE, 1L);

	res = curl_multi_add_handle(download.multi, download.easy);
	printf("%i\n", res);

	gp_dialog_run(&download.dialog);

	curl_multi_remove_handle(download.multi, download.easy);
	curl_easy_cleanup(download.easy);
	curl_multi_cleanup(download.multi);
	curl_global_cleanup();

	if (fclose(dest)) {
		ret = 4;
		goto err2;
	}

	gp_widget_free(download.dialog.layout);

	return 0;
err3:
	curl_easy_cleanup(download.easy);
	curl_multi_cleanup(download.multi);
	curl_global_cleanup();
	fclose(dest);
err2:
	unlink(dest_file);
err1:
	gp_widget_free(download.dialog.layout);
err0:
	return ret;
}
