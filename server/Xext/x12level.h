/* SPDX-License-Identifier: MIT
 *
 * X12 hierarchical client levels (ADR-0003): sandbox < user < full
 */
#ifndef _XSERVER_X12LEVEL_H
#define _XSERVER_X12LEVEL_H

#include <stdint.h>
#include "dix.h"

#define X12_LEVEL_SANDBOX 0
#define X12_LEVEL_USER    1
#define X12_LEVEL_FULL    2

/* Same numbering as X12_PROTO_LEVEL_* / Rust ClientLevel (ADR-0012). */
static inline uint8_t
X12LevelToProto(int level)
{
    if (level <= X12_LEVEL_SANDBOX)
        return X12_LEVEL_SANDBOX;
    if (level >= X12_LEVEL_FULL)
        return X12_LEVEL_FULL;
    return (uint8_t)level;
}

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
