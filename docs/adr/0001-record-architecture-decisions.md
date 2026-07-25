# ADR-0001: Record architecture decisions

## Status

Accepted — 2026-07-25

## Context

X12 will make irreversible protocol, security, and compatibility choices. Ad-hoc chat decisions will be lost; the stakeholder asked to **document all decisions**.

## Decision

Use Architecture Decision Records (ADRs) under `docs/adr/`:

- Numbered `NNNN-title.md`
- Status: Proposed | Provisional | Accepted | Superseded | Rejected
- Sections: Context, Decision, Consequences, Alternatives, References
- Index maintained in `docs/DECISIONS.md`
- Strategy-impacting answers from `docs/QUESTIONS.md` promote Provisional → Accepted

## Consequences

- Every non-trivial fork in the road gets a short written record.
- PRs that change architecture must update or add an ADR.
- Provisional decisions may guide early work but cannot silently become permanent.

## Alternatives

- Wiki-only notes — rejected (not versioned with code).
- Issue tracker only — rejected (hard to audit as a set).
