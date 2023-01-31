// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * Copyright (C) 2023 Cyril Hrubis <metan@ucw.cz>
 */

#ifndef GP_DIALOG_CURL
#define GP_DIALOG_CURL

/**
 * @brief Runs a file download dialog.
 *
 * @url An URL to download the file from
 * @file A destination for the downloaded file
 */
int gp_dialog_download_run(const char *url, const char *file);

#endif /* GP_DIALOG_CURL */
