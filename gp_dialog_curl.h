// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * Copyright (C) 2023 Cyril Hrubis <metan@ucw.cz>
 */

#ifndef GP_DIALOG_CURL
#define GP_DIALOG_CURL

typedef enum gp_dialog_download_res {
	/* File downloaded fine */
	GP_DIALOG_DOWNLOAD_OK = 0,
	/* Operation canceled by user */
	GP_DIALOG_DOWNLOAD_CANCELED = 1,
	/* Failed to open file */
	GP_DIALOG_DOWNLOAD_ERR_OPEN = 2,
	/* File coldn't be written */
	GP_DIALOG_DOWNLOAD_ERR_WRITE = 4,
	/* Download failed */
	GP_DIALOG_DOWNLOAD_FAILED = 5,
	/* CURL internal errors, allocation failures etc */
	GP_DIALOG_DOWNLOAD_ERR = 6,
} gp_dialog_download_res;

/**
 * @brief Runs a file download dialog.
 *
 * @url An URL to download the file from
 * @file A destination for the downloaded file
 * @ret Returns the end result as enum gp_dialog_download_res
 */
gp_dialog_download_res gp_dialog_download_run(const char *url, const char *file);

#endif /* GP_DIALOG_CURL */
