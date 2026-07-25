/* SPDX-License-Identifier: MIT
 *
 * X12 hierarchical client levels (ADR-0003): sandbox < user < full
 */
#ifndef _XSERVER_X12LEVEL_H
#define _XSERVER_X12LEVEL_H

#include "dix.h"

#define X12_LEVEL_SANDBOX 0
#define X12_LEVEL_USER    1
#define X12_LEVEL_FULL    2

/* Default for new clients (CLI -client-level). ADR-0003: full. */
extern int x12DefaultClientLevel;

void X12LevelExtensionInit(void);
int X12LevelOfClient(ClientPtr client);
void X12LevelSetClient(ClientPtr client, int level);

/* Parse "sandbox|user|full"; returns -1 on error. */
int X12LevelParseName(const char *name);

/* Parse comma-separated client indices for -sandbox-clients */
void X12LevelParseSandboxClients(const char *list);

#endif
